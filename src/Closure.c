#include <stdbool.h>

#include "sc/Closure.h"


extern inline bool ScClosure_release (struct ScClosure *closure);
extern inline struct ScClosure *ScClosure_acquire (struct ScClosure *closure);


bool __ScClosure_release (struct ScClosure *closure) {
  return ScClosure_release(closure);
}
