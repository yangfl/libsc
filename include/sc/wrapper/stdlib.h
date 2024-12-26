#ifndef SC_WRAPPER_STDLIB_H
#define SC_WRAPPER_STDLIB_H 1

#include <stddef.h>
#include <stdlib.h>

#include "sc/branch.h"
#include "sc/cdefs.h"
#include "sc/Exception.h"

#ifdef __cplusplus
extern "C" {
#endif


SC_API __wur __attribute_artificial__ __attribute_malloc__
__attribute_alloc_size__((1))
inline void *sc_malloc (size_t size) {
  void *ptr = malloc(size);
  if (sc_unlikely(ptr == NULL)) {
    sc_exc_set_errno("malloc");
  }
  return ptr;
}

SC_API __attribute_warn_unused_result__ __attribute_artificial__
__attribute_alloc_size__((2))
inline void *sc_realloc (void *ptr, size_t new_size) {
  void *new_ptr = realloc(ptr, new_size);
  if (sc_unlikely(new_ptr == NULL)) {
    sc_exc_set_errno("realloc");
  }
  return new_ptr;
}


#ifdef __cplusplus
}
#endif

#endif /* SC_WRAPPER_STDLIB_H */
