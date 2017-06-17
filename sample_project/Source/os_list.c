#include "os_lib.h"


#define FIRST_NODE sHeadNode.psNextNode
#define LAST_NODE sHeadNode.psPreNode

/**********************************************************************************************************
** Function name        :   OS_vNodeInit
** Descriptions         :   初始化结点类型
** parameters           :   psNode
** Returned value       :   无
***********************************************************************************************************/
void OS_vNodeInit(tsNode *psNode)
{
	psNode->psPreNode = psNode;
	psNode->psNextNode = psNode;
}

/**********************************************************************************************************
** Function name        :   OS_vListInit
** Descriptions         :   链表初始化
** parameters           :   psList
** Returned value       :   无
***********************************************************************************************************/
void OS_vListInit(tsList *psList)
{
	psList->FIRST_NODE = &(psList->sHeadNode);
	psList->LAST_NODE = &(psList->sHeadNode);
	psList->u32NodeCount = 0;
}

/**********************************************************************************************************
** Function name        :   OS_u32GetListCount
** Descriptions         :   返回链表中结点的数量
** parameters           :   psList
** Returned value       :   结点数量
***********************************************************************************************************/
uint32_t OS_u32GetListCount(tsList *psList)
{
	return psList->u32NodeCount;
}

/**********************************************************************************************************
** Function name        :   OS_psListGetFirstNode
** Descriptions         :   返回链表的首个结点
** parameters           :   psList 查询的链表
** Returned value       :   首个结点，如果链表为空，则返回0
***********************************************************************************************************/
tsNode * OS_psListGetFirstNode(tsList *psList)
{
	tsNode *psNode = (tsNode * )0;
	
	if(psList->u32NodeCount != 0)
	{
		psNode = psList->FIRST_NODE;
	}
	
	return psNode;
}

/**********************************************************************************************************
** Function name        :   OS_psListGetLastNode
** Descriptions         :   返回链表的最后一个结点
** parameters           :   psList 查询的链表
** Returned value       :   最后的结点，如果链表为空，则返回0
***********************************************************************************************************/
tsNode * OS_psListGetLastNode(tsList *psList)
{
	tsNode *psNode = (tsNode *)0;
	
	if(psList->u32NodeCount != 0)
	{
		psNode = psList->LAST_NODE;
	}
	return psNode;
		
}

/**********************************************************************************************************
** Function name        :   OS_psListGetNodePreNode
** Descriptions         :   返回链表中指定结点的前一结点
** parameters           :   list 查询的链表
** parameters           :   psNode 参考结点
** Returned value       :   前一结点结点，如果结点没有前结点（链表为空），则返回0
***********************************************************************************************************/
tsNode * OS_psListGetNodePreNode(tsNode *psNode)
{
	if(psNode->psPreNode == psNode)
	{
		return (tsNode *)0;
	}
	else
	{
		return psNode->psPreNode;
	}
}

/**********************************************************************************************************
** Function name        :   OS_psListGetNodeNextNode
** Descriptions         :   返回链表中指定结点的后一结点
** parameters           :   list 查询的链表
** parameters           :   psNode 参考结点
** Returned value       :   后一结点结点，如果结点没有前结点（链表为空），则返回0
***********************************************************************************************************/
tsNode * OS_psListGetNodeNextNode(tsNode *psNode)
{
	if(psNode->psNextNode == psNode)
	{
		return (tsNode *)0;
	}
	else
	{
		return psNode->psNextNode;
	}
}

/**********************************************************************************************************
** Function name        :   OS_vListRemoveAll
** Descriptions         :   移除链表中的所有结点
** parameters           :   psList 待清空的链表
** Returned value       :   无
***********************************************************************************************************/
void OS_vListRemoveAll(tsList *psList)
{
	uint32_t u32Count = 0;
	
	tsNode *psNextNode;

	psNextNode = psList->FIRST_NODE;
	
	for(u32Count = psList->u32NodeCount; u32Count > 0; u32Count--)
	{
		tsNode *psCurrentNode = psNextNode;
		
		psNextNode = psCurrentNode -> psNextNode;
		
		psCurrentNode -> psPreNode = psCurrentNode;
		psCurrentNode -> psNextNode = psCurrentNode;

	}
			
	psList -> FIRST_NODE = &(psList->sHeadNode);
	psList -> LAST_NODE = &(psList->sHeadNode);
	psList -> u32NodeCount = 0;
}

