#include <sys_list.h>
#include "sys_type.h"

void sys_list_Init (sys_list_t* pList)
{
   if (pList) {
      pList->count = 0;
      pList->pHead = (sys_list_node_t*) 0;
      pList->pTail = (sys_list_node_t*) 0;
   }
}

sys_list_node_t* sys_list_Append (sys_list_t* pList, void* pData)
{
   sys_list_node_t* pListNode = (sys_list_node_t*)sys_malloc(sizeof(sys_list_node_t));

   if (0 != pListNode) {
      pListNode->pData = pData;
   }

   return sys_list_AppendNode(pList, pListNode);
}

sys_list_node_t* sys_list_AppendNode (sys_list_t* pList, sys_list_node_t* pNode)
{
   sys_list_node_t* pListNode = pNode;

   if (0 != pListNode) {
      pListNode->pNext = (sys_list_node_t*) 0;
      if (0 != pList->pTail) {
         pList->pTail->pNext = pListNode;
         pListNode->pPrev = pList->pTail;
      }
      if (0 == pList->pHead) {
         pList->pHead = pListNode;
         pListNode->pPrev = (sys_list_node_t*) 0;
      }
      pList->pTail = pListNode;
      pList->count++;
   }

   return pListNode;
}

/* Delete the head node from the list and return the data item stored	*/
/* in that node..							*/

void *sys_list_DeleteHead (sys_list_t* pList)
{
   sys_list_node_t* pNode = (0 != pList) ? pList->pHead : 0;

   if (0 != pNode) {
      void* pdata = pNode->pData;
      sys_list_Remove (pList, pNode);
      sys_free(pNode);
      return pdata;
   }
   return 0;
}

/* Free all nodes, but not the data */
void sys_list_FreeNodes (sys_list_t* pList)
{
   sys_list_node_t *pNode, *pNextNode;

   pNode = pList->pHead;
   while ( pNode != 0 ) {
      pNextNode = pNode->pNext;
      sys_free(pNode);
	  pNode = pNextNode;
   }
   pList->count = 0;
   pList->pHead = pList->pTail = 0;
}

/* Free all nodes and their data */
void sys_list_FreeAll (sys_list_t* pList)
{
   sys_list_node_t *pNode, *pNextNode;

   pNode = pList->pHead;
   while ( pNode != 0 ) {
      pNextNode = pNode->pNext;
	  sys_free(pNode->pData);
      sys_free(pNode);
	  pNode = pNextNode;
   }
   pList->count = 0;
   pList->pHead = pList->pTail = 0;
}

/* Remove node from list. Node is not freed */
void sys_list_Remove (sys_list_t* pList, sys_list_node_t* pNode)
{
   if(pNode->pNext != 0) {
      pNode->pNext->pPrev = pNode->pPrev;
   }
   else { /* tail */
      pList->pTail = pNode->pPrev;
   }

   if(pNode->pPrev != 0) {
      pNode->pPrev->pNext = pNode->pNext;
   }
   else { /* head */
      pList->pHead = pNode->pNext;
   }

   pNode->pNext = (sys_list_node_t*) 0;
   pNode->pPrev = (sys_list_node_t*) 0;
   pList->count--;
}

void sys_list_FindAndRemove(sys_list_t* pList, void *pData)
{
   sys_list_node_t *pNode;

   pNode = pList->pHead;
   while (pNode !=0){
      if(pNode->pData == pData) /* pointer comparison*/
         break;
	  pNode = pNode->pNext;
   }
   if(pNode)
      sys_list_Remove(pList, pNode);
}

void *sys_list_Delete(sys_list_t* pList, sys_list_node_t* pNode)
{
	void *pData;

	pData = pNode->pData;
	sys_list_Remove(pList, pNode);
	sys_free(pNode);

	return pData;
}

void *sys_list_FindAndDelete(sys_list_t* pList, void* pData)
{
   sys_list_node_t *pNode;
   void *p;

   pNode = pList->pHead;
   while (pNode !=0){
      if(pNode->pData == pData) /* pointer comparison*/
         break;
	  pNode = pNode->pNext;
   }
   if(pNode)
   {
	  p = pNode->pData;
      sys_list_Remove(pList, pNode);
	  sys_free(pNode);
   }
   else
   {
	  p = NULL;
   }

   return p;
}
    
sys_list_node_t* sys_list_FindByIndex (sys_list_t* pList, int index) 
{
   sys_list_node_t* pNode;
   int i;

   if(index >= (int)pList->count) return 0;
   for(i = 0, pNode = pList->pHead; (i < index) && (pNode != 0); i++) {
      pNode = pNode->pNext;
   }
   return pNode;
}

//------sys_chain------------------------------------------------------------------------------------------

int sys_chain_init(sys_chain_t *pChain)
{
	if (pChain == NULL)
		return -1;

	pChain->count = 0;
	pChain->pHead = NULL;
	pChain->pTail = NULL;
	pChain->pLock = sys_mutex_init();
	if (pChain->pLock == NULL)
		return -1;
	else
		return SYS_OK;
}

int sys_chain_init_noLock(sys_chain_t *pChain)
{
	if (pChain == NULL)
		return -1;

	pChain->count = 0;
	pChain->pHead = NULL;
	pChain->pTail = NULL;
	pChain->pLock = NULL;

	return SYS_OK;
}

