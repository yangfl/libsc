#ifndef SC_CDEFS_H
#define SC_CDEFS_H 1


#ifdef SC_HAS_SYS_CDEFS_H
#  include <sys/cdefs.h>
#elif defined __has_include
#  if __has_include(<sys/cdefs.h>)
#    include <sys/cdefs.h>
#  endif
#endif


#ifndef __always_inline
#  define __always_inline inline __attribute__((__always_inline__))
#endif
#ifndef __attr_access
#  define __attr_access(x) __attribute__((__access__ x))
#endif
#ifndef __attr_access_none
#  define __attr_access_none(argno) __attr_access((__none__, argno))
#endif
#ifndef __attr_cleanup
#  define __attr_cleanup(x) __attribute__((__cleanup__ x))
#endif
#ifndef __attr_format
#  define __attr_format(x) __attribute__((__format__ x))
#endif
#ifndef __attribute_artificial__
#  define __attribute_artificial__ __attribute__((__artificial__))
#endif
#ifndef __attribute_const__
#  define __attribute_const__ __attribute__((__const__))
#endif
#ifndef __COLD
#  define __COLD __attribute__((__cold__))
#endif
#ifndef __attribute_constructor__
#  define __attribute_constructor__ __attribute__((__constructor__))
#endif
#ifndef __attr_dealloc
#  define __attr_dealloc(dealloc, argno) \
    __attribute__((__malloc__(dealloc, argno)))
#endif
#ifndef __attr_dealloc_fclose
#  define __attr_dealloc_fclose __attr_dealloc(fclose, 1)
#endif
#ifndef __attribute_malloc__
#  define __attribute_malloc__ __attribute__((__malloc__))
#endif
#ifndef __attribute_alloc_size__
#  define __attribute_alloc_size__(params) \
    __attribute__((__alloc_size__ params))
#endif
#ifndef __attribute_maybe_unused__
#  define __attribute_maybe_unused__ __attribute__((__unused__))
#endif
#ifndef __attribute_noinline__
#  define __attribute_noinline__ __attribute__((__noinline__))
#endif
#ifndef __attribute_pure__
#  define __attribute_pure__ __attribute__((__pure__))
#endif
#ifndef __attribute_warn_unused_result__
#  define __attribute_warn_unused_result__ \
    __attribute__((__warn_unused_result__))
#endif
#ifndef __wur
#  define __wur __attribute_warn_unused_result__
#endif
#ifndef __attribute_weak__
#  if defined _WIN32 || defined __CYGWIN__
#    define __attribute_weak__
#  else
#    define __attribute_weak__ __attribute__((__weak__))
#  endif
#endif
#ifndef __nonnull
#  define __nonnull(params) __attribute__((__nonnull__ params))
#endif
#ifndef __packed
#  define __packed __attribute__((__packed__))
#endif
#ifndef __THROW
#  if defined __cplusplus
#    if __cplusplus >= 201103L
#      define __THROW noexcept(true)
#    else
#      define __THROW throw()
#   endif
#  else
#    define __THROW __attribute__((__nothrow__, __leaf__))
#  endif
#endif
#ifndef __returns_nonnull
#  define __returns_nonnull __attribute__((__returns_nonnull__))
#endif


#ifndef SC_API
#  if defined _WIN32 || defined __CYGWIN__
#    ifdef SC_BUILDING_DLL
#      define SC_API __declspec(dllexport)
#    elif defined SC_BUILDING_STATIC
#      define SC_API
#    else
#      define SC_API __declspec(dllimport)
#    endif
#  else
#    define SC_API __attribute__((visibility("default")))
#  endif
#endif
#ifndef SC_IMPL
#  define SC_IMPL __attribute_weak__
#endif


#endif /* SC_CDEFS_H */
