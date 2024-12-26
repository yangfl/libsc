#ifndef SC_WRAPPER_STDIO_H
#define SC_WRAPPER_STDIO_H 1

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include "sc/branch.h"
#include "sc/cdefs.h"
#include "sc/Exception.h"

#ifdef __cplusplus
extern "C" {
#endif


SC_API __attribute_artificial__ __nonnull((4)) __attr_access((__read_only__, 1))
inline size_t sc_fwrite (
    const void *__restrict buffer, size_t size, size_t count,
    FILE *__restrict stream) {
  size_t ret = fwrite(buffer, size, count, stream);
  if (sc_unlikely(ret != count)) {
    sc_exc_set_errno("fwrite");
  }
  return ret;
}

SC_API __wur __attribute_artificial__ __nonnull((4))
__attr_access((__write_only__, 1))
inline size_t sc_fread (
    void *__restrict buffer, size_t size, size_t count,
    FILE *__restrict stream) {
  size_t ret = fread(buffer, size, count, stream);
  if (sc_unlikely(ret != count)) {
    sc_exc_set_errno("fread");
  }
  return ret;
}

SC_API __attr_access((__read_only__, 2)) __attr_format((gnu_printf, 2, 0))
inline int sc_vfprintf (
    FILE *__restrict stream, const char *__restrict format, va_list args) {
  int ret = vfprintf(stream, format, args);
  if (sc_unlikely(ret < 0)) {
    sc_exc_set_errno("vfprintf");
  }
  return ret;
}

SC_API __wur __attr_access((__read_only__, 2)) __attr_format((gnu_scanf, 2, 0))
inline int sc_vfscanf (
    FILE *__restrict stream, const char *__restrict format, va_list args) {
  int ret = vfscanf(stream, format, args);
  if (sc_unlikely(ret < 0)) {
    sc_exc_set_errno("vfscanf");
  }
  return ret;
}


#ifdef __cplusplus
}
#endif

#endif /* SC_WRAPPER_STDIO_H */
