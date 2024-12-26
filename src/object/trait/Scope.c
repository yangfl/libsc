#include <stddef.h>
#include <string.h>

#include "sc/cdefs.h"
#include "sc/object/trait/Scope.h"


extern inline int ScClosure_call_enter (
  const struct ScClosure *closure, void *obj);
extern inline void ScClosure_call_exit (
  const struct ScClosure *closure, void *obj);


SC_IMPL
int sc_trait_scope_memzero (void *obj, void *data) {
  memset(obj, 0, (size_t) data);
  return 0;
}
