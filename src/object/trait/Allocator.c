#include <stddef.h>

#include "sc/object/trait/Allocator.h"


extern inline void *ScClosure_call_malloc (
  const struct ScClosure *closure, size_t size);
extern inline void *ScClosure_call_realloc (
  const struct ScClosure *closure, void *ptr, size_t new_size);
extern inline int ScClosure_call_irealloc (
  const struct ScClosure *closure, void *ptrp, size_t new_size);
extern inline void ScClosure_call_free (
  const struct ScClosure *closure, void *ptr);
