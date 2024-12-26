#include <stddef.h>

#include "sc/cdefs.h"
#include "sc/system/execinfo.h"


SC_IMPL
int backtrace (void **buffer, int size) {
  (void) buffer;
  (void) size;
  return 0;
}


SC_IMPL
char **backtrace_symbols (void *const *buffer, int size) {
  (void) buffer;
  (void) size;
  return NULL;
}


SC_IMPL
void backtrace_symbols_fd (void *const *buffer, int size, int fd) {
  (void) buffer;
  (void) size;
  (void) fd;
}
