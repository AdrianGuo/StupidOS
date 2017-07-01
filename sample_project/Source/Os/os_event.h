/*************************************** Copyright (c)******************************************************
** File name            :   os_event.h
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   
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
#ifndef OS_EVENT_H
#define OS_EVENT_H

#include "os_config.h"
#include "os_lib.h"

typedef enum OS_eEventType{
    E_OS_EVENT_TYPE_UNKNOWN = 0, 				// 未知类型
}OS_teEventType;

// Event控制结构
typedef struct OS_sEventControl {
    OS_teEventType eType;						// Event类型

    OS_tsList sWaitList;						// 任务等待列表
}OS_tsEventControl;

/**********************************************************************************************************
** Function name        :   OS_vEventInit
** Descriptions         :   初始化事件控制块
** parameters           :   psEventControl 事件控制块
** parameters           :   eEventType 事件控制块的类型
** Returned value       :   无
***********************************************************************************************************/
void OS_vEventInit (OS_tsEventControl *psEventControl, OS_teEventType eEventType);

/**********************************************************************************************************
** Function name        :   OS_vEventWait
** Descriptions         :   让指定在事件控制块上等待事件发生
** parameters           :   psEvent 事件控制块
** parameters           :   psTask 等待事件发生的任务
** parameters           :   pvMsg 事件消息存储的具体位置
** parameters           :   u32State 消息类型
** parameters           :   u32Timeout 等待多长时间
** Returned value       :   优先级最高的且可运行的任务
***********************************************************************************************************/
void OS_vEventWait (OS_tsEventControl *psEvent, OS_tsTask *psTask, void *pvMsg, uint32_t u32State, uint32_t u32Timeout);

/**********************************************************************************************************
** Function name        :   OS_psEventWakeUp
** Descriptions         :   从事件控制块中唤醒首个等待的任务
** parameters           :   psEvent 事件控制块
** parameters           :   pvMsg 事件消息
** parameters           :   u32Result 告知事件的等待结果
** Returned value       :   首个等待的任务，如果没有任务等待，则返回0
***********************************************************************************************************/
OS_tsTask * OS_psEventWakeUp (OS_tsEventControl * psEvent, void * pvMsg, uint32_t u32Result);

/**********************************************************************************************************
** Function name        :   OS_vEventRemoveTask
** Descriptions         :   将任务从其等待队列中强制移除
** parameters           :   psTask 待移除的任务
** parameters           :   u32Result 告知事件的等待结果
** Returned value       :   无
***********************************************************************************************************/
void OS_vEventRemoveTask (OS_tsTask * psTask, void * pvMsg, uint32_t u32Result);

#endif
