#include <sys_mt.h>

#ifdef SYS_MT

#include <stdlib.h>
#include <stdio.h>
#include <internal.h>


#if !defined(__VXWORKS_OS__) && !defined(__PSOS__) && \
	!defined(WIN32) && !defined(_WIN32_WCE) && !defined(HAVE_PTHREAD_WIN32) && \
    !defined(HAVE_PTHREAD) && !defined(HAVE_PTH_PTHREAD_H)
#error No thread implementation found!
#endif

#if defined(HAVE_PTHREAD) || defined(HAVE_PTH_PTHREAD_H) || defined(HAVE_PTHREAD_WIN32)

struct sys_mutex *
sys_mutex_init ()
{
  sys_mutex_t *mut = (sys_mutex_t *) sys_malloc (sizeof (sys_mutex_t));

  if (mut == NULL)
    return NULL;
  pthread_mutex_init (mut, NULL);
  return (struct sys_mutex *) mut;
}

void
sys_mutex_destroy (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut == NULL)
    return;
  pthread_mutex_destroy (mut);
  sys_free (mut);
}

int
sys_mutex_lock (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut == NULL)
    return -1;
  return pthread_mutex_lock (mut);
}

int
sys_mutex_unlock (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut == NULL)
    return -1;
  return pthread_mutex_unlock (mut);
}

#endif

#if (defined(HAVE_SEMAPHORE_H) && !defined(__APPLE_CC__)) || defined(HAVE_PTHREAD_WIN32)

/* Counting Semaphore is initialized to value */
struct sys_sem *
sys_sem_init (unsigned int value)
{
  sys_sem_t *sem = (sys_sem_t *) sys_malloc (sizeof (sys_sem_t));
  if (sem == NULL)
    return NULL;

  if (sem_init (sem, 0, value) == 0)
    return (struct sys_sem *) sem;
  sys_free (sem);
  return NULL;
}

int
sys_sem_destroy (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return 0;
  sem_destroy (sem);
  sys_free (sem);
  return 0;
}

int
sys_sem_post (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return -1;
  return sem_post (sem);
}

int
sys_sem_wait (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return -1;
  return sem_wait (sem);
}

int
sys_sem_trywait (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return -1;
  return sem_trywait (sem);
}

#elif defined (HAVE_SYS_SEM_H)
/* support for semctl, semop, semget */

#define SEM_PERM 0600

struct sys_sem *
sys_sem_init (unsigned int value)
{
  union semun val;
  int i;
  sys_sem_t *sem = (sys_sem_t *) sys_malloc (sizeof (sys_sem_t));

  if (sem == NULL)
    return NULL;

  sem->semid = semget (IPC_PRIVATE, 1, IPC_CREAT | SEM_PERM);
  if (sem->semid == -1)
    {
      perror ("semget error");
      sys_free (sem);
      return NULL;
    }
  val.val = (int) value;
  i = semctl (sem->semid, 0, SETVAL, val);
  if (i != 0)
    {
      perror ("semctl error");
      sys_free (sem);
      return NULL;
    }
  return (struct sys_sem *) sem;
}

int
sys_sem_destroy (struct sys_sem *_sem)
{
  union semun val;
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return 0;
  val.val = 0;
  semctl (sem->semid, 0, IPC_RMID, val);
  sys_free (sem);
  return 0;
}

int
sys_sem_post (struct sys_sem *_sem)
{
  struct sembuf sb;
  sys_sem_t *sem = (sys_sem_t *) _sem;

  if (sem == NULL)
    return -1;
  sb.sem_num = 0;
  sb.sem_op = 1;
  sb.sem_flg = 0;
  return semop (sem->semid, &sb, 1);
}

int
sys_sem_wait (struct sys_sem *_sem)
{
  struct sembuf sb;
  sys_sem_t *sem = (sys_sem_t *) _sem;

  if (sem == NULL)
    return -1;
  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = 0;
  return semop (sem->semid, &sb, 1);
}

int
sys_sem_trywait (struct sys_sem *_sem)
{
  struct sembuf sb;
  sys_sem_t *sem = (sys_sem_t *) _sem;

  if (sem == NULL)
    return -1;
  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = IPC_NOWAIT;
  return semop (sem->semid, &sb, 1);
}

#endif


/* use VxWorks implementation */
#ifdef __VXWORKS_OS__

struct sys_mutex *
sys_mutex_init ()
{
  return (struct sys_mutex *) semMCreate (SEM_Q_FIFO|SEM_DELETE_SAFE);
}

void
sys_mutex_destroy (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut == NULL)
    return;
  semDelete (mut);
}

int
sys_mutex_lock (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut == NULL)
    return -1;
  return semTake (mut, WAIT_FOREVER);
}

int
sys_mutex_unlock (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut == NULL)
    return -1;
  return semGive (mut);
}

struct sys_sem *
sys_sem_init (unsigned int value)
{
  SEM_ID initsem;
  sys_sem_t *x;

  x = (sys_sem_t *) sys_malloc (sizeof (sys_sem_t));
  if (x == NULL)
    return NULL;
  initsem = semCCreate (SEM_Q_FIFO, value);
  x->semId = initsem;
  x->refCnt = value;
  x->sem_name = NULL;
  return (struct sys_sem *) x;
}

