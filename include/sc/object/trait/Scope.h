#ifndef SC_OBJECT_TRAIT_SCOPE_H
#define SC_OBJECT_TRAIT_SCOPE_H 1

#include "sc/cdefs.h"
#include "sc/Closure.h"
#include "sc/object/Trait.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef int (*sc_enter_t) (void *obj, void *data);
typedef void (*sc_exit_t) (void *obj, void *data);

SC_API __attribute_artificial__ __attr_access((__read_only__, 1))
/**
 * @brief Enter a scope / Initialize an object.
 *
 * @param closure Closure.
 * @param obj Object.
 * @return 0 if success.
 */
inline int ScClosure_call_enter (
    const struct ScClosure *__restrict closure, void *obj) {
  return sc_likely(closure == NULL) ? 0 :
    ScClosure_call(closure, sc_enter_t, obj);
}

SC_API __attribute_artificial__ __attr_access((__read_only__, 1))
/**
 * @brief Quit a scope / Destroy an object.
 *
 * @param closure Closure.
 * @param obj Object.
 */
inline void ScClosure_call_exit (
    const struct ScClosure *__restrict closure, void *obj) {
  if (sc_unlikely(closure != NULL)) {
    ScClosure_call(closure, sc_exit_t, obj);
  }
}


#define ScTraitScope_NSID 0x1u

/// Abstract the scope.
struct ScTraitScope {
  struct ScClosure *enter;
  struct ScClosure *exit;
};


SC_API __nonnull((1))
int sc_trait_scope_memzero (void *obj, void *data) __THROW;


#ifdef __cplusplus
}
#endif

#endif /* SC_OBJECT_TRAIT_SCOPE_H */