/**********************************************************************************************************
** Function name        :   OS_vListAddFirst
** Descriptions         :   将指定结点添加到链表的头部
** parameters           :   psList 待插入链表
** parameters						:   psNode 待插入的结点
** Returned value       :   无
***********************************************************************************************************/
void OS_vListAddFirst(tsList *psList, tsNode *psNode)
{	
	psNode->psPreNode = &(psList->sHeadNode);
	psNode->psNextNode = psList->FIRST_NODE;
	
	psList->FIRST_NODE->psPreNode = psNode;
	
	psList->FIRST_NODE = psNode;
	
	psList->u32NodeCount++;
}

/**********************************************************************************************************
** Function name        :   OS_tListAddLast
** Descriptions         :   将指定结点添加到链表的末尾
** parameters           :   psList 待插入链表
** parameters						:   psNode 待插入的结点
** Returned value       :   无
***********************************************************************************************************/
void OS_vListAddLast(tsList *psList, tsNode *psNode)
{
	psNode->psPreNode = psList->LAST_NODE;
	psNode->psNextNode = &(psList->sHeadNode);
	
	psList->LAST_NODE->psNextNode = psNode;
	
	psList->LAST_NODE = psNode;
	
	psList->u32NodeCount++;
}


/**********************************************************************************************************
** Function name        :   OS_psListRemoveFirst
** Descriptions         :   移除链表中的第1个结点
** parameters           :   psList 待移除链表
** Returned value       :   如果链表为空，返回0，否则的话，返回第1个结点
***********************************************************************************************************/
tsNode * OS_psListRemoveFirst(tsList *psList)
{
	tsNode *psNode = (tsNode *)0;
	
	if(psList->u32NodeCount != 0)
	{
		psNode = psList->FIRST_NODE;
		psNode->psNextNode->psPreNode = &(psList->sHeadNode);
		psList->FIRST_NODE = psNode->psNextNode;
		
		psList->u32NodeCount--;
	}
	return psNode;
}

/**********************************************************************************************************
** Function name        :   OS_psListRemoveLast
** Descriptions         :   移除链表中最后1个结点
** parameters           :   psList 待移除链表
** Returned value       :   如果链表为空，返回0，否则的话，返回最后个结点
***********************************************************************************************************/
tsNode * OS_psListRemoveLast(tsList *psList)
{
	tsNode *psNode = (tsNode *)0;
	
	if(psList->u32NodeCount != 0)
	{
		psNode = psList->LAST_NODE;
		psNode->psPreNode->psNextNode = &(psList->sHeadNode);
		psList->LAST_NODE = psNode->psPreNode;
		
		psList->u32NodeCount--;
	}
	return psNode;
}

/**********************************************************************************************************
** Function name        :   OS_vListInsertAfter
** Descriptions         :   将指定的结点插入到某个结点后面
** parameters           :   psList 			待插入的链表
** parameters           :   psNodeRef 		参考结点
** parameters           :   psNodeToInsrt 	待插入的结构
** Returned value       :   无
***********************************************************************************************************/
void OS_vListInsertAfter(tsList *psList, tsNode *psNodeRef, tsNode *psNodeToInsrt)
{
	psNodeToInsrt->psPreNode = psNodeRef;
	psNodeToInsrt->psNextNode = psNodeRef->psNextNode;
	
	psNodeRef->psNextNode->psPreNode = psNodeToInsrt;
	psNodeRef->psNextNode = psNodeToInsrt;
	
	psList->u32NodeCount++;
}

/**********************************************************************************************************
** Function name        :   OS_vListInsertBefore
** Descriptions         :   将指定的结点插入到某个结点前面
** parameters           :   psList 			待插入的链表
** parameters           :   psNodeRef 		参考结点
** parameters           :   psNodeToInsrt 	待插入的结构
** Returned value       :   无
***********************************************************************************************************/
void OS_vListInsertBefore(tsList *psList, tsNode *psNodeRef, tsNode *psNodeToInsrt)
{
	psNodeToInsrt->psPreNode = psNodeRef->psPreNode;
	psNodeToInsrt->psNextNode = psNodeRef;
	
	psNodeRef->psPreNode->psNextNode = psNodeToInsrt;
	psNodeRef->psPreNode = psNodeToInsrt;
	
	psList->u32NodeCount++;
}

/**********************************************************************************************************
** Function name        :   OS_vListRemove
** Descriptions         :   将指定结点从链表中移除
** parameters           :   psList 	待移除的链表
** parameters           :   psNode 	待移除的结点
** Returned value       :   无
***********************************************************************************************************/
void OS_vListRemove(tsList *psList, tsNode *psNode)
{
	psNode->psPreNode->psNextNode = psNode->psNextNode;
	psNode->psNextNode->psPreNode = psNode->psPreNode;
	
	psList->u32NodeCount--;
}

