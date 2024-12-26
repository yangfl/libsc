#ifndef SC_CLOSURE_H
#define SC_CLOSURE_H 1

#include <stdbool.h>

#include "sc/_Closure.h"
#include "sc/Refcnt.h"

#ifdef __cplusplus
extern "C" {
#endif


inline bool ScClosure_release (struct ScClosure *closure) {
  return sc_rel(closure);
}

inline struct ScClosure *ScClosure_acquire (struct ScClosure *closure) {
  return sc_acq(closure);
}


#ifdef __cplusplus
}
#endif

#endif /* SC_CLOSURE_H */
