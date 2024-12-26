#define _GNU_SOURCE

#include <stddef.h>

#include "sc/cdefs.h"
#include "sc/thread/threadname.h"

#ifndef SC_HAS_PTHREAD_NAME

SC_IMPL
int sc_threadname_get (char *buf, size_t size) {
  (void) buf;
  (void) size;
  return 0;
}


SC_IMPL
int sc_threadname_set (const char *format, ...) {
  (void) format;
  return 0;
}


SC_IMPL
int sc_threadname_append (const char *format, ...) {
  (void) format;
  return 0;
}

#else

#ifndef SC_HAS_PTHREAD_NP_H
#  ifdef __has_include
#    if __has_include(<pthread_np.h>)
#      define SC_HAS_PTHREAD_NP_H 1
#    endif
#  elif defined __OpenBSD__ || defined __NetBSD__ || defined __FreeBSD__ || \
        defined __DragonFly__
#    define SC_HAS_PTHREAD_NP_H 1
#  endif
#endif

#include <pthread.h>
#ifdef SC_HAS_PTHREAD_NP_H
#include <pthread_np.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "sc/branch.h"


SC_IMPL
int sc_threadname_get (char *buf, size_t size) {
  return pthread_getname_np(pthread_self(), buf, size);
}


SC_IMPL
int sc_threadname_set (const char *format, ...) {
  char name[SC_THREADNAME_SIZE];

  va_list ap;
  va_start(ap, format);
  vsnprintf(name, sizeof(name), format, ap);
  va_end(ap);

  return pthread_setname_np(pthread_self(), name);
}


SC_IMPL
int sc_threadname_append (const char *format, ...) {
  pthread_t thread = pthread_self();

  char name[SC_THREADNAME_SIZE];
  int ret = pthread_getname_np(thread, name, sizeof(name));
  if (sc_unlikely(ret != 0)) {
    return ret;
  }
  size_t len = strlen(name);

  va_list ap;
  va_start(ap, format);
  vsnprintf(name + len, sizeof(name) - len, format, ap);
  va_end(ap);

  return pthread_setname_np(thread, name);
}

#endif
