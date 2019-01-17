#include <sys_mt.h>

#ifdef SYS_MT

#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

#include <internal.h>
#include <sys_condv.h>


#if !defined(__VXWORKS_OS__) && !defined(__PSOS__) && \
	!defined(WIN32) && !defined(_WIN32_WCE) && !defined(HAVE_PTHREAD_WIN32) && \
    !defined(HAVE_PTHREAD) && !defined(HAVE_PTH_PTHREAD_H)
#error No thread implementation found!
#endif

#if defined(HAVE_PTHREAD) || defined(HAVE_PTH_PTHREAD_H) || defined(HAVE_PTHREAD_WIN32)
/*
  #include <sys/types.h>
  #include <sys/timeb.h>
*/
#include <pthread.h>

struct sys_cond *
sys_cond_init ()
{
  sys_cond_t *cond = (sys_cond_t *) sys_malloc (sizeof (sys_cond_t));
  if (cond && (pthread_cond_init (&cond->cv, NULL) == 0))
    {
      return (struct sys_cond *) (cond);
    }
  sys_free (cond);

  return NULL;
}

int
sys_cond_destroy (struct sys_cond *_cond)
{
  int ret;

  if (!_cond) return -1;
  ret = pthread_cond_destroy (&_cond->cv);
  sys_free (_cond);
  return ret;
}

int
sys_cond_signal (struct sys_cond *_cond)
{
  if (!_cond) return -1;
  return pthread_cond_signal (&_cond->cv);
}


int
sys_cond_wait (struct sys_cond *_cond, struct sys_mutex *_mut)
{
  if (!_cond) return -1;
  return pthread_cond_wait (&_cond->cv, (pthread_mutex_t *) _mut);
}


int
sys_cond_timedwait (struct sys_cond *_cond, struct sys_mutex *_mut,
		     const struct timespec *abstime)
{
  if (!_cond) return -1;
  return pthread_cond_timedwait (&_cond->cv, (pthread_mutex_t *) _mut,
				 (const struct timespec *) abstime);
}

#endif


#if (defined(WIN32) || defined(_WIN32_WCE)) && !defined(HAVE_PTHREAD_WIN32)

#include <sys/types.h>
#include <sys/timeb.h>

struct sys_cond *
sys_cond_init ()
{
  sys_cond_t *cond = (sys_cond_t *) sys_malloc (sizeof (sys_cond_t));
  if (cond && (cond->mut = sys_mutex_init ()) != NULL)
    {
      cond->sem = sys_sem_init (0);	/* initially locked */
      return (struct sys_cond *) (cond);
    }
  sys_free (cond);

  return NULL;
}

int
sys_cond_destroy (struct sys_cond *_cond)
{
  if (!_cond) return 0;
  if (_cond->sem == NULL)
    return 0;

  sys_sem_destroy (_cond->sem);

  if (_cond->mut == NULL)
    return 0;

  sys_mutex_destroy (_cond->mut);
  sys_free (_cond);
  return (0);
}

int
sys_cond_signal (struct sys_cond *_cond)
{
  if (!_cond) return -1;
  return sys_sem_post (_cond->sem);
}


int
sys_cond_wait (struct sys_cond *_cond, struct sys_mutex *_mut)
{
  int ret1 = 0, ret2 = 0, ret3 = 0;

  if (!_cond) return -1;

  if (sys_mutex_lock (_cond->mut))
    return -1;

  if (sys_mutex_unlock (_mut))
    return -1;

  ret1 = sys_sem_wait (_cond->sem);

  ret2 = sys_mutex_lock (_mut);

  ret3 = sys_mutex_unlock (_cond->mut);

  if (ret1 || ret2 || ret3)
    return -1;
  return 0;
}

#define SYS_CLOCK_REALTIME 4002

int
__sys_clock_gettime (unsigned int clock_id, struct timespec *tp)
{
  struct _timeb time_val;

  if (clock_id != SYS_CLOCK_REALTIME)
    return -1;

  if (tp == NULL)
    return -1;

  _ftime (&time_val);
  tp->tv_sec = time_val.time;
  tp->tv_nsec = time_val.millitm * 1000000;
  return 0;
}

