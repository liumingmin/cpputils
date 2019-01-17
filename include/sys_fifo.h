
#ifndef _FIFO_H_
#define _FIFO_H_

#include "sys_port.h"

#ifndef SYS_MT
#error "SYS_MT wasn't defined"
#endif

#include "sys_mt.h"
#include "sys_type.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Structure for referencing a fifo.
 * @var sys_fifo_t
 */
  typedef struct sys_fifo sys_fifo_t;

/**
 * Structure for referencing a fifo.
 * @struct sys_fifo
 */
  struct sys_fifo
  {
    struct sys_mutex *pLock;
    struct sys_sem *pNotEmptySem;
	//------------------------
	int MaxElements;
	int MaxElementSize;
    int elments;                    /**< number of elements */
	char *pBuffer;
	char *pBufferEnd;
	char *pAdd;
	char *pGet;
  };


/**
 * Create a fifo.
 * return NULL means create fail.
 */
  sys_fifo_t *sys_fifo_create(int MaxElements, int MaxElementSize);
/**
 * Destroy a fifo.
 */
  void sys_fifo_destroy(sys_fifo_t *pFifo);
/**
 * Init a fifo.
 * return SYS_OK if init succeeded.
 */
  int sys_fifo_init (sys_fifo_t * pFifo);
/**
 * Free a fifo element.
 */
  void sys_fifo_free (sys_fifo_t * pFifo);
/**
 * Add an element in a fifo.
 * return SYS_OK if succeeded.
 * Caution: you'd better check whether the fifo is full before you call sys_fifo_add().
 */
  int sys_fifo_add (sys_fifo_t * pFifo, void *pData, int DataLen);
/**
 * Get an element from a fifo or block until one is added.
 * param pData : buffer to hold data get from fifo.
 * return number bytes got from fifo.
 * Caution: the buffer to hold data must have enough size.
 */
  int sys_fifo_get (sys_fifo_t * pFifo, void *pData);
/**
 * Try to get an element from a fifo, but do not block if there is no element.
 * return number bytes got from fifo.
 */
  int sys_fifo_tryget (sys_fifo_t * pFifo, void *pData);
/**
 * Check the fifo is full or not.
 */
  SYSBOOL sys_fifo_isfull (sys_fifo_t * pFifo);



#ifdef __cplusplus
}
#endif


#endif
