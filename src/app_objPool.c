#include "sys_objPool.h"
#include "sys_type.h"


/*
 Create and return object pool, this function will call 
 sys_init_ObjPool() to init object pool.
*/
ObjPool_t* sys_create_ObjPool(int MaxObjects, int ObjectSize, char *Name)
{
	ObjPool_t *pPool;

	pPool = sys_create_ObjPool_ex(MaxObjects, ObjectSize, NULL, NULL, Name);
	
	return pPool;
}

ObjPool_t* sys_create_ObjPool_ex(int MaxObjects, int ObjectSize, ObjInitFunc pInitFunc, ObjFreeFunc pFreeFunc, char *Name)
{
	ObjPool_t* pPool;
	int ret;

	pPool = sys_malloc(sizeof(ObjPool_t));
	if (pPool == NULL)
		return NULL;

	memset(pPool,0,sizeof(ObjPool_t));
	pPool->MaxObjectNum = MaxObjects;
	pPool->ObjectSize = ObjectSize;
	pPool->pInitFunc = pInitFunc;
	pPool->pFreeFunc = pFreeFunc;
	strcpy(pPool->Name, Name);

	ret = sys_init_ObjPool(pPool);
	if (ret == SYS_OK)
	{
		return pPool;
	}
	else
	{
		sys_free_ObjPool(pPool);
		sys_free(pPool);
		return NULL;
	}
}

/*
 destroy object pool, this function will call 
 sys_free_ObjPool() to free the memory allocated for objects
 and call sys_free() to free pObjPool.
*/
void sys_destroy_ObjPool(ObjPool_t *pObjPool)
{
	if (pObjPool != NULL)
	{
		sys_free_ObjPool(pObjPool);
		sys_free(pObjPool);
	}
}

/*
 Allocate memory for objects. before this call,
 you must set 'MaxobjectNum' and 'ObjectSize' in 
 pObjPool param.
*/
int sys_init_ObjPool(ObjPool_t *pObjPool)
{
	int i;

	if (pObjPool == NULL) return SYS_E_NOTINIT;

	pObjPool->Lock = sys_mutex_init();
	if (pObjPool->Lock == NULL)
		return SYS_E_NOMEM;
	
	pObjPool->pMemPool = sys_malloc(sizeof(ObjContainer_t) * pObjPool->MaxObjectNum);
	if (pObjPool->pMemPool == NULL)
		return SYS_E_NOMEM;

	pObjPool->pLast = pObjPool->pMemPool;
	pObjPool->pFirst = pObjPool->pLast + (pObjPool->MaxObjectNum - 1);
	
	// Init all objects to NULL.
	pObjPool->Objects = 0;
	for (i=0; i < pObjPool->MaxObjectNum; i++)
	{
		(pObjPool->pLast + i)->pObj = NULL; 
	}
	
	// Get mem for obj.
	for (i=0; i < pObjPool->MaxObjectNum; i++)
	{
		if (pObjPool->pInitFunc == NULL)
			(pObjPool->pLast + i)->pObj = sys_malloc(pObjPool->ObjectSize);
		else
			(pObjPool->pLast + i)->pObj = pObjPool->pInitFunc();

		if ((pObjPool->pLast + i)->pObj == NULL) 
			return SYS_E_NOMEM;
	}

	pObjPool->Objects = pObjPool->MaxObjectNum;
	return SYS_OK;
}

/*
 Only free the memory allocated for objects,
 the sys_free() call on the ObjPool is not
 automatically done by sys_FreeObjPool().
*/
void sys_free_ObjPool(ObjPool_t *pObjPool)
{
	int i;

	if (pObjPool == NULL) return;

	sys_mutex_destroy(pObjPool->Lock);

	if (pObjPool->pMemPool == NULL)
		return;
	
	for (i=0; i < pObjPool->MaxObjectNum; i++)
	{
		if ((pObjPool->pLast + i)->pObj != NULL)
		{
			if (pObjPool->pFreeFunc == NULL)
			{
				sys_free((pObjPool->pLast + i)->pObj);
			}
			else
			{
				pObjPool->pFreeFunc((pObjPool->pLast + i)->pObj);
			}
			(pObjPool->pLast + i)->pObj = NULL;
		}
	}
	sys_free(pObjPool->pMemPool);

	if (pObjPool->Objects != pObjPool->MaxObjectNum)
		sys_trace(__FILE__, __LINE__, SYS_FATAL, "Invalid amount of objects when destroy objpool:%s (%d,%d)\n",
		          pObjPool->Name, pObjPool->Objects, pObjPool->MaxObjectNum);

	memset(pObjPool,0,sizeof(ObjPool_t));
}

/*
 Get an object from object pool.
 return NULL means fail.
*/
void *sys_malloc_obj(ObjPool_t *pObjPool)
{
	void *pResult;

	if (pObjPool == NULL) return NULL;

	//lock
	sys_mutex_lock(pObjPool->Lock);

	if (pObjPool->Objects == 0)
	{
		pResult = NULL;
		sys_trace(__FILE__, __LINE__, SYS_FATAL, "Lack of objects in ObjPool:%s !!\n",pObjPool->Name);
	}
	else
	{
		pResult = pObjPool->pFirst->pObj;
		pObjPool->pFirst->pObj = NULL;
		pObjPool->pFirst = pObjPool->pFirst - 1;
		pObjPool->Objects--;
	}

	//unlock
	sys_mutex_unlock (pObjPool->Lock);
	
	return pResult;
}

/*
 Give object back to object pool.
*/
void sys_free_obj(ObjPool_t *pObjPool, void *pObj)
{
	if ((pObjPool == NULL) || (pObj == NULL)) return;

	//lock
	sys_mutex_lock(pObjPool->Lock);

	if (pObjPool->Objects < pObjPool->MaxObjectNum)
	{
		pObjPool->pFirst = pObjPool->pFirst + 1;
		pObjPool->pFirst->pObj = pObj;
		pObjPool->Objects++;
	}
	else
	{
		sys_trace(__FILE__, __LINE__, SYS_FATAL, "ObjPool:%s is full(%d objects), but sys_free_obj() been call !!\n",
		          pObjPool->Name, pObjPool->Objects);
	}
	
	//unlock
	sys_mutex_unlock (pObjPool->Lock);
}