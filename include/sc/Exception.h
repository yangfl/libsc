#ifndef SC_EXCEPTION_H
#define SC_EXCEPTION_H 1

#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "sc/system/execinfo.h"
#include "sc/system/tls.h"

#include "sc/branch.h"
#include "sc/cdefs.h"
#include "sc/errno.h"
#include "sc/Closure.h"

#ifdef __cplusplus
extern "C" {
#endif


/// The exception class.
struct ScException {
  /// Error code domain ID.
  unsigned long domain;
  /// Error code.
  int code;
  /// Name of callee function that caused the exception, if any.
  const char *callee;

  /// Error message.
  const char *what;
  union {
    /// Userdata pointer.
    void *userdata;
    /// Userdata buffer, as a string.
    char str[128];
    /// Userdata buffer.
    unsigned char buf[128];
  };
  /// Userdata destroyer.
  struct ScClosure *destroy;

  /// Capacity of @ref ScException::frames .
  int frames_capacity;
  /// Length of @ref ScException::frames .
  int frames_len;
  /// Stack frames.
  void *frames[];
};

SC_API __nonnull()
/**
 * @brief Free exception resources.
 *
 * @param exc Exception object.
 */
inline void ScException_destroy (struct ScException *exc) {
  if (sc_unlikely(exc->destroy != NULL)) {
    ScClosure_call(exc->destroy, void (*) (void *, void *), &exc->userdata);
    exc->destroy = NULL;
  }
}

SC_API __nonnull((1)) __attr_access_none((4)) __attr_access_none((5))
/**
 * @brief Set exception.
 *
 * @param exc Exception object.
 * @param domain Error code domain ID.
 * @param code Error code.
 * @param callee Name of callee function that caused the exception, if any.
 * @param what Error message.
 * @return 0.
 */
inline int ScException_init (
    struct ScException *__restrict exc, unsigned long domain, int code,
    const char *callee, const char *what) {
  exc->domain = domain;
  exc->code = code;
  exc->callee = callee;
  exc->what = what;
  if (exc->frames_capacity > 0) {
    exc->frames_len = backtrace(exc->frames, exc->frames_capacity);
  }
  return 0;
}


SC_API __attribute_warn_unused_result__ __attribute_const__
char *sc_strerror (int code) __THROW;
#ifdef _WIN32
SC_API __attribute_warn_unused_result__ __attribute_const__
char *sc_strerror_win32 (int code) __THROW;
#endif

__attribute_warn_unused_result__
typedef char *(*sc_strerror_fn_t) (int code);

struct ScExceptionHandler {
  unsigned long domain;
  sc_strerror_fn_t strfn;
};

SC_API __attribute_warn_unused_result__ __attribute_const__
const struct ScExceptionHandler *sc_exc_handler_get (unsigned long domain)
__THROW;
SC_API
int sc_exc_handler_set (unsigned long domain, sc_strerror_fn_t strfn) __THROW;


SC_API
/// The default exception.
extern thread_local struct ScException *sc_exc;

SC_API
/// Catched ignorable exception.
extern thread_local struct ScException sc_exc_null;

SC_API __returns_nonnull
inline struct ScException *sc_exc_try_catch (void) {
  struct ScException *saved_exc = sc_exc;
  sc_exc = &sc_exc_null;
  return saved_exc;
}

SC_API __COLD __returns_nonnull
struct ScException *sc_exc_init (void) __THROW;

SC_API __returns_nonnull
inline struct ScException *sc_exc_get (void) {
  return sc_likely(sc_exc != NULL) ? sc_exc : sc_exc_init();
}

SC_API __COLD __attr_access_none((3)) __attr_access_none((4))
/**
 * @brief Set exception.
 *
 * @param domain Error code domain ID.
 * @param code Error code.
 * @param callee Name of callee function that caused the exception, if any.
 * @param what Error message.
 * @return @c -code .
 */
inline int __sc_exc_set (
    unsigned long domain, int code, const char *callee, const char *what) {
  struct ScException *exc = sc_exc_get();
  ScException_destroy(exc);
  ScException_init(exc, domain, code, callee, what);
  return -code;
}

SC_API __COLD __attr_access_none((3))
__attr_access((__read_only__, 4)) __attr_format((gnu_printf, 4, 5))
int __sc_exc_setfmt (
  unsigned long domain, int code, const char *callee,
  const char *__restrict format, ...) __THROW;

#define sc_exc_set(code, callee, what) __sc_exc_set(SC_ED, code, callee, what)
#define sc_exc_setfmt(code, callee, ...) \
  __sc_exc_setfmt(SC_ED, code, callee, __VA_ARGS__)

__COLD
inline int sc_exc_set_sc (int code) {
  return __sc_exc_set(SC_ED_SC, code, NULL, NULL);
}

SC_API __COLD __attr_access_none((1))
inline int sc_exc_set_errno (const char *callee) {
  return __sc_exc_set(SC_ED_STD, errno, callee, NULL);
}

#ifdef _WIN32

SC_API __COLD __attr_access_none((1))
inline int sc_exc_set_win32 (const char *callee) {
  return __sc_exc_set(SC_ED_WIN32, GetLastError(), callee, NULL);
}

#endif


#ifdef __cplusplus
}
#endif

#endif /* SC_EXCEPTION_H */
