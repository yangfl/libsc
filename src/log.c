#define _POSIX_THREAD_SAFE_FUNCTIONS

#ifndef SC_HAS_EXECINFO_H
#  ifdef __has_include
#    if __has_include(<execinfo.h>)
#      define SC_HAS_EXECINFO_H 1
#    endif
#  elif defined __GLIBC__
#    define SC_HAS_EXECINFO_H 1
#  endif
#endif

#include <errno.h>
#ifdef SC_HAS_EXECINFO_H
#include <execinfo.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "include/platform/tls.h"
#include "platform/nowide.h"

#include "include/defs.h"
#include "include/log.h"
#include "color.h"

#define SC_LOG_DOMAIN "sc"


SC_IMPL
const char *const sc_log_level_names[SC_LOGLVL_COUNT] = {
  "EMERG", "ALERT", "CRIT", "ERR",
  "WARNING", "NOTICE", "INFO", "DEBUG", "VERBOSE",
};

#ifndef SC_LOG_NO_COLOR

static const char *const sc_log_level_colors[SC_LOGLVL_COUNT] = {
  "5;1;" COLOR_CODE(COLOR_BG_RED) ";"
    COLOR_CODE(COLOR_FG_BRIGHT_WHITE),
  "1;" COLOR_CODE(COLOR_BG_RED) ";"
    COLOR_CODE(COLOR_FG_BRIGHT_WHITE),
  "1;" COLOR_CODE(COLOR_FG_RED),
  "1;" COLOR_CODE(COLOR_FG_RED),
  "1;" COLOR_CODE(COLOR_FG_YELLOW),
  "1;" COLOR_CODE(COLOR_FG_GREEN),
  COLOR_CODE(COLOR_FG_BLUE),
  COLOR_CODE(COLOR_FG_MAGENTA),
  COLOR_CODE(COLOR_FG_CYAN),
};

#endif

static int sc_log_begin_file (
    void *data, const char *log_domain, unsigned int log_level) {
  if_fail (log_level <= SC_LOG_VERBOSE) {
    log_level = SC_LOG_VERBOSE;
  }

  time_t curtime = time(NULL);
  struct tm timeinfo;
  localtime_r(&curtime, &timeinfo);
  char timebuf[64];
  mcsftime(timebuf, sizeof(timebuf), "%b %d %H:%M:%S", &timeinfo);
#ifdef SC_LOG_NO_COLOR
  return fmprintf(
    (FILE *) data, "[%s] %-8s %s: ",
    timebuf, sc_log_level_names[log_level],
    log_domain == NULL || log_domain[0] == '\0' ? "**" : log_domain);
#else
  return fmprintf(
    (FILE *) data, COLOR_SEQ(COLOR_FG_GREEN) "[%s]" RESET_SEQ " "
      SGR_FORMAT_SEQ_START "%-8s" RESET_SEQ " %s: ",
    timebuf, sc_log_level_colors[log_level], sc_log_level_names[log_level],
    log_domain == NULL || log_domain[0] == '\0' ? "**" : log_domain);
#endif
}


__attr_format((printf, 4, 0))
static int sc_vlog_default (
    void *data, const char *log_domain, unsigned int log_level,
    const char *format, va_list arg) {
  int ret = 0;
  if (sc_log_begin_fn != NULL) {
    ret += sc_log_begin_fn(data, log_domain, log_level);
  }
  ret += sc_log_vprint_fn(data, format, arg);
  if (sc_log_end_fn != NULL) {
    ret += sc_log_end_fn(data, log_domain, log_level);
  }
  return ret;
}


SC_IMPL
void *sc_log_fn_data;
SC_IMPL
sc_logevent_fn_t sc_log_begin_fn = sc_log_begin_file;
SC_IMPL
sc_vprint_fn_t sc_log_vprint_fn = (sc_vprint_fn_t) vfmprintf;
SC_IMPL
sc_logevent_fn_t sc_log_end_fn = (sc_logevent_fn_t) fflush;
SC_IMPL
int (*sc_vlog_fn) (
  void *data, const char *log_domain, unsigned int log_level,
  const char *format, va_list arg) = sc_vlog_default;


__attribute_constructor__
static void sc_log_fn_data_init (void) {
  sc_log_fn_data = stdout;
}


SC_IMPL
int sc_log_print_fn (void *data, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  int ret = sc_log_vprint_fn(data, format, arg);
  va_end(arg);
  return ret;
}


SC_IMPL
unsigned int sc_log_level = SC_LOG_NOTICE;


SC_IMPL
int sc_vlog (
    const char *log_domain, unsigned int log_level,
    const char *format, va_list arg) {
  return_if_fail (log_level <= sc_log_level) 0;
  return_if_fail (format[0] != '\0') 0;
  return sc_vlog_fn(sc_log_fn_data, log_domain, log_level, format, arg);
}


SC_IMPL
int sc_log (
    const char *log_domain, unsigned int log_level, const char *format, ...) {
  return_if_fail (log_level <= sc_log_level) 0;
  return_if_fail (format[0] != '\0') 0;

  va_list arg;
  va_start(arg, format);
  int ret = sc_vlog_fn(sc_log_fn_data, log_domain, log_level, format, arg);
  va_end(arg);
  return ret;
}


SC_IMPL
int sc_vlogger (void *data, const char *format, va_list arg) {
  const struct ScLogger *logger = data;
  return sc_vlog(logger->log_domain, logger->log_level, format, arg);
}


