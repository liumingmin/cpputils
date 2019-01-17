#ifndef _SYS_OBJPOOL_H_
#define _SYS_OBJPOOL_H_

#include "sys_port.h"

#ifndef SYS_MT
#error "SYS_MT wasn't defined"
#endif

#include <sys_mt.h>


#ifdef __cplusplus
extern "C"
{
#endif


typedef void *(*ObjInitFunc)(void);
typedef void (*ObjFreeFunc)(void *pObj);

typedef struct
{
	void * pObj;	
}
ObjContainer_t;
	
typedef struct 
{
	struct sys_mutex *Lock;
	ObjInitFunc pInitFunc;
	ObjFreeFunc pFreeFunc;
	void *pMemPool;
	ObjContainer_t *pLast;
	ObjContainer_t *pFirst;
	int MaxObjectNum;
	int Objects;    // objects count in pool.
	int ObjectSize;
	char Name[32];
}
ObjPool_t;


/*
 Create and return object pool, this function will call 
 sys_init_ObjPool() to init object pool.
*/
ObjPool_t* sys_create_ObjPool(int MaxObjects, int ObjectSize, char *Name);
ObjPool_t* sys_create_ObjPool_ex(int MaxObjects, int ObjectSize, ObjInitFunc pInitFunc, ObjFreeFunc pFreeFunc, char *Name);

/*
 destroy object pool, this function will call 
 sys_free_ObjPool() to free the memory allocated for objects
 and call sys_free() to free pObjPool.
*/
void sys_destroy_ObjPool(ObjPool_t *pObjPool);

/*
 Allocate memory for objects. before this call,
 you must set 'MaxobjectNum' and 'ObjectSize' in 
 pObjPool param.
 return SYS_OK if init succeeded.
*/
int sys_init_ObjPool(ObjPool_t *pObjPool);

/*
 Only free the memory allocated for objects,
 the sys_free() call on the 'pObjPool' is not
 automatically done by sys_free_ObjPool().
*/
void sys_free_ObjPool(ObjPool_t *pObjPool);

/*
 Get an object from object pool.
 return NULL means fail.
*/
void *sys_malloc_obj(ObjPool_t *pObjPool);

/*
 Give object back to object pool.
 Caution: don't call sys_free_obj() on same pointer more then one times,
          the implementation of sys_free_obj() can't avoid this.
*/
void sys_free_obj(ObjPool_t *pObjPool, void *pObj);


#ifdef __cplusplus
}
#endif
	

#endif /* #ifndef _SYS_OBJPOOL_H_ */