static int
_delta_time (const struct timespec *start, const struct timespec *end)
{
  int difx;

  if (start == NULL || end == NULL) return 0;

  difx = ((end->tv_sec - start->tv_sec) * 1000) +
    ((end->tv_nsec - start->tv_nsec) / 1000000);

  return difx;
}


int
sys_cond_timedwait (struct sys_cond *_cond, struct sys_mutex *_mut,
		     const struct timespec *abstime)
{
  DWORD dwRet;
  struct timespec now;
  int timeout_ms;
  HANDLE sem;

  if (!_cond) return -1;

  sem = *((HANDLE *) _cond->sem);

  if (sem == NULL)
    return -1;

  if (abstime == NULL)
    return -1;

  __sys_clock_gettime (SYS_CLOCK_REALTIME, &now);

  timeout_ms = _delta_time (&now, abstime);
  if (timeout_ms <= 0)
    return 1;			/* ETIMEDOUT; */

  if (sys_mutex_unlock (_mut))
    return -1;

  dwRet = WaitForSingleObject (sem, timeout_ms);

  if (sys_mutex_lock (_mut))
    return -1;

  switch (dwRet)
    {
    case WAIT_OBJECT_0:
      return 0;
      break;
    case WAIT_TIMEOUT:
      return 1;			/* ETIMEDOUT; */
      break;
    default:
      return -1;
      break;
    }
}
#endif

#ifdef __PSOS__
 /*TODO*/
#endif
/* use VxWorks implementation */
#ifdef __VXWORKS_OS__

struct sys_cond *
sys_cond_init ()
{
  sys_cond_t *cond = (sys_cond_t *) sys_malloc (sizeof (sys_cond_t));
  if ( (cond->sem = sys_sem_init (0))!=NULL)
  {
   return (struct sys_cond *) (cond);
  }
  sys_free (cond);

  return NULL;
}

int
sys_cond_destroy (struct sys_cond *_cond)
{
  if (_cond->sem == NULL)
    return 0;

  sys_sem_destroy (_cond->sem);
  sys_free (_cond);
  return (0);
}

int
sys_cond_signal (struct sys_cond *_cond)
{
  return sys_sem_post (_cond->sem);
}

+static int _cond_wait(struct sys_cond *_cond, struct sys_mutex *_mut, int
ticks)
{
 int ret;
   if (sys_mutex_unlock (_mut)!=0)
   {
    return -1;
   }

  ret =  semTake( ((sys_sem_t *) _cond->sem)->semId, ticks);
   if (ret != OK)
   {
  switch (errno)
    {
       case S_objLib_OBJ_ID_ERROR:
       /* fall through */
    case S_objLib_OBJ_UNAVAILABLE:
     /* fall through */
#if 0
       case S_intLib_NOT_ISR_CALLABLE:
#endif
        ret = -1;
        break;
       case S_objLib_OBJ_TIMEOUT:
        ret = 1;
        break;
       default:  /* vxworks has bugs */
         ret = 1;
         break;
    }
   }

    if (sys_mutex_lock (_mut))
    {
     ret = -1;
    }
   return ret;
}

int
sys_cond_wait (struct sys_cond *_cond, struct sys_mutex *_mut)
{
 return _cond_wait(_cond, _mut, WAIT_FOREVER);
}

int
sys_cond_timedwait (struct sys_cond *_cond, struct sys_mutex *_mut,
       const struct timespec *abstime)
{
  int rate = sysClkRateGet();
  struct timespec now;
  long sec, nsec;
  int ticks;
  SEM_ID sem;
  if (_cond==NULL)
   return -1;

  sem = ((sys_sem_t *) _cond->sem)->semId;

  if (sem == NULL)
    return -1;

  if (abstime == NULL)
    return -1;
  clock_gettime(CLOCK_REALTIME, &now);

  sec  = abstime->tv_sec - now.tv_sec;
  nsec = abstime->tv_nsec - now.tv_nsec;

  while ( (sec > 0) && (nsec < 0))
  {
    --sec;
    nsec += 1000000000;
  }
  if (nsec < 0)
    return 1; /*ETIMEDOUT; */
  ticks = (sec * rate) + ( nsec / 1000 * rate / 1000000);

  return _cond_wait(_cond, _mut, ticks);
}

#endif

#endif /* #ifdef SYS_MT */