/**
 * Free the mutex only. 
 * Caution: you must free all node on the chain before you call it.
 */
void sys_chain_free(sys_chain_t *pChain)
{
	if (pChain == NULL)
		return;

	pChain->count = 0;
	pChain->pHead = NULL;
	pChain->pTail = NULL;
	sys_mutex_destroy(pChain->pLock);
	pChain->pLock = NULL;
}

int sys_chain_count(sys_chain_t *pChain)
{
	if (pChain == NULL)
		return 0;

	return pChain->count;
}

void sys_chain_add_after_noLock(sys_chain_t *pChain, sys_chain_node_t *pNodeToAdd, sys_chain_node_t *pNodeToLocate)
{
	pNodeToAdd->pNext = pNodeToLocate->pNext;
	pNodeToAdd->pPrev = pNodeToLocate;

	if (pNodeToLocate->pNext != NULL)
		pNodeToLocate->pNext->pPrev = pNodeToAdd;
	else
		pChain->pTail = pNodeToAdd;

	pNodeToLocate->pNext = pNodeToAdd;

	pChain->count++;
}

void sys_chain_add_before_noLock(sys_chain_t *pChain, sys_chain_node_t *pNodeToAdd, sys_chain_node_t *pNodeToLocate)
{
	pNodeToAdd->pNext = pNodeToLocate;
	pNodeToAdd->pPrev = pNodeToLocate->pPrev;

	if (pNodeToLocate->pPrev != NULL)
		pNodeToLocate->pPrev->pNext = pNodeToAdd;
	else
		pChain->pHead = pNodeToAdd;

	pNodeToLocate->pPrev = pNodeToAdd;

	pChain->count++;
}

void sys_chain_add_tail_noLock(sys_chain_t *pChain, sys_chain_node_t *pNode)
{
	if (pChain->pTail == NULL)
	{
		pNode->pNext = NULL;
		pNode->pPrev = NULL;
		pChain->pHead = pNode;
		pChain->pTail = pNode;
		pChain->count = 1;
	}
	else
	{
		sys_chain_add_after_noLock(pChain,pNode,pChain->pTail);
	}
}

void sys_chain_add_head_noLock(sys_chain_t *pChain, sys_chain_node_t *pNode)
{
	if (pChain->pHead == NULL)
	{
		pNode->pNext = NULL;
		pNode->pPrev = NULL;
		pChain->pHead = pNode;
		pChain->pTail = pNode;
		pChain->count = 1;
	}
	else
	{
		sys_chain_add_before_noLock(pChain,pNode,pChain->pHead);
	}
}

void sys_chain_remove_noLock(sys_chain_t *pChain, sys_chain_node_t *pNode)
{
	if (pNode->pNext != NULL){
		pNode->pNext->pPrev = pNode->pPrev;
	}
	else{
		pChain->pTail = pNode->pPrev;
	}

	if (pNode->pPrev != NULL){
		pNode->pPrev->pNext = pNode->pNext;
	}
	else{
		pChain->pHead = pNode->pNext;
	}

	pNode->pNext = NULL;
	pNode->pPrev = NULL;
	pChain->count--;
}

void sys_chain_add_after(sys_chain_t *pChain, sys_chain_node_t *pNodeToAdd, sys_chain_node_t *pNodeToLocate)
{
	if ((pChain == NULL) || (pNodeToAdd == NULL) || (pNodeToLocate == NULL))
		return;

	//lock
	sys_mutex_lock(pChain->pLock);
	//
	sys_chain_add_after_noLock(pChain, pNodeToAdd, pNodeToLocate);
	//
	sys_mutex_unlock(pChain->pLock);
	//unlock
}

void sys_chain_add_before(sys_chain_t *pChain, sys_chain_node_t *pNodeToAdd, sys_chain_node_t *pNodeToLocate)
{
	if ((pChain == NULL) || (pNodeToAdd == NULL) || (pNodeToLocate == NULL))
		return;

	//lock
	sys_mutex_lock(pChain->pLock);
	//
	sys_chain_add_before_noLock(pChain, pNodeToAdd, pNodeToLocate);
	// 
	sys_mutex_unlock(pChain->pLock);
	//unlock
}

void sys_chain_add_tail(sys_chain_t *pChain, sys_chain_node_t *pNode)
{
	if ((pNode == NULL)||(pChain == NULL))
		return;

	//lock
	sys_mutex_lock(pChain->pLock);
	//
	sys_chain_add_tail_noLock(pChain, pNode);
	// 
	sys_mutex_unlock(pChain->pLock);
	//unlock
}

void sys_chain_add_head(sys_chain_t *pChain, sys_chain_node_t *pNode)
{
	if ((pNode == NULL)||(pChain == NULL))
		return;

	//lock
	sys_mutex_lock(pChain->pLock);
	//
	sys_chain_add_head_noLock(pChain, pNode);
	// 
	sys_mutex_unlock(pChain->pLock);
	//unlock
}

void sys_chain_remove(sys_chain_t *pChain, sys_chain_node_t *pNode)
{
	if ((pNode == NULL)||(pChain == NULL))
		return;

	//lock
	sys_mutex_lock(pChain->pLock);
	//
	sys_chain_remove_noLock(pChain, pNode);
	//
	sys_mutex_unlock(pChain->pLock);
	//unlock
}

