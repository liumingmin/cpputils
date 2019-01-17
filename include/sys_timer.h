#ifndef _SYS_TIMER_H_
#define _SYS_TIMER_H_

#include "sys_port.h"

#ifndef SYS_MT
#error "SYS_MT wasn't defined"
#endif

#include <sys_mt.h>
#include <internal.h> // for struct timeval.
#include "sys_type.h"

#define TIME_GRANULARITY     10 //msec
#define TIMER_MAX_LOST_TICK    8 //granularity


#ifdef __cplusplus
extern "C"
{
#endif


typedef int (*TimerCallBackFunc)(void *data);

typedef struct sys_timer
{
	//must place here as head, don't change !!
	struct sys_timer *pPrev;
	struct sys_timer *pNext;
	//

	int expireTime, timeout;
	SYSBOOL reRegister, isIdle;
	void *pCallBackParamData;
	TimerCallBackFunc pTimeoutCallBackfunc;
	int tag;
}
sys_timer_t;


/*
 CAUTION:
	timer is not thread safe !!, but operation like "add","delete","resume",
	"stop" is thread safe.
*/

/*
 Allocate memory for timers, and start timer thread.
 return SYS_OK if success.
*/
int sys_init_timers(int MaxTimerNum);

/*
 Free memory allocated for timers and terminate timer thread.
*/
void sys_free_timers(void);

/*
 Return timer added.
 return NULL means fail.
*/
sys_timer_t *sys_timer_add(int ExpireTime, TimerCallBackFunc pCBfunc, void *pCBParam, SYSBOOL StartNow, SYSBOOL reReg);
/*
 restart timer.
*/
void sys_timer_resume(sys_timer_t *pTimer);
/*
 restart timer.
*/
void sys_timer_restart(sys_timer_t *pTimer, int ExpireTime, TimerCallBackFunc pCBfunc, void *pCBParam, SYSBOOL reReg);
/*
 stop timer.
*/
void sys_timer_stop(sys_timer_t *pTimer);
/*
 delete timer.
*/
void sys_timer_delete(sys_timer_t *pTimer);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _SYS_TIMER_H_ */
