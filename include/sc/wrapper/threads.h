#ifndef SC_WRAPPER_THREADS_H
#define SC_WRAPPER_THREADS_H 1

#include "sc/system/threads.h"

#include "sc/branch.h"
#include "sc/cdefs.h"
#include "sc/Exception.h"

#ifdef __cplusplus
extern "C" {
#endif


SC_API
inline int sc_mtx_init (mtx_t *mutex, int type) {
  int ret = mtx_init(mutex, type);
  if (sc_unlikely(ret != thrd_success)) {
    sc_exc_set_errno("mtx_init");
  }
  return ret;
}

SC_API
inline int sc_mtx_lock (mtx_t *mutex) {
  int ret = mtx_lock(mutex);
  if (sc_unlikely(ret != thrd_success)) {
    sc_exc_set_errno("mtx_lock");
  }
  return ret;
}

SC_API
inline int sc_mtx_trylock (mtx_t *mutex) {
  int ret = mtx_trylock(mutex);
  if (sc_unlikely(ret != thrd_success)) {
    sc_exc_set_errno("mtx_lock");
  }
  return ret;
}

SC_API
inline int sc_mtx_unlock (mtx_t *mutex) {
  int ret = mtx_unlock(mutex);
  if (sc_unlikely(ret != thrd_success)) {
    sc_exc_set_errno("mtx_lock");
  }
  return ret;
}

SC_API
inline int sc_cnd_init (cnd_t *cond) {
  int ret = cnd_init(cond);
  if (sc_unlikely(ret != thrd_success)) {
    sc_exc_set_errno("cnd_init");
  }
  return ret;
}

SC_API
inline int sc_cnd_signal (cnd_t *cond) {
  int ret = cnd_signal(cond);
  if (sc_unlikely(ret != thrd_success)) {
    sc_exc_set_errno("cnd_signal");
  }
  return ret;
}

SC_API
inline int sc_cnd_broadcast (cnd_t *cond) {
  int ret = cnd_broadcast(cond);
  if (sc_unlikely(ret != thrd_success)) {
    sc_exc_set_errno("cnd_broadcast");
  }
  return ret;
}

SC_API
inline int sc_cnd_wait (cnd_t *cond, mtx_t *mutex) {
  int ret = cnd_wait(cond, mutex);
  if (sc_unlikely(ret != thrd_success)) {
    sc_exc_set_errno("cnd_wait");
  }
  return ret;
}


#ifdef __cplusplus
}
#endif

#endif /* SC_WRAPPER_THREADS_H */
