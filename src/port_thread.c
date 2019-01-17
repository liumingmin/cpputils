#include <sys_mt.h>

#ifdef SYS_MT

#include <stdio.h>
#include <internal.h>

#if (defined(WIN32) || defined(_WIN32_WCE)) && !defined(HAVE_PTHREAD_WIN32)
#  if defined _WIN32_WCE
#    include <winbase.h>
#    define _beginthreadex	CreateThread
#    define	_endthreadex	ExitThread
#  elif defined WIN32
#    include <process.h>
#  endif
#endif

/* stack size is only needed on VxWorks. */

#ifndef __VXWORKS_OS__
#if defined(HAVE_PTHREAD) || defined(HAVE_PTH_PTHREAD_H) || defined(HAVE_PTHREAD_WIN32)

struct sys_thread *
sys_thread_create (int stacksize, void *(*func) (void *), void *arg)
{
  int i;
  sys_thread_t *thread =
    (sys_thread_t *) sys_malloc (sizeof (sys_thread_t));
  if (thread == NULL)
    return NULL;

  i = pthread_create (thread, NULL, func, (void *) arg);
  if (i != 0)
    {
      return NULL;
    }
  return (struct sys_thread *) thread;
}

int
sys_thread_set_priority (struct sys_thread *thread, int priority)
{
  return 0;
}

int
sys_thread_join (struct sys_thread *_thread)
{
  sys_thread_t *thread = (sys_thread_t *) _thread;
  if (thread == NULL)
    return -1;
  return pthread_join (*thread, NULL);
}

void
sys_thread_exit ()
{
  pthread_exit (NULL);
}

#endif
#endif


#if (defined(WIN32) || defined(_WIN32_WCE)) && !defined(HAVE_PTHREAD_WIN32)

struct sys_thread *
sys_thread_create (int stacksize, void *(*func) (void *), void *arg)
{
  sys_thread_t *thread =
    (sys_thread_t *) sys_malloc (sizeof (sys_thread_t));
  if (thread == NULL)
    return NULL;
  thread->h = (HANDLE) _beginthreadex (NULL,	/* default security attr */
				       0,	/* use default one */
				      (unsigned (__stdcall *)(void *)) func, arg, 0, &(thread->id));
  if (thread->h == 0)
    {
      sys_free (thread);
      return NULL;
    }
  return (struct sys_thread *) thread;
}

int
sys_thread_join (struct sys_thread *_thread)
{
  int i;
  sys_thread_t *thread = (sys_thread_t *) _thread;

  if (thread == NULL)
    return -1;
  i = WaitForSingleObject (thread->h, INFINITE);
  if (i == WAIT_OBJECT_0)
    {
      /* fprintf (stdout, "thread joined!\n"); */
    }
  else
    {
      /* fprintf (stdout, "ERROR!! thread joined ERROR!!\n"); */
      return -1;
    }
  CloseHandle (thread->h);
  return (0);
}

void
sys_thread_exit ()
{
  /* ExitThread(0); */
  _endthreadex (0);
}

int
sys_thread_set_priority (struct sys_thread *thread, int priority)
{
  return 0;
}


#endif

#ifndef __VXWORKS_OS__
#ifdef __PSOS__
struct sys_thread *
sys_thread_create (int stacksize, void *(*func) (void *), void *arg)
{
  sys_thread_t *thread =
    (sys_thread_t *) sys_malloc (sizeof (sys_thread_t));
  if (thread == NULL)
    return (NULL);
  if (t_create ("sip", 150, stacksize, 0, 0, &thread->tid) != 0)
    {
      sys_free (thread);
      return (NULL);
    }

  if (t_start (thread->tid, T_PREEMPT | T_ISR, func, 0) != 0)
    {
      sys_free (thread);
      return (NULL);
    }

  return (struct sys_thread *) thread;
}

int
sys_thread_set_priority (struct sys_thread *_thread, int priority)
{
  unsigned long oldprio;
  sys_thread_t *thread = (sys_thread_t *) _thread;

  if (thread == NULL)
    return -1;
  t_set_pri (thread->tid, priority, &oldprio);
  return 0;
}

int
sys_thread_join (struct sys_thread *_thread)
{
  sys_thread_t *thread = (sys_thread_t *) _thread;
  if (thread == NULL)
    return -1;
  t_delete (thread->tid);

  return (0);
}

void
sys_thread_exit ()
{
  t_delete (0);
}
#endif
#endif

#ifdef __VXWORKS_OS__
struct sys_thread *
sys_thread_create (int stacksize, void *(*func) (void *), void *arg)
{
  sys_thread_t *thread =
    (sys_thread_t *) sys_malloc (sizeof (sys_thread_t));
  if (thread == NULL)
    return NULL;
  thread->id = taskSpawn (NULL, 5, 0, stacksize, (FUNCPTR) func, (int) arg,
			  0, 0, 0, 0, 0, 0, 0, 0, 0);
  if (thread->id < 0)
    sys_free (thread);
  return (struct sys_thread *) thread;
}

int
sys_thread_set_priority (struct sys_thread *_thread, int priority)
{
  sys_thread_t *thread = (sys_thread_t *) _thread;
  if (thread == NULL)
    return -1;
  taskPrioritySet (thread->id, 1);
  return 0;
}

int
sys_thread_join (struct sys_thread *_thread)
{
  sys_thread_t *thread = (sys_thread_t *) _thread;
  if (thread == NULL)
    return -1;
  return taskDelete (thread->id);
}

void
sys_thread_exit ()
{
  /*?? */
}

#endif

#endif /* #ifdef SYS_MT */
