#ifndef SC__REFCNT_H
#define SC__REFCNT_H 1

#include <stdatomic.h>
#include <stdbool.h>

#include "sc/cdefs.h"

#ifdef __cplusplus
extern "C" {
#endif


struct ScClosure;

struct ScRefcnt {
  atomic_int refcnt;
  struct ScClosure *destroy;
  struct ScClosure *free;
};

SC_API __COLD __nonnull((1))
bool __ScRefcnt_release (struct ScRefcnt *refcnt, void *obj);
SC_API __attribute_artificial__ __nonnull((1))
inline bool ScRefcnt_release (struct ScRefcnt *refcnt, void *obj);
SC_API __attribute_artificial__ __nonnull((1))
inline void *ScRefcnt_acquire (struct ScRefcnt *refcnt, void *obj);

#define sc_release(obj, attr) ScRefcnt_release(&(obj)->attr, (obj))
#define sc_acquire(obj, attr) (ScRefcnt_acquire(&(obj)->attr, (obj)), (obj))

#define SC_REFCNT_ATTR_NAME refcnt

#define sc_rel(obj) sc_release(obj, SC_REFCNT_ATTR_NAME)
#define sc_acq(obj) sc_acquire(obj, SC_REFCNT_ATTR_NAME)


#ifdef __cplusplus
}
#endif

#endif /* SC__REFCNT_H */
