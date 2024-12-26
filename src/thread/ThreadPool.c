#ifdef SC_NO_THREADS

#include "log.h"
#include "sc/thread/ThreadPool.h"


struct _ScThreadPool {
  int ret;
};
static_assert(sizeof(struct _ScThreadPool) <= sizeof(struct ScThreadPool));
static_assert(alignof(struct _ScThreadPool) <= alignof(struct ScThreadPool));


int ScThreadPool_run (void *ctx, thrd_start_t func, void *arg) {
  struct _ScThreadPool *pool = ctx;

  pool->ret = func(arg);
  return pool->ret;
}


int ScThreadPool_stop (
    struct ScThreadPool *_pool, const struct ScException **excp) {
}


void ScThreadPool_destroy (struct ScThreadPool *_pool) {
  (void) _pool;
}


int ScThreadPool_init (
    struct ScThreadPool *_pool, unsigned int nproc, const char *name) {
  const struct _ScThreadPool *pool = (const struct _ScThreadPool *) _pool;

  pool->ret = 0;
  return 0;
}

#else

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "sc/system/threads.h"

#include "sc/branch.h"
#include "sc/errno.h"
#include "sc/Exception.h"
#include "sc/thread/nproc.h"
#include "sc/thread/threadname.h"
#include "sc/thread/ThreadPool.h"

struct _ScThreadPool;


struct ScThreadPoolWorker {
  /// Parent thread pool.
  struct _ScThreadPool *pool;
  /// Thread object.
  thrd_t thr;
  /// Worker ID.
  unsigned int id;
  /// Return value of the job.
  int ret;
  /// Pointer to thread exception object.
  struct ScException *exc;
};


struct _ScThreadPool {
  /// Worker threads.
  struct ScThreadPoolWorker *workers;
  /// Number of workers.
  unsigned int nproc;
  /// Index of the worker that encountered an error, -1 if none.
  volatile int err_i;

  /// Friendly thread name for workers.
  char name[SC_THREADNAME_SIZE];

  /// -1: stopped, 0: idle, 1: busy
  volatile signed char state;
  /// Lock for @ref _ScThreadPool::producer_cond and @ref _ScThreadPool::consumer_cond .
  mtx_t mutex;
  /// Condition variable when a new job is available.
  cnd_t producer_cond;
  /// Condition variable when a worker is available.
  cnd_t consumer_cond;

  thrd_start_t func;
  void *arg;
};
static_assert(sizeof(struct _ScThreadPool) <= sizeof(struct ScThreadPool));
static_assert(alignof(struct _ScThreadPool) <= alignof(struct ScThreadPool));


static int ScThreadPool_worker (void *arg) {
  struct ScThreadPoolWorker *worker = arg;
  struct _ScThreadPool *pool = worker->pool;

  if (pool->name[0] != '\0') {
    sc_threadname_append(" (%s)", pool->name);
  }
  worker->exc = sc_exc_get();
  //sc_debug("ScThreadPool worker %u begin\n", worker->id);

  int ret = 0;
  while (true) {
    if (sc_unlikely(pool->state < 0)) {
      break;
    }

    if (sc_unlikely(mtx_lock(&pool->mutex) != thrd_success)) {
      sc_exc_set_errno("mtx_lock");
      ret = -SC_EEXC;
      break;
    }

    thrd_start_t func = NULL;
    void *funcarg;
    do {
      if (sc_likely(pool->state == 0)) {
        if (sc_unlikely(cnd_wait(
            &pool->consumer_cond, &pool->mutex) != thrd_success)) {
          sc_exc_set_errno("cnd_wait");
          ret = -SC_EEXC;
          break;
        }
      }

      if (sc_unlikely(pool->state < 0)) {
        break;
      }
      if (sc_unlikely(pool->state == 0)) {
        continue;
      }

      func = pool->func;
      funcarg = pool->arg;
      pool->state = 0;
    } while (false);

    mtx_unlock(&pool->mutex);
    if (sc_unlikely(ret < 0)) {
      break;
    }
    cnd_signal(&pool->producer_cond);

    if (sc_likely(func != NULL)) {
      int res = func(funcarg);
      if (sc_unlikely(res < 0)) {
        worker->ret = res;
        pool->err_i = worker->id;
      }
    }
  }

  //sc_debug("ScThreadPool worker %u stopped\n", worker->id);
  return ret;
}


