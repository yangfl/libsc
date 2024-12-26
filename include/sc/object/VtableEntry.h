#ifndef SC_OBJECT_VTABLEENTRY_H
#define SC_OBJECT_VTABLEENTRY_H 1

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif


struct ScVtableEntry {
  unsigned int nsid;
  ssize_t offset;
  const void *vtable;
};

inline const struct ScVtableEntry *ScVtableEntryArray_find_id (
    const struct ScVtableEntry *entrys, unsigned int nsid) {
  for (unsigned int i = 0;; i++) {
    unsigned int vtable_nsid = entrys[i].nsid;
    if (vtable_nsid == nsid) {
      return &entrys[i];
    }
    if (vtable_nsid == 0) {
      return NULL;
    }
  }
}

#define ScVtableEntryArray_find(entrys, type) \
  ScVtableEntryArray_find_id(entrys, type ## _NSID)


#ifdef __cplusplus
}
#endif

#endif /* SC_OBJECT_VTABLEENTRY_H */