int
sys_sem_destroy (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return 0;
  semDelete (sem->semId);
  sys_free (sem);
  return 0;
}

int
sys_sem_post (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return -1;
  return semGive (sem->semId);
}

int
sys_sem_wait (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return -1;
  return semTake (sem->semId, WAIT_FOREVER);
}

int
sys_sem_trywait (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return -1;
  return semTake (sem->semId, NO_WAIT);
}

#endif


#if (defined(WIN32) || defined(_WIN32_WCE)) && !defined(HAVE_PTHREAD_WIN32)

#include <limits.h>

struct sys_mutex *
sys_mutex_init ()
{
  sys_mutex_t *mut = (sys_mutex_t *) sys_malloc (sizeof (sys_mutex_t));
  if (mut == NULL)
    return NULL;
  if ((mut->h = CreateMutex (NULL, FALSE, NULL)) != NULL)
    return (struct sys_mutex *) (mut);
  sys_free (mut);
  return (NULL);
}

void
sys_mutex_destroy (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut == NULL)
    return;
  CloseHandle (mut->h);
  sys_free (mut);
}

int
sys_mutex_lock (struct sys_mutex *_mut)
{
  DWORD err;
  sys_mutex_t *mut = (sys_mutex_t *) _mut;

  if (mut == NULL)
    return -1;
  if ((err = WaitForSingleObject (mut->h, INFINITE)) == WAIT_OBJECT_0)
    return (0);
  return (EBUSY);
}

int
sys_mutex_unlock (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut == NULL)
    return -1;
  ReleaseMutex (mut->h);
  return (0);
}

struct sys_sem *
sys_sem_init (unsigned int value)
{
  sys_sem_t *sem = (sys_sem_t *) sys_malloc (sizeof (sys_sem_t));
  if (sem == NULL)
    return NULL;

  if ((sem->h = CreateSemaphore (NULL, value, LONG_MAX, NULL)) != NULL)
    return (struct sys_sem *) (sem);
  sys_free (sem);
  return (NULL);
}

int
sys_sem_destroy (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return 0;
  CloseHandle (sem->h);
  sys_free (sem);
  return (0);
}

int
sys_sem_post (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return -1;
  ReleaseSemaphore (sem->h, 1, NULL);
  return (0);
}

int
sys_sem_wait (struct sys_sem *_sem)
{
  DWORD err;
  sys_sem_t *sem = (sys_sem_t *) _sem;

  if (sem == NULL)
    return -1;
  if ((err = WaitForSingleObject (sem->h, INFINITE)) == WAIT_OBJECT_0)
    return (0);
  if (err == WAIT_TIMEOUT)
    return (EBUSY);
  return (EBUSY);
}

int
sys_sem_trywait (struct sys_sem *_sem)
{
  DWORD err;
  sys_sem_t *sem = (sys_sem_t *) _sem;

  if (sem == NULL)
    return -1;
  if ((err = WaitForSingleObject (sem->h, 0)) == WAIT_OBJECT_0)
    return (0);
  return (EBUSY);
}
#endif

#ifdef __PSOS__
struct sys_mutex *
sys_mutex_init ()
{
  sys_mutex_t *mut = (sys_mutex_t *) sys_malloc (sizeof (sys_mutex_t));

  if (sm_create ("mut", 1, 0, &mut->id) == 0)
    return (struct sys_mutex *) (mut);
  sys_free (mut);
  return (NULL);
}

void
sys_mutex_destroy (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut)
    {
      sm_delete (mut->id);
      sys_free (mut);
    }
}

int
sys_mutex_lock (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut)
    {
      if (sm_p (mut->id, SM_WAIT, 0) != 0)
	return (-1);
    }
  return (0);
}

int
sys_mutex_unlock (struct sys_mutex *_mut)
{
  sys_mutex_t *mut = (sys_mutex_t *) _mut;
  if (mut)
    {
      sm_v (mut->id);
    }

  return (0);
}

struct sys_sem *
sys_sem_init (unsigned int value)
{
  sys_sem_t *sem = (sys_sem_t *) sys_malloc (sizeof (sys_sem_t));

  if (sm_create ("sem", value, 0, &sem->id) == 0)
    return (struct sys_sem *) (sem);
  sys_free (sem);
  return (NULL);
}

int
sys_sem_destroy (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return 0;
  sm_delete (sem->id);
  sys_free (sem);
  return (0);
}

int
sys_sem_post (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return -1;
  return (sm_v (sem->id));
}

int
sys_sem_wait (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return -1;
  if (sm_p (sem->id, SM_WAIT, 0) != 0)
    return (-1);
  return (0);
}

int
sys_sem_trywait (struct sys_sem *_sem)
{
  sys_sem_t *sem = (sys_sem_t *) _sem;
  if (sem == NULL)
    return -1;
  if (sm_p (sem->id, SM_NOWAIT, 0) != 0)
    return (-1);
  return (0);
}

#endif

#endif /* #ifdef SYS_MT */
