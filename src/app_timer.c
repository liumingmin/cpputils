#include "sys_timer.h"
#include "sys_time.h"
#include "sys_type.h"
#include "sys_objPool.h"
#include "sys_list.h"

#define _TIMER(node) ((sys_timer_t*)node)
#define LOCK_ON 1
#define LOCK_OFF 0

//Global variable.
static ObjPool_t *pTimersPool = NULL;
static sys_chain_t IdleTimerChain;
static sys_chain_t WorkingTimerChain;
static struct sys_thread * pTimerTask;
static SYSBOOL TimerTerminated;
//

//prototype of funcions
void *sys_timer_task(void *pParam);
void sys_timer_remove_idle(sys_timer_t *pIdleTimer, int Lock);
void sys_timer_remove_working(sys_timer_t *pTimer, int Lock);
//

//-----------------------------------------------------------------

/*
 Allocate memory for timers, and start timer thread.
*/
int sys_init_timers(int MaxTimerNum)
{
	int ret;

	if (pTimersPool != NULL)
		return -1;
	
	//create timers pool
	pTimersPool = sys_create_ObjPool(MaxTimerNum, sizeof(sys_timer_t), "Timer pool");
	if (pTimersPool == NULL)
		return -1;
	
	//init timer chain.
	ret = sys_chain_init(&IdleTimerChain);
	if (ret != SYS_OK)
		return -1;
	ret = sys_chain_init(&WorkingTimerChain);
	if (ret != SYS_OK)
		return -1;
	
	//start timer thread
	TimerTerminated = FALSE;
	pTimerTask = sys_thread_create(0,&sys_timer_task,NULL);
	if (pTimerTask == NULL)
		return -1;

	return SYS_OK;
}

void sys_free_timers(void)
{
	sys_chain_node_t *pNode;

	if (pTimersPool == NULL) 
		return;
	
	if (pTimerTask != NULL)
	{
		TimerTerminated = TRUE;
		sys_thread_join(pTimerTask);
	}
	
	sys_mutex_lock(WorkingTimerChain.pLock);
	while (WorkingTimerChain.count > 0)
	{
		pNode = WorkingTimerChain.pHead;
		sys_timer_remove_working(_TIMER(pNode), LOCK_OFF);
		sys_free_obj(pTimersPool, pNode);
	}
	sys_mutex_unlock(WorkingTimerChain.pLock);
	sys_chain_free(&WorkingTimerChain);

	sys_mutex_lock(IdleTimerChain.pLock);
	while (IdleTimerChain.count > 0)
	{
		pNode = IdleTimerChain.pHead;
		sys_timer_remove_idle(_TIMER(pNode), LOCK_OFF);
		sys_free_obj(pTimersPool, pNode);
	}
	sys_mutex_unlock(IdleTimerChain.pLock);
	sys_chain_free(&IdleTimerChain);

	sys_destroy_ObjPool(pTimersPool);
	pTimersPool = NULL;
}

void sys_timer_set_idle(sys_timer_t *pIdleTimer, int Lock)
{
	if (Lock)
		sys_chain_add_tail(&IdleTimerChain, (sys_chain_node_t*)pIdleTimer);
	else
		sys_chain_add_tail_noLock(&IdleTimerChain, (sys_chain_node_t*)pIdleTimer);

	pIdleTimer->isIdle = TRUE;
}

void sys_timer_remove_idle(sys_timer_t *pIdleTimer, int Lock)
{
	if (Lock)
		sys_chain_remove(&IdleTimerChain, (sys_chain_node_t*)pIdleTimer);
	else
		sys_chain_remove_noLock(&IdleTimerChain, (sys_chain_node_t*)pIdleTimer);
}

/////
void sys_timer_set_working_noLock(sys_timer_t *pAddTimer)
{
	sys_chain_node_t *pCurrentNode, *pNextNode;
	int timeout;

	if (WorkingTimerChain.count == 0) // Empty chain ?
	{
		pAddTimer->timeout = pAddTimer->expireTime;
		sys_chain_add_head_noLock(&WorkingTimerChain, (sys_chain_node_t*)pAddTimer);
	}
	else
	{
		pCurrentNode = WorkingTimerChain.pHead;	//Compare first timer whith timer being added.
		if (pAddTimer->expireTime < _TIMER(pCurrentNode)->timeout)
		{
			pAddTimer->timeout = pAddTimer->expireTime;
			_TIMER(pCurrentNode)->timeout = _TIMER(pCurrentNode)->timeout - pAddTimer->timeout;
			sys_chain_add_head_noLock(&WorkingTimerChain, (sys_chain_node_t*)pAddTimer);
		}
		else
		{
			timeout = pAddTimer->expireTime;
			while (pCurrentNode != NULL)
			{
				timeout = timeout - _TIMER(pCurrentNode)->timeout;
				pNextNode = pCurrentNode->pNext;

				if (pNextNode == NULL)
				{
					break;
				}
				if (timeout < _TIMER(pNextNode)->timeout)
				{
					_TIMER(pNextNode)->timeout = _TIMER(pNextNode)->timeout - timeout;
					break;
				}

				pCurrentNode = pNextNode;
			}
			pAddTimer->timeout = timeout;
			sys_chain_add_after_noLock(&WorkingTimerChain, (sys_chain_node_t*)pAddTimer, pCurrentNode);
		}
	}

	pAddTimer->isIdle = FALSE;
}

