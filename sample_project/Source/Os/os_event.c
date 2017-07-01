/*************************************** Copyright (c)******************************************************
** File name            :   os_event.c
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
#include "stupidOS.h"



void OS_vEventInit (OS_tsEventControl *psEventControl, OS_teEventType eEventType)
{
    psEventControl->eType = eEventType;
    OS_vListInit(&(psEventControl->sWaitList));
}


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
void OS_vEventWait (OS_tsEventControl *psEvent, OS_tsTask *psTask, void *pvMsg, uint32_t u32State, uint32_t u32Timeout)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();
    psTask->psWaitEvent = psEvent;
    psTask->u32State |= u32State;
    psTask->pvEventMsg = pvMsg;
    psTask->u32WaitEventResult = E_OS_ERROR_NO_ERROR;
    
    // 将任务从就绪队列中移除
    OS_vTaskSchedUnRdy(psTask);
    // 将任务插入到等待队列中
    OS_vListAddLast(&(psEvent->sWaitList), &(psTask->sNodeLink));
    // 如果发现有设置超时，在同时插入到延时队列中
    // 当时间到达时，由延时处理机制负责将任务从延时列表中移除，同时从事件列表中移除
    if(u32Timeout)
    {
        OS_vTimeTaskDelayWait(psTask, u32Timeout);
    }
    //为什么不调度来着？？如果是自己就调度
    OS_vTaskExitCritical(u32Status);
}

/**********************************************************************************************************
** Function name        :   OS_psEventWakeUp
** Descriptions         :   从事件控制块中唤醒首个等待的任务
** parameters           :   psEvent 事件控制块
** parameters           :   pvMsg 事件消息
** parameters           :   u32Result 告知事件的等待结果
** Returned value       :   首个等待的任务，如果没有任务等待，则返回0
***********************************************************************************************************/
OS_tsTask * OS_psEventWakeUp (OS_tsEventControl * psEvent, void * pvMsg, uint32_t u32Result)
{
    OS_tsTask *psTask = (OS_tsTask *)0;
    OS_tsNode *psNode;
    uint32_t u32Status = OS_u32TaskEnterCritical();
   
    if((psNode = OS_psListRemoveFirst(&psEvent->sWaitList)) != (OS_tsNode *)0)
    {
        psTask = OS_psNodeParent(psNode, OS_tsTask, sNodeLink);
        psTask->psWaitEvent = (OS_tsEventControl *)0;
        psTask->pvEventMsg = pvMsg;
        psTask->u32WaitEventResult = u32Result;
        psTask->u32State &= ~STUPIDOS_TASK_WAIT_MASK;

        // 任务申请了超时等待，这里检查下，将其从延时队列中移除
        if(psTask->u32DelayTicks != 0)
        {
            OS_vTimeTaskWakeUp(psTask);
        }

        // 将任务加入就绪队列
        OS_vTaskSchedRdy(psTask);    
    }

    //为什么不调度来着？？
    OS_vTaskExitCritical(u32Status);
    return psTask;
}

/**********************************************************************************************************
** Function name        :   OS_vEventRemoveTask
** Descriptions         :   将任务从其等待队列中强制移除
** parameters           :   psTask 待移除的任务
** parameters           :   u32Result 告知事件的等待结果
** Returned value       :   无
***********************************************************************************************************/
void OS_vEventRemoveTask (OS_tsTask * psTask, void * pvMsg, uint32_t u32Result)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();

    OS_vListRemove(&psTask->psWaitEvent->sWaitList, &psTask->sNodeLink);

    psTask->psWaitEvent = (OS_tsEventControl *)0;
    psTask->pvEventMsg = pvMsg;
    psTask->u32WaitEventResult = u32Result;
    psTask->u32State &= ~STUPIDOS_TASK_WAIT_MASK;
    
    //为什么不调度来着？？此函数由OS_vTaskSystemTickHandler直接调用，之前已做延时检查，之后会做调度
    OS_vTaskExitCritical(u32Status);
}