int ScThreadPool_run (void *_pool, thrd_start_t func, void *arg) {
  struct _ScThreadPool *pool = _pool;

  if (sc_unlikely(pool->state < 0)) {
    return sc_exc_set_sc(SC_ESTOP);
  }
  if (sc_unlikely(mtx_lock(&pool->mutex) != thrd_success)) {
    sc_exc_set_errno("mtx_lock");
    return -SC_EEXC;
  }

  int ret = 0;
  do {
    if (sc_unlikely(pool->state > 0)) {
      if (sc_unlikely(cnd_wait(
          &pool->producer_cond, &pool->mutex) != thrd_success)) {
        sc_exc_set_errno("cnd_wait");
        ret = -SC_EEXC;
        break;
      }
    }

    if (sc_unlikely(pool->state < 0)) {
      return sc_exc_set_sc(SC_ESTOP);
    }
    if (sc_unlikely(pool->state > 0)) {
      continue;
    }

    pool->func = func;
    pool->arg = arg;
    pool->state = 1;

    ret = 0;
  } while (false);

  mtx_unlock(&pool->mutex);

  if (ret == 0) {
    cnd_signal(&pool->consumer_cond);
  }
  return ret;
}


int ScThreadPool_stop (
    struct ScThreadPool *_pool, const struct ScException **excp) {
  struct _ScThreadPool *pool = (struct _ScThreadPool *) _pool;

  if (sc_unlikely(pool->state < 0)) {
    return;
  }

  if (sc_unlikely(mtx_lock(&pool->mutex) != thrd_success)) {
    sc_exc_set_errno("mtx_lock");
    //print error
    return;
  }

  do {
    if (sc_unlikely(pool->state > 0)) {
      if (sc_unlikely(cnd_wait(
          &pool->producer_cond, &pool->mutex) != thrd_success)) {
        sc_exc_set_errno("cnd_wait");
        //print error
        break;
      }
    }

    if (sc_unlikely(pool->state > 0)) {
      continue;
    }

    pool->state = -1;
  } while (false);

  mtx_unlock(&pool->mutex);
  cnd_broadcast(&pool->consumer_cond);

  for (unsigned int i = 0; i < pool->nproc; i++) {
    thrd_join(pool->workers[i].thr, NULL);
  }

  return pool->err_i < 0 ? 0 : pool->workers[pool->err_i].ret;
}


void ScThreadPool_destroy (struct ScThreadPool *_pool) {
  struct _ScThreadPool *pool = (struct _ScThreadPool *) _pool;

  const struct ScException *exc;
  int ret = ScThreadPool_stop(_pool, &exc);
  if_fail (ret == 0) {
    ScException_stderr(exc, NULL);
  }

  free(pool->workers);
  cnd_destroy(&pool->consumer_cond);
  cnd_destroy(&pool->producer_cond);
  mtx_destroy(&pool->mutex);
}


int ScThreadPool_init (
    struct ScThreadPool *_pool, unsigned int nproc, const char *name) {
  struct _ScThreadPool *pool = (struct _ScThreadPool *) _pool;

  if (nproc == 0) {
    nproc = sc_nproc();
    return_if_fail (nproc > 0) ERR(PL_EINVAL);
  }

  int ret;

  return_if_fail (mtx_init(&pool->mutex, mtx_plain) == thrd_success)
    ERR_STD(mtx_init);
  if_fail (cnd_init(&pool->producer_cond) == thrd_success) {
    ret = ERR_STD(cnd_init);
    goto fail_producer_cond;
  }
  if_fail (cnd_init(&pool->consumer_cond) == thrd_success) {
    ret = ERR_STD(cnd_init);
    goto fail_consumer_cond;
  }
  pool->workers = calloc(nproc, sizeof(pool->workers[0]));
  if_fail (pool->workers != NULL) {
    ret = ERR_STD(calloc);
    goto fail_workers;
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-malloc-leak"
  if (name == NULL) {
    pool->name[0] = '\0';
  } else {
    strncpy(pool->name, name, sizeof(pool->name) - 1);
    pool->name[sizeof(pool->name) - 1] = '\0';
  }
  pool->state = 0;
#pragma GCC diagnostic pop

  unsigned int i;
  for (i = 0; i < nproc; i++) {
    pool->workers[i].pool = pool;
    pool->workers[i].id = i;
    break_if_fail (thrd_create(
      &pool->workers[i].thr, ScThreadPool_worker, pool->workers + i
    ) == thrd_success);
  }
  if_fail (i > 0) {
    ret = ERR_STD(thrd_create);
    goto fail;
  }

  if_fail (i >= nproc) {
    pool->workers = realloc(pool->workers, sizeof(pool->workers[0]) * i);
  }
  pool->nproc = i;
  pool->err_i = -1;
  return 0;

fail:
  free(pool->workers);
fail_workers:
  cnd_destroy(&pool->consumer_cond);
fail_consumer_cond:
  cnd_destroy(&pool->producer_cond);
fail_producer_cond:
  mtx_destroy(&pool->mutex);
  return ret;
}

#endif