void sys_timer_set_working(sys_timer_t *pAddTimer, int Lock)
{
	if (Lock){
		sys_mutex_lock(WorkingTimerChain.pLock);
		sys_timer_set_working_noLock(pAddTimer);
		sys_mutex_unlock(WorkingTimerChain.pLock);
	}
	else{
		sys_timer_set_working_noLock(pAddTimer);
	}
}

void sys_timer_remove_working_noLock(sys_timer_t *pTimer)
{
	sys_chain_node_t *pCurrentNode, *pNextNode;

	pCurrentNode = (sys_chain_node_t*)pTimer;
	pNextNode = pCurrentNode->pNext;
	if (pNextNode != NULL)
	{
		_TIMER(pNextNode)->timeout = _TIMER(pNextNode)->timeout + _TIMER(pCurrentNode)->timeout;
	}
	sys_chain_remove_noLock(&WorkingTimerChain, pCurrentNode);
}

void sys_timer_remove_working(sys_timer_t *pTimer, int Lock)
{
	if (Lock){
		sys_mutex_lock(WorkingTimerChain.pLock);
		sys_timer_remove_working_noLock(pTimer);
		sys_mutex_unlock(WorkingTimerChain.pLock);
	}
	else{
		sys_timer_remove_working_noLock(pTimer);
	}
}

/////
void sys_timer_check_expired()
{
	sys_chain_node_t *pExpiredNode, *pCurrentNode;
	
	if (pTimersPool == NULL) return;

	//lock working chain
	sys_mutex_lock(WorkingTimerChain.pLock);
	//

	if (WorkingTimerChain.count > 0)
	{
		pCurrentNode = WorkingTimerChain.pHead;
		_TIMER(pCurrentNode)->timeout = _TIMER(pCurrentNode)->timeout - TIME_GRANULARITY;

		while ((pCurrentNode != NULL) && (_TIMER(pCurrentNode)->timeout <= 0))
		{
			pExpiredNode = pCurrentNode;
			(_TIMER(pExpiredNode)->pTimeoutCallBackfunc)(_TIMER(pExpiredNode)->pCallBackParamData); // call callback function
			sys_timer_remove_working(_TIMER(pExpiredNode), LOCK_OFF);

			if (_TIMER(pExpiredNode)->reRegister)
			{
				_TIMER(pExpiredNode)->timeout = _TIMER(pExpiredNode)->expireTime;
				sys_timer_set_working(_TIMER(pExpiredNode), LOCK_OFF);
			}
			else
			{
				sys_timer_set_idle(_TIMER(pExpiredNode), LOCK_ON);
			}
			pCurrentNode = WorkingTimerChain.pHead;
		}
	}

	//
	sys_mutex_unlock(WorkingTimerChain.pLock);
	//unlock working chain
}

void *sys_timer_task(void *pParam)
{
	int startTime, stopTime;
	int temptime, wTimes, compensator=0;
	
	startTime = sys_gettickcount();
	while (!TimerTerminated)
	{
		sys_usleep(TIME_GRANULARITY * 1000);
		stopTime = sys_gettickcount();
		temptime = stopTime - startTime + compensator;

		if (temptime < 0) 
			temptime = TIME_GRANULARITY;

		compensator = temptime % TIME_GRANULARITY;
		wTimes = temptime / TIME_GRANULARITY;

		if(wTimes > TIMER_MAX_LOST_TICK)
		{
			wTimes = 1;
		}

		while (wTimes)
		{
			sys_timer_check_expired();
			wTimes--;
		}

		startTime = stopTime;
	}
	
	return 0;
}

