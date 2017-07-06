/*************************************** Copyright (c)******************************************************
** File name            :   os_mbox.h
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   stupidOS的邮箱实现
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
#ifndef OS_MBOX_H
#define OS_MBOX_H

#include "os_config.h"
#include "os_event.h"

#define OS_MBOX_SEND_NORMAL                      0x00        // 正常发送发送至缓冲区
#define OS_MBOX_SEND_FRONT                0x01        // 消息发送至缓冲区头部

// 邮箱类型
typedef struct OS_sMbox
{
	// 事件控制块
	// 该结构被特意放到起始处，以实现tSem同时是一个tEvent的目的
	OS_tsEventControl sEvent;

	// 当前的消息数量
    uint32_t u32Count;

    // 读取消息的索引
    uint32_t u32Read;

    // 写消息的索引
    uint32_t u32Write;

    // 最大允许容纳的消息数量
    uint32_t u32MaxCount;

    // 消息存储缓冲区，即一个void *构成的数组
    void * *ppvMsgBuffer;
}OS_tsMbox;

typedef struct OS_sMboxInfo{
    // 当前的消息数量
    uint32_t u32Count;
    
    // 最大允许容纳的消息数量
    uint32_t u32MaxCount;

    // 当前等待的任务计数
    uint32_t u32TaskCount;
}OS_tsMboxInfo;

/**********************************************************************************************************
** Function name        :   OS_vMboxInit
** Descriptions         :   初始化邮箱
** parameters           :   psMbox 等待初始化的邮箱
** parameters           :   ppvMsgBuffer 消息存储缓冲区
** parameters           :   u32MaxCount 最大计数
** Returned value       :   无
***********************************************************************************************************/
void OS_vMboxInit (OS_tsMbox * psMbox, void ** ppvMsgBuffer, uint32_t u32MaxCount);

/**********************************************************************************************************
** Function name        :   OS_u32MboxWait
** Descriptions         :   等待邮箱, 获取一则消息
** parameters           :   psMbox 等待的邮箱
** parameters           :   ppvMsgBuffermsg 消息存储缓存区
** parameters           :   u32WaitTicks 最大等待的ticks数，为0表示无限等待
** Returned value       :   等待结果,E_OS_ERROR_NO_ERROR,E_OS_ERROR_TIME_OUT,E_OS_ERROR_RESOURCE_UNAVALIABLE
***********************************************************************************************************/
uint32_t OS_u32MboxWait (OS_tsMbox * psMbox, void **ppvMsgBuffermsg, uint32_t u32WaitTicks);

/**********************************************************************************************************
** Function name        :   OS_u32MboxNoWaitGet
** Descriptions         :   获取一则消息，如果没有消息，则立即退回
** parameters           :   psMbox 获取消息的邮箱
** parameters           :   ppvMsgBuffermsg 消息存储缓存区
** Returned value       :   获取结果, E_OS_ERROR_RESOURCE_UNAVALIABLE，E_OS_ERROR_NO_ERROR
***********************************************************************************************************/
uint32_t OS_u32MboxNoWaitGet (OS_tsMbox * psMbox, void **ppvMsgBuffermsg);

/**********************************************************************************************************
** Function name        :   OS_u32MboxNotify
** Descriptions         :   通知消息可用，唤醒等待队列中的一个任务，或者将消息插入到邮箱中
** parameters           :   psMbox 操作的信号量
** parameters           :   pvMsgBuffermsg 发送的消息
** parameters           :   u32NotifyOption 发送的选项
** Returned value       :   tErrorResourceFull
***********************************************************************************************************/
uint32_t OS_u32MboxNotify (OS_tsMbox * psMbox, void * pvMsgBuffermsg, uint32_t u32NotifyOption);

/**********************************************************************************************************
** Function name        :   OS_vMboxFlush
** Descriptions         :   清空邮箱中所有消息
** parameters           :   psMbox 等待清空的邮箱
** Returned value       :   无
***********************************************************************************************************/
void OS_vMboxFlush (OS_tsMbox * psMbox);

/**********************************************************************************************************
** Function name        :   OS_u32MboxDestroy
** Descriptions         :   销毁邮箱
** parameters           :   psMbox 需要销毁的邮箱
** Returned value       :   因销毁该信号量而唤醒的任务数量
***********************************************************************************************************/
uint32_t OS_u32MboxDestroy (OS_tsMbox * psMbox);

/**********************************************************************************************************
** Function name        :   OS_vMboxGetInfo
** Descriptions         :   查询状态信息
** parameters           :   mbox 查询的邮箱
** parameters           :   info 状态查询存储的位置
** Returned value       :   无
***********************************************************************************************************/
void OS_vMboxGetInfo (OS_tsMbox * psMbox, OS_tsMboxInfo * psMboxInfo);

#endif

