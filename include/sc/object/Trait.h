#ifndef SC_OBJECT_TRAIT_H
#define SC_OBJECT_TRAIT_H 1

#include <stddef.h>

#include "sc/branch.h"
#include "sc/Closure.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ScTrait_has(trait, method) \
  (sc_unlikely((trait) != NULL) && sc_unlikely((trait)->method != NULL))

#define ScTrait_call(trait, method, ...) ScClosure_call_ ## method( \
  sc_likely((trait) == NULL) ? NULL : (trait)->method, __VA_ARGS__);
#define ScTrait_call_using(trait, method, modifier, ...) \
  ScClosure_call_ ## modifier( \
    sc_likely((trait) == NULL) ? NULL : (trait)->method, __VA_ARGS__);

SC_API __attribute_artificial__ __nonnull((1)) __attr_access((__read_only__, 1))
inline void __ScTrait_destroy (const void *trait, size_t n_methods) {
  for (size_t i = 0; i < n_methods; i++) {
    struct ScClosure *closure = ((struct ScClosure *const *) trait)[i];
    if (sc_unlikely(closure != NULL)) {
      __ScClosure_release(closure);
    }
  }
}

#define ScTrait_destroy(trait) \
  __ScTrait_destroy(trait, sizeof(*(trait)) / sizeof(struct ScClosure *))


#ifdef __cplusplus
}
#endif

#endif /* SC_OBJECT_TRAIT_H */
