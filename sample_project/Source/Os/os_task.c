/*************************************** Copyright (c)******************************************************
** File name            :   os_task.c
** Latest modified Date :   
** Latest Version       :   0.1
** Descriptions         :   stupidOS的任务管理接口实现
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

/**********************************************************************************************************
** Function name        :   OS_vTaskInit
** Descriptions         :   初始化任务结构
** parameters           :   task        要初始化的任务结构
** parameters           :   entry       任务的入口函数
** parameters           :   param       传递给任务的运行参数
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskInit (OS_tsTask * psTask, void (*pvEntry)(void *), void *pvParam, uint32_t u32Prio, uint32_t * pu32Stack)
{
    // 为了简化代码，tinyOS无论是在启动时切换至第一个任务，还是在运行过程中在不同间任务切换
    // 所执行的操作都是先保存当前任务的运行环境参数（CPU寄存器值）的堆栈中(如果已经运行运行起来的话)，然后再
    // 取出从下一个任务的堆栈中取出之前的运行环境参数，然后恢复到CPU寄存器
    // 对于切换至之前从没有运行过的任务，我们为它配置一个“虚假的”保存现场，然后使用该现场恢复。

    // 注意以下两点：
    // 1、不需要用到的寄存器，直接填了寄存器号，方便在IDE调试时查看效果；
    // 2、顺序不能变，要结合PendSV_Handler以及CPU对异常的处理流程来理解
    *(--pu32Stack) = (unsigned long)(1<<24);                // XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
    *(--pu32Stack) = (unsigned long)pvEntry;                // 程序的入口地址
    *(--pu32Stack) = (unsigned long)0x14;                   // R14(LR), 任务不会通过return xxx结束自己，所以未用
    *(--pu32Stack) = (unsigned long)0x12;                   // R12, 未用
    *(--pu32Stack) = (unsigned long)0x3;                    // R3, 未用
    *(--pu32Stack) = (unsigned long)0x2;                    // R2, 未用
    *(--pu32Stack) = (unsigned long)0x1;                    // R1, 未用
    *(--pu32Stack) = (unsigned long)pvParam;                // R0 = pvParam, 传给任务的入口函数
    *(--pu32Stack) = (unsigned long)0x11;                   // R11, 未用
    *(--pu32Stack) = (unsigned long)0x10;                   // R10, 未用
    *(--pu32Stack) = (unsigned long)0x9;                    // R9, 未用
    *(--pu32Stack) = (unsigned long)0x8;                    // R8, 未用
    *(--pu32Stack) = (unsigned long)0x7;                    // R7, 未用
    *(--pu32Stack) = (unsigned long)0x6;                    // R6, 未用
    *(--pu32Stack) = (unsigned long)0x5;                    // R5, 未用
    *(--pu32Stack) = (unsigned long)0x4;                    // R4, 未用

    psTask->psStack = pu32Stack;                                // 保存最终的值

    psTask->u32Prio = u32Prio;   								// 设置任务的优先级     
    psTask->u32Slice = STUPIDOS_SLICE_MAX;
                              
	OS_vNodeInit(&(psTask->sNodeLink));
	
	psTask->u32DelayTicks = 0;
	OS_vNodeInit(&(psTask->sNodeDelay));
	
	psTask->u32SuspendCount = 0;

    psTask->OS_vClean = (void(*)(void *))0; 
    psTask->pvCleanParam = (void *)0;   
	psTask->u8RequestDeleteFlag = 0;

	OS_vTaskSchedRdy(psTask);
	
    psTask->u32State |=  STUPIDOS_TASK_STATE_RDY;
}


/**********************************************************************************************************
** Function name        :   OS_vTaskSuspend
** Descriptions         :   挂起指定的任务
** parameters           :   task        待挂起的任务
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskSuspend (OS_tsTask * psTask) 
{
    uint32_t u32Status = OS_u32TaskEnterCritical();

    // 不允许对已经进入延时状态的任务挂起
    if (!(psTask->u32State & STUPIDOS_TASK_STATE_DELAYED)) 
    {
        // 增加挂起计数，仅当该任务被执行第一次挂起操作时，才考虑是否
        // 要执行任务切换操作
        if (++psTask->u32SuspendCount <= 1)
        {
            // 设置挂起标志
            psTask->u32State |= STUPIDOS_TASK_STATE_SUSPEND;

            // 挂起方式很简单，就是将其从就绪队列中移除，这样调度器就不会发现他
            // 也就没法切换到该任务运行
            OS_vTaskSchedUnRdy(psTask);

            // 当然，这个任务可能是自己，那么就切换到其它任务
            if (psTask == OS_psCurrentTask)
            {
                OS_vTaskSched();
            }
        }
    }

    OS_vTaskExitCritical(u32Status);
}


/**********************************************************************************************************
** Function name        :   OS_vTaskWakeUp
** Descriptions         :   唤醒被挂起的任务
** parameters           :   task        待唤醒的任务
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskWakeUp (OS_tsTask * psTask)
{
    // 进入临界区
    uint32_t u32Status = OS_u32TaskEnterCritical();

     // 检查任务是否处于挂起状态
    if (psTask->u32State & STUPIDOS_TASK_STATE_SUSPEND)
    {
        // 递减挂起计数，如果为0了，则清除挂起标志，同时设置进入就绪状态
        if (--psTask->u32SuspendCount == 0) 
        {
            // 清除挂起标志
            psTask->u32State &= ~STUPIDOS_TASK_STATE_SUSPEND;

            // 同时将任务放回就绪队列中
            OS_vTaskSchedRdy(psTask);

            // 唤醒过程中，可能有更高优先级的任务就绪，执行一次任务调度
            OS_vTaskSched();
        }
    }

    // 退出临界区
    OS_vTaskExitCritical(u32Status);
}

/**********************************************************************************************************
** Function name        :   OS_vTaskSetCleanCallFunc
** Descriptions         :   设置任务被删除时调用的清理函数
** parameters           :   task  待设置的任务
** parameters           :   clean  清理函数入口地址
** parameters           :   param  传递给清理函数的参数
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskSetCleanCallFunc (OS_tsTask * psTask, void (*OS_vCleanParam)(void * pvParamParam), void * pvParamParam) 
{
    psTask->OS_vClean = OS_vCleanParam;
    psTask->pvCleanParam = pvParamParam;
}

/**********************************************************************************************************
** Function name        :   OS_vTaskForceDelete
** Descriptions         :   强制删除指定的任务
** parameters           :   task  需要删除的任务
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskForceDelete (OS_tsTask * psTask) 
{
    // 进入临界区
    uint32_t u32Status = OS_u32TaskEnterCritical();

    if(psTask->u32State & STUPIDOS_TASK_STATE_DELAYED)
    {
        OS_vTimeTaskRemove(psTask);
    }
    else if(!(psTask->u32State & STUPIDOS_TASK_STATE_SUSPEND))
    {
        OS_vTaskSchedRemove(psTask);
    }

    if(psTask->OS_vClean)
    {
        psTask->OS_vClean(psTask->pvCleanParam);
    }

    if(OS_psCurrentTask == psTask)
    {
        OS_vTaskSched();
    }
    // 退出临界区
    OS_vTaskExitCritical(u32Status);
}

/**********************************************************************************************************
** Function name        :   OS_vTaskRequestDelete
** Descriptions         :   标记任务需被删除
** parameters           :   task  需要删除的任务
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskRequestDelete (OS_tsTask * psTask) 
{
    uint32_t u32Status = OS_u32TaskEnterCritical();
    psTask->u8RequestDeleteFlag = 1;
    OS_vTaskExitCritical(u32Status);
}

/**********************************************************************************************************
** Function name        :   OS_u8TaskIsRequestedDeleted
** Descriptions         :   检查任务是否已删除
** parameters           :   task  需要删除的任务
** Returned value       :   无
***********************************************************************************************************/
uint8_t OS_u8TaskIsRequestedDeleted (void) 
{
    uint8_t u8State;

    uint32_t u32Status = OS_u32TaskEnterCritical();
    u8State = OS_psCurrentTask->u8RequestDeleteFlag;
    OS_vTaskExitCritical(u32Status);

    return u8State;
}


