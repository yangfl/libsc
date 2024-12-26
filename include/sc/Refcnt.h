#ifndef SC_REFCNT_H
#define SC_REFCNT_H 1

#include <limits.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

#include "sc/branch.h"
#include "sc/cdefs.h"
#include "sc/_Refcnt.h"
#include "sc/Closure.h"

#ifdef __cplusplus
extern "C" {
#endif


SC_API __attribute__((__noreturn__)) __COLD
void sc_refcnt_out_of_range (const atomic_int *refcnt, bool underflow);

SC_API __attribute_artificial__ __nonnull()
inline bool sc_refcnt_dec (atomic_int *refcnt) {
  if (sc_unlikely(*refcnt == INT_MIN)) {
    return false;
  }

  int orig = atomic_fetch_sub_explicit(refcnt, 1, memory_order_acq_rel);
  if (sc_unlikely(orig <= 0)) {
    sc_refcnt_out_of_range(refcnt, true);
  }
  return sc_unlikely(orig == 1);
}

SC_API __attribute_artificial__ __nonnull()
inline void sc_refcnt_inc (atomic_int *refcnt) {
  if (sc_unlikely(*refcnt == INT_MIN)) {
    return;
  }

  int orig = atomic_fetch_add_explicit(refcnt, 1, memory_order_acq_rel);
  if (sc_unlikely(orig < 0) || sc_unlikely(orig == INT_MAX)) {
    sc_refcnt_out_of_range(refcnt, sc_unlikely(orig == INT_MAX));
  }
}


inline bool ScRefcnt_release (struct ScRefcnt *refcnt, void *obj) {
  return !sc_refcnt_dec(&refcnt->refcnt) ? false :
    __ScRefcnt_release(refcnt, obj);
}

inline void *ScRefcnt_acquire (struct ScRefcnt *refcnt, void *obj) {
  sc_refcnt_inc(&refcnt->refcnt);
  return obj;
}


#ifdef __cplusplus
}
#endif

#endif /* SC_REFCNT_H */
