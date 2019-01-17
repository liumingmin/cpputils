
#ifndef _LIST_H_
#define _LIST_H_

#include "sys_port.h"

#ifndef SYS_MT
#error "SYS_MT wasn't defined"
#endif

#include "sys_mt.h"


typedef struct _DListNode {
   void* pData;
   struct _DListNode* pNext;
   struct _DListNode* pPrev;
} sys_list_node_t;

typedef struct _DList {
   unsigned int count;
   sys_list_node_t* pHead;
   sys_list_node_t* pTail;
} sys_list_t;


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * This function initializes a doubly linked list structure. It sets the number
 * of elements to zero and sets all internal pointer values to NULL. A doubly
 * linked-list structure is described by the DList type. Nodes of the list 
 * are of type DListNode.
 *
 * @param pList        A pointer to a linked list structure to be initialized.
 */
void sys_list_Init (sys_list_t* pList);

/**
 * This function removes all nodes from the linked list and releases the memory
 * that was allocated for storing the node structures (DListNode). The data
 * will not be released.
 *
 * @param pList        A pointer to a linked list structure onto which the data
 *                     item is to be appended. A pointer to an updated linked
 *                     list structure.
 */
void sys_list_FreeNodes (sys_list_t* pList);

/** 
 * This function removes all nodes from the linked list structure and releases
 * the memory that was allocated for storing the node structures
 * (DListNode) and for data. The memory for data in each node must have
 * been previously allocated with calls to sys_malloc() function.
 *
 * @param pList        Pointer to a linked list structure.
 */
void sys_list_FreeAll (sys_list_t* pList);

/**
 * This function appends an item to the linked list structure. The data item is
 * passed into the function as a void pointer that can point to any object of
 * any type. The sys_malloc() function is used to allocated the memory for the
 * list node structure; therefore, all internal list memory will be released
 * whenever sys_free() is called. The pointer to the data item itself is stored
 * in the node structure - a copy is not made.
 *
 * @param pList        A pointer to a linked list structure onto which the data
 *                     item is to be appended. A pointer to an updated linked
 *                     list structure.
 * @param pData        A pointer to a data item to be appended to the list.
 * @return             A pointer to an allocated node structure used to link
 *                     the given data value into the list.
 */ 
sys_list_node_t *sys_list_Append (sys_list_t* pList, void* pData);

sys_list_node_t *sys_list_AppendNode (sys_list_t* pList, sys_list_node_t* pNode);

/**
 * This function delete the head item from the list and returns a pointer 
 * the data item stored in that node.  The memory for the node structure 
 * is released.
 *
 * @param pList        A pointer to the linked list structure from which 
 *                     the node will be deleted.
 * @return             A pointer to the data item stored in the deleted node.
 */ 
void *sys_list_DeleteHead (sys_list_t* pList);
void *sys_list_Delete(sys_list_t* pList, sys_list_node_t* pNode);
void *sys_list_FindAndDelete(sys_list_t* pList, void* data);


/**
 * This function only remove the node from list 
 * the data item stored in that node.  The memory for the node structure 
 * and data structure is not released.
 */ 
void sys_list_Remove (sys_list_t* pList, sys_list_node_t* node);
void sys_list_FindAndRemove(sys_list_t* pList, void* data);
sys_list_node_t *sys_list_FindByIndex (sys_list_t* pList, int index);

/**
 * This function inserts an item into the linked list structure before the 
 * specified element.
 * 
 * @param pList		A pointer to a linked list structure into which the 
 *                        data item is to be inserted.
 * @param node          The position in the list where the item is to be 
 *                        inserted.  The item will be inserted before this 
 *                        node or appended to the list if node is null.
 * @param pData		A pointer to the data item to be inserted to the list.
 * @return		A pointer to an allocated node structure used to 
 *                        link the given data value into the list.
 */
//DListNode* sys_list_InsertBefore (sys_list_t* pList, sys_list_node_t* node, const void* pData);

/**
 * This function inserts an item into the linked list structure after the 
 * specified element.
 * 
 * @param pList		A pointer to a linked list structure into which the 
 *                        data item is to be inserted.
 * @param node          The position in the list where the item is to be 
 *                        inserted.  The item will be inserted after this 
 *                        node or added as the head element if node is null.
 * @param pData		A pointer to the data item to be inserted to the list.
 * @return		A pointer to an allocated node structure used to 
 *                        link the given data value into the list.
 */
//DListNode* sys_list_InsertAfter (sys_list_t* pList, sys_list_node_t* node, const void* pData);

/**
 * This function removes a node from the linked list structure. The memAlloc
 * function was used to allocate the memory for the list node structure,
 * therefore, all internal list memory will be released whenever memFree or
 * memFreePtr is called.
 *
 * @param pList        A pointer to a linked list structure onto which the data
 *                     item is to be removed. A pointer to an updated linked
 *                     list structure.
 * @param node         A pointer to the node that is to be removed. It should
 *                     already be in the linked list structure.
 */

//--------------------------------------------------------------------------------------------------------

typedef struct _Node {
   struct _Node* pNext;
   struct _Node* pPrev;
} sys_chain_node_t;

typedef struct{
	struct sys_mutex *pLock;
	unsigned int count;
	sys_chain_node_t* pHead;
	sys_chain_node_t* pTail;
} sys_chain_t;


int sys_chain_init(sys_chain_t *pChain);

/**
 * Free the mutex only. 
 * Caution: you must free all node on the chain before you call it.
 */   
void sys_chain_free(sys_chain_t *pChain);

void sys_chain_add_tail(sys_chain_t *pChain, sys_chain_node_t *pNode);
void sys_chain_add_head(sys_chain_t *pChain, sys_chain_node_t *pNode);
void sys_chain_add_before(sys_chain_t *pChain, sys_chain_node_t *pNodeToAdd, sys_chain_node_t *pNodeToLocate);
void sys_chain_add_after(sys_chain_t *pChain, sys_chain_node_t *pNodeToAdd, sys_chain_node_t *pNodeToLocate);
void sys_chain_remove(sys_chain_t *pChain, sys_chain_node_t *pNode);
int sys_chain_count(sys_chain_t *pChain);


//Call this functions will not lock the chain, be careful when use it.
int sys_chain_init_noLock(sys_chain_t *pChain);
void sys_chain_add_tail_noLock(sys_chain_t *pChain, sys_chain_node_t *pNode);
void sys_chain_add_head_noLock(sys_chain_t *pChain, sys_chain_node_t *pNode);
void sys_chain_add_after_noLock(sys_chain_t *pChain, sys_chain_node_t *pNodeToAdd, sys_chain_node_t *pNodeToLocate);
void sys_chain_add_before_noLock(sys_chain_t *pChain, sys_chain_node_t *pNodeToAdd, sys_chain_node_t *pNodeToLocate);
void sys_chain_remove_noLock(sys_chain_t *pChain, sys_chain_node_t *pNode);
////


#ifdef __cplusplus
}
#endif


#endif
