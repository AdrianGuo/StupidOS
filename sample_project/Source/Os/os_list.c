/*************************************** Copyright (c)******************************************************
** File name            :   os_list.c
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   tinyOS���õ�˫���������ݽṹ��
**
**--------------------------------------------------------------------------------------------------------
** Created by           :   Adrian
** Created date         :   2017-06-24
** Version              :   0.1
** Descriptions         :   The original version
**
**--------------------------------------------------------------------------------------------------------
** Copyright            :   
** Author Blog          :   https://github.com/AdrianGuo
**********************************************************************************************************/
#include "os_lib.h"


#define FIRST_NODE sHeadNode.psNextNode
#define LAST_NODE sHeadNode.psPreNode

/**********************************************************************************************************
** Function name        :   OS_vNodeInit
** Descriptions         :   ��ʼ���������
** parameters           :   psNode
** Returned value       :   ��
***********************************************************************************************************/
void OS_vNodeInit(OS_tsNode *psNode)
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
void OS_vListInit(OS_tsList *psList)
{
	psList->FIRST_NODE = &(psList->sHeadNode);
	psList->LAST_NODE = &(psList->sHeadNode);
	psList->u32NodeCount = 0;
}

/**********************************************************************************************************
** Function name        :   OS_u32ListGetCount
** Descriptions         :   ���������н�������
** parameters           :   psList
** Returned value       :   �������
***********************************************************************************************************/
uint32_t OS_u32ListGetCount(OS_tsList *psList)
{
	return psList->u32NodeCount;
}

/**********************************************************************************************************
** Function name        :   OS_psListGetFirstNode
** Descriptions         :   ����������׸����
** parameters           :   psList ��ѯ������
** Returned value       :   �׸���㣬�������Ϊ�գ��򷵻�0
***********************************************************************************************************/
OS_tsNode * OS_psListGetFirstNode(OS_tsList *psList)
{
	OS_tsNode *psNode = (OS_tsNode * )0;
	
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
OS_tsNode * OS_psListGetLastNode(OS_tsList *psList)
{
	OS_tsNode *psNode = (OS_tsNode *)0;
	
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
OS_tsNode * OS_psListGetNodePreNode(OS_tsNode *psNode)
{
	if(psNode->psPreNode == psNode)
	{
		return (OS_tsNode *)0;
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
OS_tsNode * OS_psListGetNodeNextNode(OS_tsNode *psNode)
{
	if(psNode->psNextNode == psNode)
	{
		return (OS_tsNode *)0;
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
void OS_vListRemoveAll(OS_tsList *psList)
{
	uint32_t u32Count = 0;
	
	OS_tsNode *psNextNode;

	psNextNode = psList->FIRST_NODE;
	
	for(u32Count = psList->u32NodeCount; u32Count > 0; u32Count--)
	{
		OS_tsNode *psCurrentNode = psNextNode;
		
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
void OS_vListAddFirst(OS_tsList *psList, OS_tsNode *psNode)
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
void OS_vListAddLast(OS_tsList *psList, OS_tsNode *psNode)
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
OS_tsNode * OS_psListRemoveFirst(OS_tsList *psList)
{
	OS_tsNode *psNode = (OS_tsNode *)0;
	
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
OS_tsNode * OS_psListRemoveLast(OS_tsList *psList)
{
	OS_tsNode *psNode = (OS_tsNode *)0;
	
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
void OS_vListInsertAfter(OS_tsList *psList, OS_tsNode *psNodeRef, OS_tsNode *psNodeToInsrt)
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
void OS_vListInsertBefore(OS_tsList *psList, OS_tsNode *psNodeRef, OS_tsNode *psNodeToInsrt)
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
void OS_vListRemove(OS_tsList *psList, OS_tsNode *psNode)
{
	psNode->psPreNode->psNextNode = psNode->psNextNode;
	psNode->psNextNode->psPreNode = psNode->psPreNode;
	
	psList->u32NodeCount--;
}

