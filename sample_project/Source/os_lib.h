/*************************************** Copyright (c)******************************************************
** File name            :   os_lib.h
** Latest modified Date :   2016-06-01
** Latest Version       :   0.1
** Descriptions         :   tinyOS所用的通用数据结构库文件。
**
**--------------------------------------------------------------------------------------------------------
** Created by           :   01课堂 lishutong
** Created date         :   2016-06-01
** Version              :   1.0
** Descriptions         :   The original version
**
**--------------------------------------------------------------------------------------------------------
** Copyright            :   版权所有，禁止用于商业用途
** Author Blog          :   http://ilishutong.com
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
typedef struct tNode{
	// 该结点的前一个结点
	struct tNode *psPreNode;
	// 该结点的后一个结点
	struct tNode *psNextNode;
}tsNode;

// StupidOS链表类型
typedef struct tList{
	//该链表的头结点
	tsNode sHeadNode;
	//该链表的节点数量
	uint32_t u32NodeCount;
}tsList;

/**********************************************************************************************************
** Function name        :   OS_vNodeInit
** Descriptions         :   初始化结点类型
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vNodeInit(tsNode *psNode);

/**********************************************************************************************************
** Function name        :   OS_tNodeParent
** Descriptions         :   获取结点所在的父struct结构首地址
** parameters           :   无
** Returned value       :   父struct结构首地址
***********************************************************************************************************/
#define OS_tNodeParent(node, parent, name) (parent *)((uint32_t)node - (uint32_t)&((parent *)0)->name)

void OS_vListInit(tsList *psList);

uint32_t OS_u32GetListCount(tsList *psList);

tsNode * OS_psListGetFirstNode(tsList *psList);

tsNode * OS_psListGetLastNode(tsList *psList);

tsNode * OS_psListGetNodePreNode(tsNode *psNode);

tsNode * OS_psListGetNodeNextNode(tsNode *psNode);

void OS_vListRemoveAll(tsList *psList);

void OS_vListAddFirst(tsList *psList, tsNode *psNode);

void OS_vListAddLast(tsList *psList, tsNode *psNode);

tsNode * OS_psListRemoveFirst(tsList *psList);

tsNode * OS_psListRemoveLast(tsList *psList);

void OS_vListInsertAfter(tsList *psList, tsNode *psNodeRef, tsNode *psNodeToInsrt);

void OS_vListInsertBefore(tsList *psList, tsNode *psNodeRef, tsNode *psNodeToInsrt);

void OS_vListRemove(tsList *psList, tsNode *psNode);



#endif /* OS_LIB_H */
