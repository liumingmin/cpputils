
#ifndef __SYS_CONDV_H__
#define __SYS_CONDV_H__

#include <stdio.h>

#ifdef SYS_MT

#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

/**
 * @file sys_condv.h
 * @brief oSIP condition variables definitions
 *
 * Those methods are only available if the library is compile
 * in multi threaded mode. This is the default for oSIP.
 */

/**
 * @defgroup oSIP_COND oSIP condition variables definitions
 * @ingroup osip2_port
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__PSOS__)

/* TODO */

#else


/* condv implementation */
#if defined(WIN32) || defined(_WIN32_WCE)
/* Prevent struct redefinition if Pthreads for Win32 is used */
#ifndef HAVE_STRUCT_TIMESPEC
#define HAVE_STRUCT_TIMESPEC 1
/**
 * timespec structure
 * @struct timespec
 */
struct timespec
{
  long tv_sec;
  long tv_nsec;
};
#endif
#endif

  struct sys_cond;

/**
 * Allocate and Initialise a condition variable
 */
  struct sys_cond *sys_cond_init (void);
/**
 * Destroy a condition variable
 * @param cond The condition variable to destroy.
 */
  int sys_cond_destroy (struct sys_cond *cond);
/**
 * Signal the condition variable.
 * @param cond The condition variable to signal.
 */
  int sys_cond_signal (struct sys_cond *cond);

/**
 * Wait on the condition variable.
 * @param cond The condition variable to wait on.
 * @param mut The external mutex 
 */
  int sys_cond_wait (struct sys_cond *cond, struct sys_mutex *mut);
/**
 * Timed wait on the condition variable.
 * @param cond The condition variable to wait on.
 * @param mut The external mutex 
 * @param abstime time to wait until
 */
  int sys_cond_timedwait (struct sys_cond *cond, struct sys_mutex *mut,
			   const struct timespec *abstime);


#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif
