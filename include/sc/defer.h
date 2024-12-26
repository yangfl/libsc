#ifndef SC_DEFER_H
#define SC_DEFER_H 1

#include "sc/cdefs.h"


#define SC_DEFER_CONCAT(a, b) a ## b
#define SC_DEFER_(var, func) \
  __extension__ auto void func (); \
  char __attribute_maybe_unused__ __attr_cleanup((func)) var; \
  void func ()
#define SC_DEFER(name) SC_DEFER_( \
  SC_DEFER_CONCAT(__sc_defer_var_, name), \
  SC_DEFER_CONCAT(__sc_defer_, name))
#define defer SC_DEFER(__LINE__)


#endif /* SC_DEFER_H */
