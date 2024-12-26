#ifndef SC_MACRO_H
#define SC_MACRO_H 1


#ifndef arraysize
#  define arraysize(a) (sizeof(a) / sizeof(a[0]))
#endif


#ifndef container_of
#  define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - offsetof(type, member)))
#endif


#ifndef max
#  define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#  define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef clamp
#  define clamp(value, left, right) min(max(value, left), right)
#endif


#endif /* SC_MACRO_H */
