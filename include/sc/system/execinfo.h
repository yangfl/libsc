#ifndef SC_SYSTEM_EXECINFO_H
#define SC_SYSTEM_EXECINFO_H 1

#ifndef SC_HAS_EXECINFO_H
#  ifdef __has_include
#    if __has_include(<execinfo.h>)
#      define SC_HAS_EXECINFO_H 1
#    endif
#  elif defined __GLIBC__
#    define SC_HAS_EXECINFO_H 1
#  endif
#endif

#ifdef SC_HAS_EXECINFO_H
#  include <execinfo.h>
#else

#include "sc/cdefs.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Store up to SIZE return address of the current program state in
   ARRAY and return the exact number of values stored.  */
extern int backtrace (void **__array, int __size) __nonnull ((1));


/* Return names of functions from the backtrace list in ARRAY in a newly
   malloc()ed memory block.  */
extern char **backtrace_symbols (void *const *__array, int __size)
     __THROW __nonnull ((1));


/* This function is similar to backtrace_symbols() but it writes the result
   immediately to a file.  */
extern void backtrace_symbols_fd (void *const *__array, int __size, int __fd)
     __THROW __nonnull ((1));


#ifdef __cplusplus
}
#endif

#endif

#endif /* SC_SYSTEM_EXECINFO_H */