sys_timer_t *sys_timer_add(int ExpireTime, TimerCallBackFunc pCBfunc, void *pCBParam, SYSBOOL StartNow, SYSBOOL reReg)
{
	sys_timer_t *pTimer;

	if (pTimersPool == NULL) return NULL;

	pTimer = (sys_timer_t*)sys_malloc_obj(pTimersPool);
	if (pTimer == NULL)
		return NULL;

	pTimer->expireTime = ExpireTime;
	pTimer->reRegister = reReg;
	pTimer->pCallBackParamData = pCBParam;
	pTimer->pTimeoutCallBackfunc = pCBfunc;

	if (StartNow)
		sys_timer_set_working(pTimer,LOCK_ON);
	else
		sys_timer_set_idle(pTimer,LOCK_ON);

	return pTimer;
}

void sys_timer_delete(sys_timer_t *pTimer)
{
	if ((pTimersPool == NULL) || (pTimer == NULL)) return;

	//lock working chain
	sys_mutex_lock(WorkingTimerChain.pLock);
	//	
		
	if (pTimer->isIdle)
		sys_timer_remove_idle(pTimer, LOCK_ON); //this lock must on !!
	else
		sys_timer_remove_working(pTimer, LOCK_OFF);

	//
	sys_mutex_unlock(WorkingTimerChain.pLock);
	//unlock working chain

	sys_free_obj(pTimersPool, pTimer);
}

void sys_timer_stop(sys_timer_t *pTimer)
{
	if ((pTimersPool == NULL) || (pTimer == NULL)) return;

	//lock working chain
	sys_mutex_lock(WorkingTimerChain.pLock);
	//
	
	if (pTimer->isIdle != TRUE)
	{
		sys_timer_remove_working(pTimer, LOCK_OFF);
		sys_timer_set_idle(pTimer, LOCK_ON);
	}
	
	//
	sys_mutex_unlock(WorkingTimerChain.pLock);
	//unlock working chain
}

void sys_timer_restart(sys_timer_t *pTimer, int ExpireTime, TimerCallBackFunc pCBfunc, void *pCBParam, SYSBOOL reReg)
{
	if ((pTimersPool == NULL) || (pTimer == NULL)) return;

	//lock working chain
	sys_mutex_lock(WorkingTimerChain.pLock);
	//
	
	if (pTimer->isIdle != TRUE) //stop it
	{
		sys_timer_remove_working(pTimer, LOCK_OFF);
		sys_timer_set_idle(pTimer, LOCK_ON);
	}
	
	pTimer->expireTime = ExpireTime;
	pTimer->pTimeoutCallBackfunc = pCBfunc;
	pTimer->pCallBackParamData = pCBParam;
	pTimer->reRegister = reReg;

	sys_timer_remove_idle(pTimer, LOCK_ON);
	pTimer->timeout = pTimer->expireTime;
	sys_timer_set_working(pTimer, LOCK_OFF);

	//
	sys_mutex_unlock(WorkingTimerChain.pLock);
	//unlock working chain
}

void sys_timer_resume(sys_timer_t *pTimer)
{
	sys_timer_restart(pTimer, pTimer->expireTime, pTimer->pTimeoutCallBackfunc, pTimer->pCallBackParamData, pTimer->reRegister);
}

//void TIMER_DEBUG(int doa)
//{
//	sys_chain_node_t *pNode;
//
//	if (doa)
//		sys_timer_check_expired();
//
//	pNode = WorkingTimerChain.pHead;
//	sys_trace(NULL, 0, SYS_INFO4, "working times:%d\n",WorkingTimerChain.count);
//	sys_trace(NULL, 0, SYS_INFO4, "idle times:%d\n",IdleTimerChain.count);
//
//	while (pNode != NULL)
//	{
//		sys_trace(NULL, 0, SYS_INFO4, "NO:%d, timeout:%d\n",*(int*)_TIMER(pNode)->pCallBackParamData,_TIMER(pNode)->timeout);
//		pNode = pNode->pNext;
//	}
//}
//
//void PrintWorking(void)
//{
//	sys_chain_node_t *pNode;
//
//	pNode = WorkingTimerChain.pHead;
//	sys_trace(NULL, 0, SYS_INFO7, "===============================\n", _TIMER(pNode)->timeout);
//	while (pNode != NULL)
//	{
//		sys_trace(NULL, 0, SYS_INFO7, "timeout:<%d>%d\n", pNode, _TIMER(pNode)->timeout);
//		pNode = pNode->pNext;
//	}
//}
//
//void DO_TRACE(sys_timer_t *pTimer, char* pStr)
//{
//	if (pTimer == pTraceTimer)
//	{
//		sys_trace(NULL, 0, SYS_INFO7, "<<<<<%s\n>>>>>", pStr);
//		PrintWorking();
//	}
//}
