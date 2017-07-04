/*************************************** Copyright (c)******************************************************
** File name            :   os_sem.c
** Latest modified Date :   
** Latest Version       :   0.1
** Descriptions         :   stupidOS的信号量实现
**
**--------------------------------------------------------------------------------------------------------
** Created by           :   Adrian
** Created date         :   2017-06-24
** Version              :   
** Descriptions         :   The original version
**
**--------------------------------------------------------------------------------------------------------
** Copyright            :   
** Author Blog          :   https://github.com/AdrianGuo
**********************************************************************************************************/
#include "stupidOS.h"
#include "os_sem.h"
/**********************************************************************************************************
** Function name        :   OS_vSemInit
** Descriptions         :   初始化信号量
** parameters           :   u32StartCount 初始的计数
** parameters           :   u32MaxCount 最大计数，如果为0，则不限数量
** Returned value       :   无
***********************************************************************************************************/
void OS_vSemInit(OS_tsSem *psSem, uint32_t u32StartCount, uint32_t u32MaxCount)
{
    OS_vEventInit(&psSem->sEvent, E_OS_EVENT_TYPE_SEM);

    psSem->u32CountMax = u32MaxCount;

    if(u32MaxCount == 0)
    {
        psSem->u32Count = u32StartCount;
    }
    else
    {
        psSem->u32Count = (u32StartCount > u32MaxCount) ? u32MaxCount : u32StartCount;
    }
}

/**********************************************************************************************************
** Function name        :   OS_u32SemWait
** Descriptions         :   等待信号量
** parameters           :   psSem 等待的信号量
** parameters           :   u32WaitTicks 当信号量计数为0时，等待的ticks数，为0时表示永远等待
** Returned value       :   等待结果,E_OS_ERROR_RESOURCE_UNAVALIABLE,E_OS_ERROR_NO_ERROR,E_OS_ERROR_TIME_OUT
***********************************************************************************************************/
uint32_t OS_u32SemWait (OS_tsSem * psSem, uint32_t u32WaitTicks)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();

    // 首先检查信号量计数是否大于0
    if(psSem->u32Count > 0)
    {
        // 如果大于0的话，消耗掉一个，然后正常退出
        --psSem->u32Count;
        OS_vTaskExitCritical(u32Status);
        return E_OS_ERROR_NO_ERROR;
    }
    else
    {
        // 然后将任务插入事件队列中
        OS_vEventWait(&psSem->sEvent, OS_psCurrentTask, (void *)0, E_OS_EVENT_TYPE_SEM, u32WaitTicks);
        OS_vTaskExitCritical(u32Status);

        // 最后再执行一次事件调度，以便于切换到其它任务
        OS_vTaskSched();

        // 当由于等待超时或者计数可用时，执行会返回到这里，然后取出等待结构
        return OS_psCurrentTask->u32WaitEventResult;
    }
}

/**********************************************************************************************************
** Function name        :   OS_u32SemNoWaitGet
** Descriptions         :   获取信号量，如果信号量计数不可用，则立即退回
** parameters           :   psSem 等待的信号量
** Returned value       :   获取结果, E_OS_ERROR_RESOURCE_UNAVALIABLE,E_OS_ERROR_NO_ERROR,E_OS_ERROR_TIME_OUT
***********************************************************************************************************/
uint32_t OS_u32SemNoWaitGet(OS_tsSem * psSem)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();

    // 首先检查信号量计数是否大于0
    if(psSem->u32Count > 0)
    {
        // 如果大于0的话，消耗掉一个，然后正常退出
        --psSem->u32Count;
        OS_vTaskExitCritical(u32Status);
        return E_OS_ERROR_NO_ERROR;
    }
    else
    {
        OS_vTaskExitCritical(u32Status);
        // 当由于等待超时或者计数可用时，执行会返回到这里，然后取出等待结构
        return E_OS_ERROR_RESOURCE_UNAVALIABLE;
    } 
}

/**********************************************************************************************************
** Function name        :   OS_vSemNotify
** Descriptions         :   通知信号量可用，唤醒等待队列中的一个任务，或者将计数+1
** parameters           :   sem 操作的信号量
** Returned value       :   无
***********************************************************************************************************/
void OS_vSemNotify (OS_tsSem * psSem)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();
    if(OS_u32EventWaitCount(&psSem->sEvent) > 0)
    {
        // 如果有的话，则直接唤醒位于队列首部（最先等待）的任务
        OS_tsTask *psTask = OS_psEventWakeUp(&psSem->sEvent, (void *)0, E_OS_ERROR_NO_ERROR);

        // 如果这个任务的优先级更高，就执行调度，切换过去
        if (psTask->u32Prio < OS_psCurrentTask->u32Prio)
        {
            OS_vTaskSched(); 
    	}
    }
    else
    {
        // 如果没有任务等待的话，增加计数
    	++psSem->u32Count;

    	// 如果这个计数超过了最大允许的计数，则递减
    	if ((psSem->u32CountMax != 0) && (psSem->u32Count > psSem->u32CountMax)) 
    	{	
    		psSem->u32Count = psSem->u32CountMax;
    	}
    }
    OS_vTaskExitCritical(u32Status); 
}

/**********************************************************************************************************
** Function name        :   OS_vSemGetInfo
** Descriptions         :   查询信号量的状态信息
** parameters           :   sem 查询的信号量
** parameters           :   info 状态查询存储的位置
** Returned value       :   无
***********************************************************************************************************/
void OS_vSemGetInfo (OS_tsSem * psSem, OS_tsSemInfo *psSemInfo)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();        
    
    // 拷贝需要的信息
    psSemInfo->u32Count = psSem->u32Count;
    psSemInfo->u32CountMax = psSem->u32CountMax;
    psSemInfo->u32TaskCount = OS_u32EventWaitCount(&psSem->sEvent);
    
    OS_vTaskExitCritical(u32Status);
}

/**********************************************************************************************************
** Function name        :   OS_u32SemDestroy
** Descriptions         :   销毁信号量
** parameters           :   psSem 需要销毁的信号量
** Returned value       :   因销毁该信号量而唤醒的任务数量
***********************************************************************************************************/
uint32_t OS_u32SemDestroy (OS_tsSem * psSem)
{       
    uint32_t u32Status = OS_u32TaskEnterCritical();   

    // 清空事件控制块中的任务
    uint32_t u32Count = OS_u32EventRemoveAll(&psSem->sEvent, (void *)0, E_OS_ERROR_DEL);  
    psSem->u32Count = 0;
    OS_vTaskExitCritical(u32Status);
    
    // 清空过程中可能有任务就绪，执行一次调度
    if (u32Count > 0) 
    {
        OS_vTaskSched(); 
    } 
    return u32Count;  
}

