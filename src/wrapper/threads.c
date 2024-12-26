#include "sc/system/threads.h"

#include "sc/wrapper/threads.h"


extern inline int sc_mtx_init (mtx_t *mutex, int type);
extern inline int sc_mtx_lock (mtx_t *mutex);
extern inline int sc_mtx_trylock (mtx_t *mutex);
extern inline int sc_mtx_unlock (mtx_t *mutex);

extern inline int sc_cnd_init (cnd_t *cond);
extern inline int sc_cnd_signal (cnd_t *cond);
extern inline int sc_cnd_broadcast (cnd_t *cond);
extern inline int sc_cnd_wait (cnd_t *cond, mtx_t *mutex);
