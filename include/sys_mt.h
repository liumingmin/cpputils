
#ifndef _SYS_MT_H_
#define _SYS_MT_H_

#include "sys_port.h"

#ifdef SYS_MT

#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

#include <stdio.h>
#include <errno.h>

/**
 * @file sys_mt.h
 * @brief oSIP Thread, Mutex and Semaphore definitions
 *
 * Those methods are only available if the library is compile
 * in multi threaded mode. This is the default for oSIP.
 */

/**
 * @defgroup oSIP_THREAD oSIP Thread Routines
 * @ingroup osip2_port
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Structure for referencing a thread
 * @struct sys_thread
 */
  struct sys_thread;

/**
 * Allocate (or initialise if a thread address is given)
 * @param stacksize The stack size of the thread. (20000 is a good value)
 * @param func The method where the thread start.
 * @param arg A pointer on the argument given to the method 'func'.
 */
  struct sys_thread *sys_thread_create (int stacksize,
					  void *(*func) (void *), void *arg);

/**
 * Join a thread.
 * @param thread The thread to join.
 */
  int sys_thread_join (struct sys_thread *thread);

/**
 * Set the priority of a thread. (NOT IMPLEMENTED ON ALL SYSTEMS)
 * @param thread The thread to work on.
 * @param priority The priority value to set.
 */
  int sys_thread_set_priority (struct sys_thread *thread, int priority);
/**
 * Exit from a thread.
 */
  void sys_thread_exit (void);

#ifdef __cplusplus
}
#endif

/** @} */

/**
 * @defgroup oSIP_SEMA oSIP semaphore definitions
 * @ingroup osip2_port
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Structure for referencing a semaphore element.
 * @struct sys_sem
 */
  struct sys_sem;

/**
 * Allocate and Initialise a semaphore.
 * @param value The initial value for the semaphore.
 */
  struct sys_sem *sys_sem_init (unsigned int value);
/**
 * Destroy a semaphore.
 * @param sem The semaphore to destroy.
 */
  int sys_sem_destroy (struct sys_sem *sem);
/**
 * Post operation on a semaphore.
 * @param sem The semaphore to destroy.
 */
  int sys_sem_post (struct sys_sem *sem);
/**
 * Wait operation on a semaphore.
 * NOTE: this call will block if the semaphore is at 0.
 * @param sem The semaphore to destroy.
 */
  int sys_sem_wait (struct sys_sem *sem);
/**
 * Wait operation on a semaphore.
 * NOTE: if the semaphore is at 0, this call won't block.
 * @param sem The semaphore to destroy.
 */
  int sys_sem_trywait (struct sys_sem *sem);


#ifdef __cplusplus
}
#endif

/** @} */

/**
 * @defgroup oSIP_MUTEX oSIP mutex definitions
 * @ingroup osip2_port
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Structure for referencing a mutex element.
 * @struct sys_mutex
 */
  struct sys_mutex;

/**
 * Allocate and Initialise a mutex.
 */
  struct sys_mutex *sys_mutex_init (void);
/**
 * Destroy the mutex.
 * @param mut The mutex to destroy.
 */
  void sys_mutex_destroy (struct sys_mutex *mut);
/**
 * Lock the mutex.
 * @param mut The mutex to lock.
 */
  int sys_mutex_lock (struct sys_mutex *mut);
/**
 * Unlock the mutex.
 * @param mut The mutex to unlock.
 */
  int sys_mutex_unlock (struct sys_mutex *mut);

#ifdef __cplusplus
}
#endif

/** @} */

#endif				/* SYS_MT */

#endif				/* end of _THREAD_H_ */
