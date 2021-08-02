/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2010 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* This file effectively just provides sched.h and time.h, which in
   turn provide others.  */

#ifndef	_PTHREAD_H
#define	_PTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <sched.h>
#include <time.h>

  typedef char pthread_t;
  typedef char pthread_cond_t;
  typedef char pthread_mutex_t;
  typedef char pthread_key_t;

  /* Unsupported features  */

#if 0
#define PTHREAD_CANCEL_DEFERRED     0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1
#define PTHREAD_CANCEL_ENABLE       0
#define PTHREAD_CANCEL_DISABLE      1
#define PTHREAD_CANCELED            ((void *) -1)
#define PTHREAD_COND_INITIALIZER    {0}
#define PTHREAD_CREATE_DETACHED     0
#define PTHREAD_CREATE_JOINABLE     1
#define PTHREAD_EXPLICIT_SCHED      0
#define PTHREAD_INHERIT_SCHED       1
#define PTHREAD_MUTEX_DEFAULT       0
#define PTHREAD_MUTEX_ERRORCHECK    1
#define PTHREAD_MUTEX_NORMAL        2
#define PTHREAD_MUTEX_INITIALIZER   ((void *) -1)
#define PTHREAD_MUTEX_RECURSIVE     3
#define PTHREAD_ONCE_INIT           0
#define PTHREAD_PRIO_INHERIT        1
#define PTHREAD_PRIO_NONE           2
#define PTHREAD_PRIO_PROTECT        3
#define PTHREAD_PROCESS_SHARED      0
#define PTHREAD_PROCESS_PRIVATE     1
#define PTHREAD_RWLOCK_INITIALIZER  ((void *) -1)
#define PTHREAD_SCOPE_PROCESS       0
#define PTHREAD_SCOPE_SYSTEM        1

int   pthread_attr_destroy(pthread_attr_t *);
int   pthread_attr_getdetachstate(const pthread_attr_t *, int *);
int   pthread_attr_getguardsize(const pthread_attr_t *, size_t *);
int   pthread_attr_getinheritsched(const pthread_attr_t *, int *);
int   pthread_attr_getschedparam(const pthread_attr_t *,
				 struct sched_param *);
int   pthread_attr_getschedpolicy(const pthread_attr_t *, int *);
int   pthread_attr_getscope(const pthread_attr_t *, int *);
int   pthread_attr_getstackaddr(const pthread_attr_t *, void **);
int   pthread_attr_getstacksize(const pthread_attr_t *, size_t *);
int   pthread_attr_init(pthread_attr_t *);
int   pthread_attr_setdetachstate(pthread_attr_t *, int);
int   pthread_attr_setguardsize(pthread_attr_t *, size_t);
int   pthread_attr_setinheritsched(pthread_attr_t *, int);
int   pthread_attr_setschedparam(pthread_attr_t *,
				 const struct sched_param *);
int   pthread_attr_setschedpolicy(pthread_attr_t *, int);
int   pthread_attr_setscope(pthread_attr_t *, int);
int   pthread_attr_setstackaddr(pthread_attr_t *, void *);
int   pthread_attr_setstacksize(pthread_attr_t *, size_t);
int   pthread_cancel(pthread_t);
void  pthread_cleanup_push(void*), void *);
void  pthread_cleanup_pop(int);
int   pthread_cond_broadcast(pthread_cond_t *);
int   pthread_cond_destroy(pthread_cond_t *);
int   pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *);
int   pthread_cond_signal(pthread_cond_t *);
int   pthread_cond_timedwait(pthread_cond_t *, 
			     pthread_mutex_t *, const struct timespec *);
int   pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int   pthread_condattr_destroy(pthread_condattr_t *);
int   pthread_condattr_getpshared(const pthread_condattr_t *, int *);
int   pthread_condattr_init(pthread_condattr_t *);
int   pthread_condattr_setpshared(pthread_condattr_t *, int);
int   pthread_create(pthread_t *, const pthread_attr_t *,
		     void *(*)(void *), void *);
int   pthread_detach(pthread_t);
int   pthread_equal(pthread_t, pthread_t);
void  pthread_exit(void *);
int   pthread_getconcurrency(void);
int   pthread_getschedparam(pthread_t, int *, struct sched_param *);
void *pthread_getspecific(pthread_key_t);
int   pthread_join(pthread_t, void **);
int   pthread_key_create(pthread_key_t *, void (*)(void *));
int   pthread_key_delete(pthread_key_t);
int   pthread_mutex_destroy(pthread_mutex_t *);
int   pthread_mutex_getprioceiling(const pthread_mutex_t *, int *);
int   pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
int   pthread_mutex_lock(pthread_mutex_t *);
int   pthread_mutex_setprioceiling(pthread_mutex_t *, int, int *);
int   pthread_mutex_trylock(pthread_mutex_t *);
int   pthread_mutex_unlock(pthread_mutex_t *);
int   pthread_mutexattr_destroy(pthread_mutexattr_t *);
int   pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *,
				       int *);
int   pthread_mutexattr_getprotocol(const pthread_mutexattr_t *, int *);
int   pthread_mutexattr_getpshared(const pthread_mutexattr_t *, int *);
int   pthread_mutexattr_gettype(const pthread_mutexattr_t *, int *);
int   pthread_mutexattr_init(pthread_mutexattr_t *);
int   pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int);
int   pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int);
int   pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
int   pthread_mutexattr_settype(pthread_mutexattr_t *, int);
int   pthread_once(pthread_once_t *, void (*)(void));
int   pthread_rwlock_destroy(pthread_rwlock_t *);
int   pthread_rwlock_init(pthread_rwlock_t *,
			  const pthread_rwlockattr_t *);
int   pthread_rwlock_rdlock(pthread_rwlock_t *);
int   pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int   pthread_rwlock_trywrlock(pthread_rwlock_t *);
int   pthread_rwlock_unlock(pthread_rwlock_t *);
int   pthread_rwlock_wrlock(pthread_rwlock_t *);
int   pthread_rwlockattr_destroy(pthread_rwlockattr_t *);
int   pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *,
				    int *);
int   pthread_rwlockattr_init(pthread_rwlockattr_t *);
int   pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int);
pthread_t pthread_self(void);
int   pthread_setcancelstate(int, int *);
int   pthread_setcanceltype(int, int *);
int   pthread_setconcurrency(int);
int   pthread_setschedparam(pthread_t, int ,
          const struct sched_param *);
int   pthread_setspecific(pthread_key_t, const void *);
void  pthread_testcancel(void);
#endif

#ifdef __cplusplus
}
#endif
#endif