/**********************************************************************************************************
** Function name        :   OS_vTaskDeleateItSelf
** Descriptions         :   任务删除自身
** parameters           :   task  需要删除的任务
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskDeleteItSelf (void) 
{
    uint32_t u32Status = OS_u32TaskEnterCritical();
    // 任务在调用该函数时，必须是处于就绪状态，不可能处于延时或挂起等其它状态
    // 所以，只需要从就绪队列中移除即可
    OS_vTaskSchedRemove(OS_psCurrentTask);
    if(OS_psCurrentTask->OS_vClean)
    {
        OS_psCurrentTask->OS_vClean(OS_psCurrentTask->pvCleanParam);
    }

    // 接下来，肯定是切换到其它任务去运行
    OS_vTaskSched();
    
    OS_vTaskExitCritical(u32Status);
}

/**********************************************************************************************************
** Function name        :   OS_vTaskGetInfo
** Descriptions         :   获取任务状态信息
** parameters           :   task  需要删除的任务
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskGetInfo(OS_tsTask *psTask, OS_tsTaskInfo *psTaskInfo)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();
    psTaskInfo->u32State = psTask->u32State;
    psTaskInfo->u32Prio = psTask->u32Prio;
    psTaskInfo->u32DelayTicks = psTask->u32DelayTicks;
    psTaskInfo->u32SuspendCount = psTask->u32SuspendCount;
    psTaskInfo->u32Slice = psTask->u32Slice;
    OS_vTaskExitCritical(u32Status);
}