SC_IMPL
int sc_logger (void *data, const char *format, ...) {
  const struct ScLogger *logger = data;
  va_list arg;
  va_start(arg, format);
  int ret = sc_vlog(logger->log_domain, logger->log_level, format, arg);
  va_end(arg);
  return ret;
}

#ifndef SC_HAS_EXECINFO_H

SC_IMPL
int sc_save_backtrace (void **frames, int size, int skip) {
  (void) frames;
  (void) size;
  (void) skip;
  return 0;
}


SC_IMPL
int sc_print_backtrace_f (
    void *const *frames, int size, sc_print_fn_t printer, void *data,
    const char *indent) {
  (void) frames;
  (void) size;
  (void) printer;
  (void) data;
  (void) indent;
  return 0;
}


SC_IMPL
int sc_print_backtrace (
    void *const *frames, int size, FILE *out, const char *indent) {
  (void) frames;
  (void) size;
  (void) out;
  (void) indent;
  return 0;
}

#else

SC_IMPL
int sc_save_backtrace (void **frames, int size, int skip) {
  skip++;
  void *frames_[size + skip];
  int frames_len = backtrace(frames_, size) - skip;
  if (frames_len <= 0) {
    sc_error("(no available backtrace)\n");
  } else {
    memcpy(frames, frames_ + skip, sizeof(frames[0]) * frames_len);
  }
  return frames_len;
}


SC_IMPL
int sc_print_backtrace_f (
    void *const *frames, int size, sc_print_fn_t printer, void *data,
    const char *indent) {
  if (indent == NULL) {
    indent = "";
  }

  int ret = printer(data, "%sTraceback:\n", indent);
  if_fail (size > 0) {
    ret += printer(data, "%s  (no available backtrace)\n", indent);
    return ret;
  }

  char **symbols = backtrace_symbols(frames, size);
  if_fail (symbols != NULL) {
    ret += printer(data, "%s  (backtrace_symbols() error)\n", indent);
    return ret;
  }

  for (int i = 0; i < size; i++) {
    ret += printer(data, "%s  %s\n", indent, symbols[i]);
    if (strstr(symbols[i], "(main+") != NULL) {
      break;
    }
  }
  free(symbols);

  return ret;
}


SC_IMPL
int sc_print_backtrace (
    void *const *frames, int size, FILE *out, const char *indent) {
  return sc_print_backtrace_f(
    frames, size, (sc_print_fn_t) fmprintf, out, indent);
}

#endif

SC_IMPL
thread_local struct ScException sc_exc = {0};


struct ScExceptionHandler {
  unsigned long domain;
  sc_strerror_fn_t strfn;
  sc_print_err_fn_t printfn;
};


static struct ScExceptionHandler sc_exc_handlers[32] = {0};
static unsigned int sc_exc_handlers_len = 0;


SC_IMPL
int ScException_print (
    const struct ScException *exc, sc_print_fn_t printer, void *data,
    const char *indent, const char *bt_indent) {
  if (indent == NULL) {
    indent = "";
  }

  int ret = 0;
  if (exc->fn == NULL) {
    ret += printer(data, "%serror: ", indent);
  } else {
    ret += printer(data, "%sat %s(): ", indent, exc->fn);
  }

  const struct ScExceptionHandler *handler = NULL;
  if (exc->domain != 0) {
    for (unsigned int i = 0; i < sc_exc_handlers_len; i++) {
      const struct ScExceptionHandler *h = sc_exc_handlers + i;
      if (h->domain == exc->domain) {
        handler = h;
        break;
      }
    }
  }

  if (exc->domain == 0) {
    ret += printer(data, "%s\n", strerror(exc->code));
#ifdef _WIN32
  } else if (exc->domain == 1) {
    ret += printer(data, "%s\n", win32_strerror(exc->code));
#endif
  } else if (handler != NULL && handler->strfn != NULL) {
    ret += printer(data, "%s\n", handler->strfn(exc->code));
  } else {
    ret += printer(data, "0x%08lx, %d\n", exc->domain, exc->code);
  }

  if (handler != NULL && handler->printfn != NULL) {
    ret += handler->printfn(exc, printer, data, indent);
  } else if (exc->what != NULL) {
    ret += printer(data, "%s  %s\n", indent, exc->what);
  }

  if (sc_log_level >= SC_LOG_DEBUG) {
    ret += sc_print_backtrace_f(
      exc->frames, exc->frames_len, printer, data, bt_indent);
  }

  return ret;
}


SC_IMPL
int ScException_stderr (
    const struct ScException *exc, const char *format, ...) {
  int ret;

  if (format == NULL) {
    ret = fprintf(stderr, "Unhandled error:\n");
  } else {
    va_list args;
    va_start(args, format);
    ret = fprintf(stderr, format, args);
    va_end(args);
  }
  ret += ScException_print(exc, (sc_print_fn_t) fprintf, stderr, "  ", "");

  if (sc_log_level >= SC_LOG_DEBUG) {
    ret += fprintf(stderr, "\nUnhandled at:\n");

    void *frames[64];
    int frames_len = sc_save_backtrace(frames, arraysize(frames), 1);
    ret += sc_print_backtrace_f(
      frames, frames_len, (sc_print_fn_t) fprintf, stderr, "");
  }

  return ret;
}


SC_IMPL
int sc_print_err (
    FILE *out, const char *indent, const char *bt_indent) {
  return ScException_print(
    &sc_exc, (sc_print_fn_t) fmprintf, out, indent, bt_indent);
}
