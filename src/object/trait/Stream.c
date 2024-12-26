#include <stddef.h>

#include "sc/object/trait/Stream.h"


extern inline size_t ScClosure_call_write (
  const struct ScClosure *closure, const void * buffer, size_t size,
  size_t count, void * stream);
extern inline size_t ScClosure_call_read (
  const struct ScClosure *closure, void *buffer, size_t size, size_t count,
  void *stream);
extern inline int ScClosure_call_vprintf (
  const struct ScClosure *closure, void *stream, const char *format,
  va_list args);
extern inline int ScClosure_call_printf (
  const struct ScClosure *closure, void *stream, const char *format, ...);
extern inline int ScClosure_call_vscanf (
  const struct ScClosure *closure, void *stream, const char *format,
  va_list args);
extern inline int ScClosure_call_scanf (
  const struct ScClosure *closure, void *stream, const char *format, ...);
