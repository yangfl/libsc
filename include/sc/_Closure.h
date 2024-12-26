#ifndef SC__CLOSURE_H
#define SC__CLOSURE_H 1

#include <stdbool.h>
#include <sys/types.h>

#include "sc/cdefs.h"
#include "sc/_Refcnt.h"

#ifdef __cplusplus
extern "C" {
#endif


#define SC_CLOSURE_DISABLE_WARNING \
  _Pragma("GCC diagnostic push") \
  _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
  _Pragma("GCC diagnostic ignored \"-Wstrict-prototypes\"")
#define SC_CLOSURE_ENABLE_WARNING _Pragma("GCC diagnostic pop")

typedef int (*ScFunc) (void);

struct ScClosure {
  ScFunc func;
  void *data;
  ssize_t offset;
  struct ScRefcnt refcnt;
};

#define __ScClosure_call(closure, type, self, ...) (((type) (closure)->func)( \
  (void *) ((char *) (self) - (closure)->offset), ## __VA_ARGS__))
#define ScClosure_call(closure, type, ...) \
  __ScClosure_call(closure, type, ## __VA_ARGS__, (closure)->data)
#define ScClosure_call_(closure, rettype, ...) \
  ScClosure_call(closure, rettype (*) (), ## __VA_ARGS__)

SC_API __attribute_artificial__ __nonnull()
inline bool ScClosure_release (struct ScClosure *closure);
SC_API __attribute_noinline__ __nonnull()
bool __ScClosure_release (struct ScClosure *closure);
SC_API __attribute_artificial__ __nonnull()
inline struct ScClosure *ScClosure_acquire (struct ScClosure *closure);


#ifdef __cplusplus
}
#endif

#endif /* SC__CLOSURE_H */
