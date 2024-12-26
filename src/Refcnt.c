#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "sc/Refcnt.h"


extern inline bool sc_refcnt_dec (atomic_int *refcnt);
extern inline void sc_refcnt_inc (atomic_int *refcnt);
extern inline bool ScRefcnt_release (struct ScRefcnt *refcnt, void *obj);
extern inline void *ScRefcnt_acquire (struct ScRefcnt *refcnt, void *obj);


SC_IMPL
void sc_refcnt_out_of_range (const atomic_int *refcnt, bool underflow) {
  fprintf(
    stderr, "Reference count %s at %p\n", underflow ? "underflow" : "overflow",
    (void *) refcnt);
  // backtrace
  exit(255);
}


SC_IMPL
bool __ScRefcnt_release (struct ScRefcnt *refcnt, void *obj) {
  struct ScClosure *destroy = refcnt->destroy;
  if (sc_unlikely(destroy != NULL)) {
    ScClosure_call(destroy, void (*) (void *, void *), obj);
    ScClosure_release(destroy);
  }

  struct ScClosure *free_ = refcnt->free;
  if (sc_likely(free_ == NULL)) {
    free(obj);
  } else {
    ScClosure_call(free_, void (*) (void *, void *), obj);
    ScClosure_release(free_);
  }

  return true;
}
