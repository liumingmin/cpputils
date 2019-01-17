
#include <internal.h>
#include <sys_time.h>

void
add_gettimeofday (struct timeval *atv, int ms)
{
  int m;
  if (ms>=1000000)
    {
      atv->tv_usec = 0;
      m = ms / 1000;
    }
  else
    {
      atv->tv_usec += ms * 1000;
      m = atv->tv_usec / 1000000;
      atv->tv_usec = atv->tv_usec % 1000000;
    }
  atv->tv_sec += m;
}

void
min_timercmp (struct timeval *tv1, struct timeval *tv2)
{
  if (tv2->tv_sec == -1)
    return;
  if (sys_timercmp (tv1, tv2, >))
    {
      /* replace tv1 with tv2 info */
      tv1->tv_sec = tv2->tv_sec;
      tv1->tv_usec = tv2->tv_usec;
    }
}

int sys_gettickcount(void)
{
	static int sSeconds = 0;
	struct timeval tv; /* timeval structure */
	int t; 

	sys_gettimeofday(&tv,NULL);

	if (!sSeconds)
		sSeconds = tv.tv_sec;

	t=( (tv.tv_sec - sSeconds)*1000 + (tv.tv_usec/1000) );
	return (t==0xffffffff)?0:t;
}

#if !defined(__PALMOS__) && (defined(WIN32) || defined(_WIN32_WCE))

#include <time.h>
#include <sys/timeb.h>

int
sys_gettimeofday (struct timeval *tp, void *tz)
{
  struct _timeb timebuffer;

  _ftime (&timebuffer);
  tp->tv_sec = timebuffer.time;
  tp->tv_usec = timebuffer.millitm * 1000;
  return 0;
}

#endif
