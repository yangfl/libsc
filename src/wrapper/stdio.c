#include <stddef.h>
#include <stdio.h>

#include "sc/wrapper/stdio.h"


extern inline size_t sc_fwrite (
  const void *buffer, size_t size, size_t count, FILE *stream);
extern inline size_t sc_fread (
  void *buffer, size_t size, size_t count, FILE *stream);
extern inline int sc_vfprintf (FILE *stream, const char *format, va_list args);
extern inline int sc_vfscanf (FILE *stream, const char *format, va_list args);
