#include "os_lib.h"


#define FIRST_NODE sHeadNode.psNextNode
#define LAST_NODE sHeadNode.psPreNode

/**********************************************************************************************************
** Function name        :   OS_vNodeInit
** Descriptions         :   ��ʼ���������
** parameters           :   psNode
** Returned value       :   ��
***********************************************************************************************************/
void OS_vNodeInit(tsNode *psNode)
{
	psNode->psPreNode = psNode;
	psNode->psNextNode = psNode;
}

/**********************************************************************************************************
** Function name        :   OS_vListInit
** Descriptions         :   �����ʼ��
** parameters           :   psList
** Returned value       :   ��
***********************************************************************************************************/
void OS_vListInit(tsList *psList)
{
	psList->FIRST_NODE = &(psList->sHeadNode);
	psList->LAST_NODE = &(psList->sHeadNode);
	psList->u32NodeCount = 0;
}

/**********************************************************************************************************
** Function name        :   OS_u32GetListCount
** Descriptions         :   ���������н�������
** parameters           :   psList
** Returned value       :   �������
***********************************************************************************************************/
uint32_t OS_u32GetListCount(tsList *psList)
{
	return psList->u32NodeCount;
}

/**********************************************************************************************************
** Function name        :   OS_psListGetFirstNode
** Descriptions         :   ����������׸����
** parameters           :   psList ��ѯ������
** Returned value       :   �׸���㣬�������Ϊ�գ��򷵻�0
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
** Descriptions         :   ������������һ�����
** parameters           :   psList ��ѯ������
** Returned value       :   ���Ľ�㣬�������Ϊ�գ��򷵻�0
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
** Descriptions         :   ����������ָ������ǰһ���
** parameters           :   list ��ѯ������
** parameters           :   psNode �ο����
** Returned value       :   ǰһ����㣬������û��ǰ��㣨����Ϊ�գ����򷵻�0
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
** Descriptions         :   ����������ָ�����ĺ�һ���
** parameters           :   list ��ѯ������
** parameters           :   psNode �ο����
** Returned value       :   ��һ����㣬������û��ǰ��㣨����Ϊ�գ����򷵻�0
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
** Descriptions         :   �Ƴ������е����н��
** parameters           :   psList ����յ�����
** Returned value       :   ��
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
** Descriptions         :   ��ָ�������ӵ������ͷ��
** parameters           :   psList ����������
** parameters						:   psNode ������Ľ��
** Returned value       :   ��
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
** Descriptions         :   ��ָ�������ӵ������ĩβ
** parameters           :   psList ����������
** parameters						:   psNode ������Ľ��
** Returned value       :   ��
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
** Descriptions         :   �Ƴ������еĵ�1�����
** parameters           :   psList ���Ƴ�����
** Returned value       :   �������Ϊ�գ�����0������Ļ������ص�1�����
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
** Descriptions         :   �Ƴ����������1�����
** parameters           :   psList ���Ƴ�����
** Returned value       :   �������Ϊ�գ�����0������Ļ��������������
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
** Descriptions         :   ��ָ���Ľ����뵽ĳ��������
** parameters           :   psList 			�����������
** parameters           :   psNodeRef 		�ο����
** parameters           :   psNodeToInsrt 	������Ľṹ
** Returned value       :   ��
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
** Descriptions         :   ��ָ���Ľ����뵽ĳ�����ǰ��
** parameters           :   psList 			�����������
** parameters           :   psNodeRef 		�ο����
** parameters           :   psNodeToInsrt 	������Ľṹ
** Returned value       :   ��
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
** Descriptions         :   ��ָ�������������Ƴ�
** parameters           :   psList 	���Ƴ�������
** parameters           :   psNode 	���Ƴ��Ľ��
** Returned value       :   ��
***********************************************************************************************************/
void OS_vListRemove(tsList *psList, tsNode *psNode)
{
	psNode->psPreNode->psNextNode = psNode->psNextNode;
	psNode->psNextNode->psPreNode = psNode->psPreNode;
	
	psList->u32NodeCount--;
}

