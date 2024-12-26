#ifndef SC_OBJECT_TRAIT_SUBSCRIPTABLE_H
#define SC_OBJECT_TRAIT_SUBSCRIPTABLE_H 1

#include "sc/cdefs.h"
#include "sc/Closure.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ScTraitSubscriptable_NSID 0x3u

struct ScTraitSubscriptable {
  struct ScClosure *__restrict at;
};


#ifdef __cplusplus
}
#endif

#endif /* SC_OBJECT_TRAIT_SUBSCRIPTABLE_H */
