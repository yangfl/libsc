#ifndef SC_THREAD_THREADPOOL_H
#define SC_THREAD_THREADPOOL_H 1

#include "sc/cdefs.h"
#include "sc/system/threads.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ScException;


struct ScThreadPool {
  union {
    char __size[192];
    long long __align;
  };
};

SC_API __nonnull((1)) __attr_access((__write_only__, 2))
int ScThreadPool_stop (struct ScThreadPool *pool);
SC_API __nonnull((1, 2))
int ScThreadPool_run (void *pool, thrd_start_t func, void *arg);

SC_API __nonnull()
void ScThreadPool_destroy (struct ScThreadPool *pool);
SC_API __nonnull((1)) __attr_access((__read_only__, 3))
int ScThreadPool_init (
  struct ScThreadPool *__restrict pool, unsigned int nproc,
  const char *__restrict name);


#ifdef __cplusplus
}
#endif

#endif /* SC_THREAD_THREADPOOL_H */
