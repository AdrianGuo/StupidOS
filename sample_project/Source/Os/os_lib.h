/*************************************** Copyright (c)******************************************************
** File name            :   os_lib.h
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   stupidOS所用的通用数据结构库文件。
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
#ifndef OS_LIB_H
#define OS_LIB_H

// 标准头文件，里面包含了常用的类型定义，如uint32_t
#include <stdint.h>

// 位图类型
typedef struct 
{
	uint32_t u32bitmap;
}OS_tsBitMap;

/**********************************************************************************************************
** Function name        :   OS_vBitmapInit
** Descriptions         :   初始化bitmap将所有的位全清0
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vBitmapInit (OS_tsBitMap * psBitMap);

/**********************************************************************************************************
** Function name        :   OS_u32BitmapPosCount
** Descriptions         :   返回最大支持的位置数量
** parameters           :   无
** Returned value       :   最大支持的位置数量
***********************************************************************************************************/
uint32_t OS_u32BitmapPosCount (void);

/**********************************************************************************************************
** Function name        :   OS_vBitmapSet
** Descriptions         :   设置bitmap中的某个位
** parameters           :   pos 需要设置的位
** Returned value       :   无
***********************************************************************************************************/
void OS_vBitmapSet (OS_tsBitMap * psBitMap, uint32_t pos);

/**********************************************************************************************************
** Function name        :   OS_vBitmapClear
** Descriptions         :   清除bitmap中的某个位
** parameters           :   pos 需要清除的位
** Returned value       :   无
***********************************************************************************************************/
void OS_vBitmapClear (OS_tsBitMap * psBitMap, uint32_t pos);

/**********************************************************************************************************
** Function name        :   OS_u32BitmapGetFirstSet
** Descriptions         :   从位图中第0位开始查找，找到第1个被设置的位置序号
** parameters           :   无
** Returned value       :   第1个被设置的位序号
***********************************************************************************************************/
uint32_t OS_u32BitmapGetFirstSet (OS_tsBitMap * psBitMap);



// StupidOS链表的结点类型
typedef struct OS_Node{
	// 该结点的前一个结点
	struct OS_Node *psPreNode;
	// 该结点的后一个结点
	struct OS_Node *psNextNode;
}OS_tsNode;

// StupidOS链表类型
typedef struct OS_List{
	//该链表的头结点
	OS_tsNode sHeadNode;
	//该链表的节点数量
	uint32_t u32NodeCount;
}OS_tsList;

/**********************************************************************************************************
** Function name        :   OS_vNodeInit
** Descriptions         :   初始化结点类型
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vNodeInit(OS_tsNode *psNode);

/**********************************************************************************************************
** Function name        :   OS_psNodeParent
** Descriptions         :   获取结点所在的父struct结构首地址
** parameters           :   无
** Returned value       :   父struct结构首地址
***********************************************************************************************************/
#define OS_psNodeParent(NodeAddr, ParentType, NodeName) (ParentType *)((uint32_t)NodeAddr - (uint32_t)&((ParentType *)0)->NodeName)

void OS_vListInit(OS_tsList *psList);

uint32_t OS_u32ListGetCount(OS_tsList *psList);

OS_tsNode * OS_psListGetFirstNode(OS_tsList *psList);

OS_tsNode * OS_psListGetLastNode(OS_tsList *psList);

OS_tsNode * OS_psListGetNodePreNode(OS_tsNode *psNode);

OS_tsNode * OS_psListGetNodeNextNode(OS_tsNode *psNode);

void OS_vListRemoveAll(OS_tsList *psList);

void OS_vListAddFirst(OS_tsList *psList, OS_tsNode *psNode);

void OS_vListAddLast(OS_tsList *psList, OS_tsNode *psNode);

OS_tsNode * OS_psListRemoveFirst(OS_tsList *psList);

OS_tsNode * OS_psListRemoveLast(OS_tsList *psList);

void OS_vListInsertAfter(OS_tsList *psList, OS_tsNode *psNodeRef, OS_tsNode *psNodeToInsrt);

void OS_vListInsertBefore(OS_tsList *psList, OS_tsNode *psNodeRef, OS_tsNode *psNodeToInsrt);

void OS_vListRemove(OS_tsList *psList, OS_tsNode *psNode);



#endif /* OS_LIB_H */
