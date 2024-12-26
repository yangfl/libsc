#ifndef SC_OBJECT_TRAIT_STREAM_H
#define SC_OBJECT_TRAIT_STREAM_H 1

#include <stdarg.h>
#include <stddef.h>

#include "sc/cdefs.h"
#include "sc/Closure.h"
#include "sc/object/Trait.h"
#include "sc/wrapper/stdio.h"

#ifdef __cplusplus
extern "C" {
#endif


__attr_access((__read_only__, 1))
typedef size_t (*sc_write_fn_t) (
  const void *__restrict buffer, size_t size, size_t count,
  void *__restrict stream, void *data);
__wur __attr_access((__write_only__, 1))
typedef size_t (*sc_read_fn_t) (
  void *__restrict buffer, size_t size, size_t count,
  void *__restrict stream, void *data);
__attr_access((__read_only__, 2)) __attr_format((gnu_printf, 2, 0))
typedef int (*sc_vprintf_fn_t) (
  void *__restrict stream, const char *__restrict format, va_list args,
  void *data);
__wur __attr_access((__read_only__, 2)) __attr_format((gnu_scanf, 2, 0))
typedef int (*sc_vscanf_fn_t) (
  void *__restrict stream, const char *__restrict format, va_list args,
  void *data);

SC_API __attr_access((__read_only__, 1)) __attr_access((__read_only__, 2))
inline size_t ScClosure_call_write (
    const struct ScClosure *__restrict closure,
    const void *__restrict buffer, size_t size, size_t count,
    void *__restrict stream) {
  return sc_likely(closure == NULL) ? sc_fwrite(buffer, size, count, stream) :
    ((sc_write_fn_t) closure->func)(
      buffer, size, count, (char *) stream - closure->offset,
      closure->data);
}

SC_API __wur __attr_access((__read_only__, 1))
__attr_access((__write_only__, 2))
inline size_t ScClosure_call_read (
    const struct ScClosure *__restrict closure,
    void *__restrict buffer, size_t size, size_t count,
    void *__restrict stream) {
  return sc_likely(closure == NULL) ? sc_fread(buffer, size, count, stream) :
    ((sc_read_fn_t) closure->func)(
      buffer, size, count, (char *) stream - closure->offset,
      closure->data);
}

SC_API __attr_access((__read_only__, 1))
__attr_access((__read_only__, 3)) __attr_format((gnu_printf, 3, 0))
inline int ScClosure_call_vprintf (
    const struct ScClosure *__restrict closure,
    void *__restrict stream, const char *__restrict format, va_list args) {
  return sc_likely(closure == NULL) ? sc_vfprintf(stream, format, args) :
    ScClosure_call(closure, sc_vprintf_fn_t, stream, format, args);
}

SC_API __attr_access((__read_only__, 1))
__attr_access((__read_only__, 3)) __attr_format((gnu_printf, 3, 4))
inline int ScClosure_call_printf (
    const struct ScClosure *__restrict closure,
    void *__restrict stream, const char *__restrict format, ...) {
  va_list args;
  va_start(args, format);
  int ret = ScClosure_call_vprintf(closure, stream, format, args);
  va_end(args);
  return ret;
}

SC_API __attr_access((__read_only__, 1))
__attr_access((__read_only__, 3)) __attr_format((gnu_scanf, 3, 0))
inline int ScClosure_call_vscanf (
    const struct ScClosure *__restrict closure,
    void *__restrict stream, const char *__restrict format, va_list args) {
  return sc_likely(closure == NULL) ? sc_vfscanf(stream, format, args) :
    ScClosure_call(closure, sc_vscanf_fn_t, stream, format, args);
}

SC_API __attr_access((__read_only__, 1))
__attr_access((__read_only__, 3)) __attr_format((gnu_scanf, 3, 4))
inline int ScClosure_call_scanf (
    const struct ScClosure *__restrict closure,
    void *__restrict stream, const char *__restrict format, ...) {
  va_list args;
  va_start(args, format);
  int ret = ScClosure_call_vscanf(closure, stream, format, args);
  va_end(args);
  return ret;
}


struct ScTraitStream {
  struct ScClosure *write;
  struct ScClosure *read;
  struct ScClosure *vprintf;
  struct ScClosure *vscanf;
};


#ifdef __cplusplus
}
#endif

#endif /* SC_OBJECT_TRAIT_STREAM_H */
