#ifndef SC_OBJECT_TRAIT_ALLOCATOR_H
#define SC_OBJECT_TRAIT_ALLOCATOR_H 1

#include <stddef.h>
#include <stdlib.h>

#include "sc/branch.h"
#include "sc/cdefs.h"
#include "sc/errno.h"
#include "sc/Closure.h"
#include "sc/object/Trait.h"
#include "sc/wrapper/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef void *(*sc_malloc_t) (size_t size, void *data);
typedef void *(*sc_realloc_t) (void *ptr, size_t new_size, void *data);
typedef void (*sc_free_t) (void *ptr, void *data);

SC_API __wur __attribute_artificial__ __attribute_alloc_size__((2))
__attr_access((__read_only__, 1))
inline void *ScClosure_call_malloc (
    const struct ScClosure *__restrict closure, size_t size) {
  return sc_likely(closure == NULL) ? sc_malloc(size) :
    (char *) ((sc_malloc_t) closure->func)(size, closure->data) +
    closure->offset;
}

SC_API __attribute_warn_unused_result__ __attribute_artificial__
__attribute_alloc_size__((3)) __attr_access((__read_only__, 1))
inline void *ScClosure_call_realloc (
    const struct ScClosure *__restrict closure, void *__restrict ptr,
    size_t new_size) {
  return sc_likely(closure == NULL) ? sc_realloc(ptr, new_size) :
    ScClosure_call(closure, sc_realloc_t, ptr, new_size);
}

SC_API __attribute_artificial__ __nonnull((2)) __attr_access((__read_only__, 1))
inline int ScClosure_call_irealloc (
    const struct ScClosure *__restrict closure, void *__restrict ptrp,
    size_t new_size) {
  void *new_ptr = ScClosure_call_realloc(closure, *(void **) ptrp, new_size);
  if (sc_unlikely(new_ptr == NULL)) {
    return -SC_ENOMEM;
  }
  *(void **) ptrp = new_ptr;
  return 0;
}

SC_API __attribute_artificial__ __attr_access((__read_only__, 1))
inline void ScClosure_call_free (
    const struct ScClosure *__restrict closure, void *__restrict ptr) {
  if (sc_likely(closure == NULL)) {
    free(ptr);
  } else {
    ScClosure_call(closure, sc_free_t, ptr);
  }
}


#define ScTraitAllocator_NSID 0x2u

/// Abstract the allocator.
struct ScTraitAllocator {
  struct ScClosure *malloc;
  struct ScClosure *realloc;
  struct ScClosure *free;
};


#ifdef __cplusplus
}
#endif

#endif /* SC_OBJECT_TRAIT_ALLOCATOR_H */
