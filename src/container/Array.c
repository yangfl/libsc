#include <stddef.h>
#include <sys/types.h>

#include "sc/branch.h"
#include "sc/errno.h"
#include "sc/object/Trait.h"

#include "sc/container/Array.h"


extern inline void *ScArray_get (const struct ScArray *arr, size_t pos);
extern inline void *ScArray_back (const struct ScArray *arr);
extern inline void *ScArray_at (const struct ScArray *arr, ssize_t pos);

extern inline int ScArray_set_capacity (struct ScArray *arr, size_t new_cap);
extern inline void ScArray_shrink (struct ScArray *arr);
extern inline int ScArray_reserve (struct ScArray *arr, size_t new_cap);

extern inline int ScArray_push (struct ScArray *arr);
extern inline void ScArray_pop (struct ScArray *arr);

extern inline int ScArray_set_len_shrink (struct ScArray *arr, size_t new_len);
extern inline int ScArray_set_len_expand (struct ScArray *arr, size_t new_len);
extern inline int ScArray_set_len (struct ScArray *arr, size_t new_len);
extern inline void ScArray_clear (struct ScArray *arr);

extern inline void ScArray_destroy (struct ScArray *arr);
extern inline int ScArray_init (
  struct ScArray *arr, size_t elt_size, bool null_terminated);
extern inline int ScArray_init_string (struct ScArray *arr);


int __ScArray_set_len_shrink (struct ScArray *arr, size_t new_len) {
  if (ScTrait_has(&arr->elt_initializer, exit)) {
    sc_exit_t exit_func = (sc_exit_t) arr->elt_initializer.exit->func;
    void *exit_data = arr->elt_initializer.exit->data;
    size_t old_len = arr->len;

    for (size_t i = new_len; i < old_len; i++) {
      exit_func(ScArray_get(arr, i), exit_data);
    }
  }
  return __ScArray_set_len(arr, new_len);
}


int __ScArray_set_len_expand (struct ScArray *arr, size_t new_len) {
  int ret = ScArray_reserve(arr, new_len);
  if (sc_unlikely(ret < 0)) {
    return ret;
  }

  if (ScTrait_has(&arr->elt_initializer, enter)) {
    sc_enter_t enter_func = (sc_enter_t) arr->elt_initializer.enter->func;
    void *enter_data = arr->elt_initializer.enter->data;
    size_t old_len = arr->len;

    for (size_t i = old_len; i < new_len; i++) {
      int ret_ = enter_func(ScArray_get(arr, i), enter_data);
      if (sc_unlikely(ret_ < 0)) {
        arr->len = i;
        __ScArray_set_len_shrink(arr, old_len);
        return -SC_EEXC;
      }
    }
  }
  return __ScArray_set_len(arr, new_len);
}
