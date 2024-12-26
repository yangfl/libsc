#ifndef SC_ARRAY_H
#define SC_ARRAY_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "sc/system/stdbit.h"

#include "sc/branch.h"
#include "sc/cdefs.h"
#include "sc/errno.h"
#include "sc/Exception.h"
#include "sc/object/Trait.h"
#include "sc/object/trait/Allocator.h"
#include "sc/object/trait/Scope.h"

#ifdef __cplusplus
extern "C" {
#endif


/// Dynamic array.
struct ScArray {
  /// Pointer to the element data.
  void *data;
  /**
   * @brief Number of elements, not including the terminating zero element.
   */
  size_t len;
  /**
   * @brief Number of elements allocated, not including the terminating zero
   * element.
   */
  size_t capacity;

  /// Size of each element.
  size_t elt_size;
  /// Element initializer.
  struct ScTraitScope elt_initializer;
  /// Element allocator.
  struct ScTraitAllocator elt_allocator;
  /// Whether the array is null-terminated.
  bool null_terminated;

  /// User data, not modified by the library.
  void *userdata;
};

SC_API __attribute_artificial__ __nonnull()
/**
 * @brief Return pointer to the element at specified location. No bounds
 * checking is performed.
 *
 * @param arr The array.
 * @param pos Position of the element to return.
 * @return Pointer to the element.
 */
inline void *ScArray_get (const struct ScArray *arr, size_t pos) {
  return (char *) arr->data + pos * arr->elt_size;
}

SC_API __attribute_artificial__ __nonnull()
/**
 * @brief Access the last element.
 *
 * Calling on an empty container causes undefined behavior.
 *
 * @param arr The array.
 * @return Pointer to the element.
 */
inline void *ScArray_back (const struct ScArray *arr) {
  return ScArray_get(arr, arr->len - 1);
}

SC_API __attribute_artificial__ __nonnull()
/**
 * @brief Return pointer to the element at specified location, with bounds
 * checking.
 *
 * @param arr The array.
 * @param pos Position of the element to return. May be negative.
 * @return Pointer to the element, or @c NULL if out of bounds.
 */
inline void *ScArray_at (const struct ScArray *arr, ssize_t pos) {
  if (pos >= 0) {
    if (sc_likely((size_t) pos < arr->len)) {
      return ScArray_get(arr, pos);
    }
  } else {
    if (sc_likely((size_t) -pos <= arr->len)) {
      return ScArray_get(arr, arr->len + pos);
    }
  }
  sc_exc_set_sc(SC_ERANGE);
  return NULL;
}

SC_API __nonnull()
/**
 * @brief Set the array capacity to exactly @p new_cap . If @p new_cap is
 * greater than the current capacity, new storage is allocated, otherwise unused
 * capacity is freed.
 *
 * This function does not change the length of the array.
 *
 * If <tt>new_cap < arr->len</tt>, the behavior is undefined.
 *
 * @param arr The array.
 * @param new_cap The new capacity.
 * @return 0 if success.
 */
inline int ScArray_set_capacity (struct ScArray *arr, size_t new_cap) {
  if (sc_unlikely(new_cap == arr->capacity)) {
    return 0;
  }

  int ret = ScTrait_call_using(
    &arr->elt_allocator, realloc, irealloc, &arr->data,
    (new_cap + 1) * arr->elt_size);
  if (sc_unlikely(ret < 0)) {
    return ret;
  }

  arr->capacity = new_cap;
  return 0;
}

SC_API __attribute_artificial__ __nonnull()
/**
 * @brief Request the removal of unused capacity.
 *
 * @param arr The array.
 */
inline void ScArray_shrink (struct ScArray *arr) {
  if (sc_unlikely(arr->capacity == arr->len)) {
    return;
  }

  struct ScException *saved_exc = sc_exc_try_catch();
  ScArray_set_capacity(arr, arr->len);
  sc_exc = saved_exc;
}

__attribute_artificial__
static inline size_t __sc_align_memsize (size_t x) {
  size_t part = 1ull << (sizeof(x) * 8 - stdc_leading_zeros(x) - 1);
  if (sc_unlikely(part > 4096 * 16)) {
    part = 4096 * 16;
  }
  return (x + part - 1) & ~(part - 1);
}

SC_API __attribute_artificial__ __nonnull()
/**
 * @brief Increase the capacity of the array (the total number of elements that
 * the array can hold without requiring reallocation) to a value that's greater
 * or equal to @p new_cap. If @p new_cap is greater than the current capacity,
 * new storage is allocated, otherwise the function does nothing.
 *
 * This function does not change the length of the array.
 *
 * @param arr The array.
 * @param new_cap The new capacity.
 * @return 0 if success.
 */
inline int ScArray_reserve (struct ScArray *arr, size_t new_cap) {
  if (sc_unlikely(new_cap <= arr->capacity)) {
    return 0;
  }

  size_t new_size_aligned = __sc_align_memsize((new_cap + 1) * arr->elt_size);
  size_t new_cap_aligned;
  if (sc_likely(arr->elt_size == 1)) {
    new_cap_aligned = new_size_aligned - 1;
  } else {
    new_cap_aligned = new_size_aligned / arr->elt_size - 1;
  }
  return ScArray_set_capacity(arr, new_cap_aligned);
}

static inline void __ScArray_ensure_null_terminated (struct ScArray *arr) {
  if (sc_likely(!arr->null_terminated)) {
    return;
  }

  void *end = ScArray_get(arr, arr->len);
  if (sc_likely(arr->elt_size == 1)) {
    *(unsigned char *) end = 0;
  } else {
    memset(end, 0, arr->elt_size);
  }
}

SC_API __attribute_artificial__ __nonnull()
/**
 * @brief Push constructed element in-place at the end. The caller must
 * initialize the element before or immediately after.
 *
 * @param arr The array.
 * @return 0 if success.
 */
inline int ScArray_push (struct ScArray *arr) {
  int ret = ScArray_reserve(arr, arr->len + 1);
  if (sc_unlikely(ret < 0)) {
    return ret;
  }

  arr->len++;
  __ScArray_ensure_null_terminated(arr);
  return 0;
}

SC_API __attribute_artificial__ __nonnull()
/**
 * @brief Pop the last element. The caller must deinitialize or move the element
 * before.
 *
 * Calling on an empty container causes undefined behavior.
 *
 * @param arr The array.
 */
inline void ScArray_pop (struct ScArray *arr) {
  arr->len--;
  __ScArray_ensure_null_terminated(arr);
}

__nonnull()
static inline int __ScArray_set_len (struct ScArray *arr, size_t new_len) {
  arr->len = new_len;
  __ScArray_ensure_null_terminated(arr);
  return 0;
}

SC_API __COLD __nonnull()
int __ScArray_set_len_shrink (struct ScArray *arr, size_t new_len);

SC_API __nonnull()
/**
 * @brief Shrink array length to @p new_len and deinitialize extra elements, but
 * leave @c arr->capacity unchanged.
 *
 * If <tt>new_len > arr->len</tt>, the behavior is undefined.
 *
 * This function always succeeds.
 *
 * @param arr The array.
 * @param new_len The new length.
 * @return 0.
 */
inline int ScArray_set_len_shrink (struct ScArray *arr, size_t new_len) {
  if (ScTrait_has(&arr->elt_initializer, exit)) {
    return __ScArray_set_len_shrink(arr, new_len);
  }
  return __ScArray_set_len(arr, new_len);
}

SC_API __COLD __nonnull()
int __ScArray_set_len_expand (struct ScArray *arr, size_t new_len);

SC_API __nonnull()
/**
 * @brief Expand array length to @p new_len and initialize newly added elements.
 *
 * If <tt>new_len < arr->len</tt>, the behavior is undefined.
 *
 * @param arr The array.
 * @param new_len The new length.
 * @return 0 if success.
 */
inline int ScArray_set_len_expand (struct ScArray *arr, size_t new_len) {
  if (ScTrait_has(&arr->elt_initializer, enter)) {
    return __ScArray_set_len_expand(arr, new_len);
  }

  int ret = ScArray_reserve(arr, new_len);
  if (sc_unlikely(ret < 0)) {
    return ret;
  }

  return __ScArray_set_len(arr, new_len);
}

SC_API __attribute_artificial__ __nonnull()
/**
 * @brief Set array length to @p new_len . If @p new_len is larger than
 * @c arr->len , newly added elements will be initialized. If @p new_len is
 * smaller than @c arr->len , extra elements will be deinitialized, but leave
 * @c arr->capacity unchanged.
 *
 * If <tt>new_len <= arr->len</tt>, this function always succeeds.
 *
 * @param arr The array.
 * @param new_len The new length.
 * @param shrink Whether to free array memory if possible.
 * @return 0 if success.
 */
inline int ScArray_set_len (struct ScArray *arr, size_t new_len) {
  if (sc_unlikely(new_len == arr->len)) {
    return 0;
  } else if (new_len < arr->len) {
    return ScArray_set_len_shrink(arr, new_len);
  } else {
    return ScArray_set_len_expand(arr, new_len);
  }
}

SC_API __attribute_artificial__ __nonnull()
/**
 * @brief Erase all elements from the container. After this call, @c arr->len is
 * zero.
 *
 * Leave @c arr->capacity unchanged.
 *
 * @param arr The array.
 */
inline void ScArray_clear (struct ScArray *arr) {
  ScArray_set_len_shrink(arr, 0);
}

SC_API __attribute_artificial__ __nonnull()
/**
 * @brief Release all resources used by the container. All remaining elements
 * are deinitialized.
 *
 * @param arr The array.
 */
inline void ScArray_destroy (struct ScArray *arr) {
  if (ScTrait_has(&arr->elt_initializer, exit)) {
    __ScArray_set_len_shrink(arr, 0);
  }
  ScTrait_call(&arr->elt_allocator, free, arr->data);
  ScTrait_destroy(&arr->elt_initializer);
  ScTrait_destroy(&arr->elt_allocator);
}

SC_API __attribute_artificial__ __nonnull((1))
inline int ScArray_init (
    struct ScArray *arr, size_t elt_size, bool null_terminated) {
  arr->data = NULL;
  arr->len = 0;
  arr->capacity = 0;
  arr->elt_size = elt_size;
  arr->elt_initializer = (struct ScTraitScope) {0};
  arr->elt_allocator = (struct ScTraitAllocator) {0};
  arr->null_terminated = null_terminated;
  return 0;
}

SC_API __attribute_artificial__ __nonnull((1))
inline int ScArray_init_string (struct ScArray *arr) {
  return ScArray_init(arr, 1, true);
}


#ifdef __cplusplus
}
#endif

#endif /* SC_ARRAY_H */
