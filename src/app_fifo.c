#include "sys_type.h"
#include "sys_fifo.h"



sys_fifo_t *sys_fifo_create(int MaxElements, int MaxElementSize)
{
	sys_fifo_t *pFifo;
	int res;

	pFifo = sys_malloc(sizeof(sys_fifo_t));
	if (pFifo == NULL)
		return pFifo;

	memset(pFifo,0,sizeof(sys_fifo_t));
	pFifo->MaxElements = MaxElements;
	pFifo->MaxElementSize = MaxElementSize;

	res = sys_fifo_init(pFifo);
	if (res != SYS_OK)
	{
		sys_fifo_free(pFifo);
		sys_free(pFifo);
		return NULL;
	}
	else
	{
		return pFifo;
	}
}

void sys_fifo_destroy(sys_fifo_t *pFifo)
{
	if (pFifo != NULL)
	{
		sys_fifo_free(pFifo);
		sys_free(pFifo);
	}
}

int sys_fifo_init (sys_fifo_t * pFifo)
{
	int FifoSize;

	if (pFifo == NULL) return SYS_E_NOTINIT;

	pFifo->pLock = sys_mutex_init();
	if (pFifo->pLock == NULL)
		return SYS_E_NOMEM;

	pFifo->pNotEmptySem = sys_sem_init(0);
	if (pFifo->pNotEmptySem == NULL)
		return SYS_E_NOMEM;

	FifoSize = pFifo->MaxElements * pFifo->MaxElementSize;
	pFifo->pBuffer = sys_malloc(FifoSize);
	if (pFifo->pBuffer == NULL) return SYS_E_NOMEM;

	pFifo->elments = 0;
	pFifo->pAdd = pFifo->pBuffer;
	pFifo->pGet = pFifo->pBuffer;
	pFifo->pBufferEnd = pFifo->pBuffer + FifoSize;

	return SYS_OK;
}


void sys_fifo_free (sys_fifo_t * pFifo)
{
	if (pFifo == NULL) return;

	sys_mutex_destroy(pFifo->pLock);
	sys_sem_destroy(pFifo->pNotEmptySem);

	if (pFifo->pBuffer !=  NULL)
	{
		sys_free(pFifo->pBuffer);
		pFifo->pBuffer = NULL;
	}
	memset(pFifo, 0, sizeof(sys_fifo_t));
}

void sys_fifo_increase(sys_fifo_t * pFifo)
{
	pFifo->pAdd = pFifo->pAdd + pFifo->MaxElementSize;
	if (pFifo->pAdd == pFifo->pBufferEnd)
		pFifo->pAdd = pFifo->pBuffer; //point to begin
	pFifo->elments++;
}

void sys_fifo_decrease(sys_fifo_t * pFifo)
{
	pFifo->pGet = pFifo->pGet +pFifo->MaxElementSize;
	if (pFifo->pGet == pFifo->pBufferEnd)
		pFifo->pGet = pFifo->pBuffer; //point to begin
	pFifo->elments--;
}


int sys_fifo_add (sys_fifo_t * pFifo, void *pData, int DataLen)
{
	sys_mutex_lock(pFifo->pLock);

	if ((DataLen > pFifo->MaxElementSize) || (pFifo->elments >= pFifo->MaxElements))
	{
		sys_mutex_unlock(pFifo->pLock);
		return SYS_E_OUTOFBND;
	}

	memcpy(pFifo->pAdd, pData, DataLen);
	sys_fifo_increase(pFifo);
	sys_sem_post(pFifo->pNotEmptySem);

	sys_mutex_unlock(pFifo->pLock);

	return SYS_OK;
}


int sys_fifo_get (sys_fifo_t * pFifo, void *pData)
{
	int sem;
	
	sem = sys_sem_wait(pFifo->pNotEmptySem);
	if(sem != 0)
		return 0;

	sys_mutex_lock(pFifo->pLock);

	memcpy(pData, pFifo->pGet, pFifo->MaxElementSize);
	sys_fifo_decrease(pFifo);

	sys_mutex_unlock(pFifo->pLock);

	return pFifo->MaxElementSize;
}


int sys_fifo_tryget (sys_fifo_t * pFifo, void *pData)
{
	if (pFifo->elments == 0)
		return 0;
	return sys_fifo_get(pFifo, pData);
}


SYSBOOL sys_fifo_isfull (sys_fifo_t * pFifo)
{
	SYSBOOL isFull;
	
	sys_mutex_lock(pFifo->pLock);

	if (pFifo->elments >= pFifo->MaxElements)
		isFull = TRUE;
	else
		isFull = FALSE;

	sys_mutex_unlock(pFifo->pLock);

	return isFull;
}