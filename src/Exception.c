#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "sc/system/tls.h"

#include "sc/branch.h"
#include "sc/errno.h"
#include "sc/cdefs.h"
#include "sc/Exception.h"

#define SC_EXC_FRAMES 256


extern inline void ScException_destroy (struct ScException *exc);
extern inline int ScException_init (
  struct ScException *exc, unsigned long domain, int code,
  const char *callee, const char *what);
extern inline struct ScException *sc_exc_try_catch (void);
extern inline struct ScException *sc_exc_get (void);
extern inline int __sc_exc_set (
  unsigned long domain, int code, const char *callee, const char *what);
extern inline int sc_exc_set_sc (int code);
extern inline int sc_exc_set_errno (const char *callee);
#ifdef _WIN32
extern inline int sc_exc_set_win32 (const char *callee);
#endif


char *sc_strerror (int code) {
  switch (code) {
    case SC_EEXC:
      return (char *) "exception";
    default:
      return NULL;
  }
}

#ifdef _WIN32

char *sc_strerror_win32 (int code) {
  WCHAR ws[256];
  static thread_local char buf[512];

  FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    ws, sizeof(ws) / sizeof(ws[0]), NULL);
  WideCharToMultiByte(CP_UTF8, 0, ws, -1, buf, sizeof(buf), NULL, NULL);

  return buf;
}

#endif

static const struct ScExceptionHandler sc_exc_handlers_static[] = {
  {SC_ED_STD, strerror},
  {SC_ED_SC, sc_strerror},
#ifdef _WIN32
  {SC_ED_WIN32, sc_strerror_win32},
#endif
  {0}
};
static struct ScExceptionHandler sc_exc_handlers[32] = {0};
static unsigned int sc_exc_handlers_len = 0;


SC_IMPL
const struct ScExceptionHandler *sc_exc_handler_get (unsigned long domain) {
  for (unsigned int i = 0; sc_exc_handlers_static[i].domain != 0; i++) {
    const struct ScExceptionHandler *handler = &sc_exc_handlers_static[i];
    if (handler->domain == domain) {
      return handler;
    }
  }

  for (unsigned int i = 0; i < sc_exc_handlers_len; i++) {
    const struct ScExceptionHandler *handler = &sc_exc_handlers[i];
    if (handler->domain == domain) {
      return handler;
    }
  }

  return NULL;
}


SC_IMPL
int sc_exc_handler_set (unsigned long domain, sc_strerror_fn_t strfn) {
  for (unsigned int i = 0; sc_exc_handlers_static[i].domain != 0; i++) {
    const struct ScExceptionHandler *handler = &sc_exc_handlers_static[i];
    if (sc_unlikely(handler->domain == domain)) {
      return sc_exc_set_sc(SC_EINVAL);
    }
  }

  unsigned int i;
  for (i = 0; i < sc_exc_handlers_len; i++) {
    struct ScExceptionHandler *handler = sc_exc_handlers + i;
    if (handler->domain == domain) {
      handler->strfn = strfn;
      return 0;
    }
  }
  if (sc_unlikely(i >= sizeof(sc_exc_handlers) / sizeof(sc_exc_handlers[0]))) {
    return sc_exc_set_sc(SC_ENOMEM);
  }

  struct ScExceptionHandler *handler = &sc_exc_handlers[sc_exc_handlers_len];
  handler->domain = domain;
  handler->strfn = strfn;
  sc_exc_handlers_len++;
  return 0;
}


SC_IMPL
thread_local struct ScException *sc_exc = NULL;
SC_IMPL
thread_local struct ScException sc_exc_null = {0};


SC_IMPL
struct ScException *sc_exc_init (void) {
  static thread_local union {
    struct ScException exc;
    char buf[sizeof(struct ScException) + SC_EXC_FRAMES * sizeof(void *)];
  } sc_exc_tls = {.exc.frames_capacity = SC_EXC_FRAMES};

  sc_exc = &sc_exc_tls.exc;
  return sc_exc;
}


SC_IMPL
int __sc_exc_setfmt (
    unsigned long domain, int code, const char *callee,
    const char *__restrict format, ...) {
  struct ScException *exc = sc_exc_get();
  ScException_destroy(exc);

  const char *what = NULL;
  if (format != NULL) {
    va_list args;
    va_start(args, format);
    vsnprintf(exc->str, sizeof(exc->str), format, args);
    va_end(args);
    what = exc->str;
  }

  ScException_init(exc, domain, code, callee, what);
  return -code;
}
