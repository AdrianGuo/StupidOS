/*************************************** Copyright (c)******************************************************
** File name            :   os_sem.h
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   stupidOS的信号量实现
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
#ifndef OS_SEM_H
#define OS_SEM_H

#include "os_config.h"
#include "os_event.h"

typedef struct OS_sSem{
	// 事件控制块
	// 该结构被特意放到起始处，以实现tSem同时是一个tEvent的目的
	OS_tsEventControl sEvent;
	// 当前的计数
	uint32_t u32Count;
	// 最大计数
	uint32_t u32CountMax;
}OS_tsSem;

typedef struct OS_sSemInfo{
	// 当前信号量的计数
    uint32_t u32Count;
    // 信号量允许的最大计数
    uint32_t u32CountMax;

    // 当前等待的任务计数
    uint32_t u32TaskCount;
}OS_tsSemInfo;

/**********************************************************************************************************
** Function name        :   OS_vSemInit
** Descriptions         :   初始化信号量
** parameters           :   u32StartCount 初始的计数
** parameters           :   u32MaxCount 最大计数，如果为0，则不限数量
** Returned value       :   无
***********************************************************************************************************/
void OS_vSemInit(OS_tsSem *psSem, uint32_t u32StartCount, uint32_t u32MaxCount);

/**********************************************************************************************************
** Function name        :   OS_u32SemWait
** Descriptions         :   等待信号量
** parameters           :   psSem 等待的信号量
** parameters           :   u32WaitTicks 当信号量计数为0时，等待的ticks数，为0时表示永远等待
** Returned value       :   等待结果,E_OS_ERROR_RESOURCE_UNAVALIABLE,E_OS_ERROR_NO_ERROR,E_OS_ERROR_TIME_OUT
***********************************************************************************************************/
uint32_t OS_u32SemWait (OS_tsSem * psSem, uint32_t u32WaitTicks);

/**********************************************************************************************************
** Function name        :   OS_u32SemNoWaitGet
** Descriptions         :   获取信号量，如果信号量计数不可用，则立即退回
** parameters           :   psSem 等待的信号量
** Returned value       :   获取结果, E_OS_ERROR_RESOURCE_UNAVALIABLE,E_OS_ERROR_NO_ERROR,E_OS_ERROR_TIME_OUT
***********************************************************************************************************/
uint32_t OS_u32SemNoWaitGet(OS_tsSem * psSem);

/**********************************************************************************************************
** Function name        :   OS_vSemNotify
** Descriptions         :   通知信号量可用，唤醒等待队列中的一个任务，或者将计数+1
** parameters           :   sem 操作的信号量
** Returned value       :   无
***********************************************************************************************************/
void OS_vSemNotify (OS_tsSem * psSem);

/**********************************************************************************************************
** Function name        :   OS_vSemGetInfo
** Descriptions         :   查询信号量的状态信息
** parameters           :   sem 查询的信号量
** parameters           :   info 状态查询存储的位置
** Returned value       :   无
***********************************************************************************************************/
void OS_vSemGetInfo (OS_tsSem * psSem, OS_tsSemInfo *psSemInfo);

/**********************************************************************************************************
** Function name        :   OS_u32SemDestroy
** Descriptions         :   销毁信号量
** parameters           :   psSem 需要销毁的信号量
** Returned value       :   因销毁该信号量而唤醒的任务数量
***********************************************************************************************************/
uint32_t OS_u32SemDestroy (OS_tsSem * psSem);

#endif

