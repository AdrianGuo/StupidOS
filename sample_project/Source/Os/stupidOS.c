/*************************************** Copyright (c)******************************************************
** File name            :   stupidOS.c
** Latest modified Date :   2017-06-24
** Latest Version       :   0.1
** Descriptions         :   stupidOS主文件，包含OS各模块初始化借口
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
#include "ARMCM3.h"


// 当前任务：记录当前是哪个任务正在运行
OS_tsTask * OS_psCurrentTask;

// 下一个将即运行的任务：在进行任务切换前，先设置好该值，然后任务切换过程中会从中读取下一任务信息
OS_tsTask * OS_psNextTask;

// 空闲任务
OS_tsTask * psIdleTask;

// 任务优先级的标记位置结构
OS_tsBitMap sTaskPrioBitmap;

// 所有任务的数组链表
OS_tsList sTaskTableList[STUPIDOS_PRO_COUNT];
// 调度锁计数器
uint8_t u8SchedLockCount;

// 延时队列
OS_tsList sTaskDelayList;


/**********************************************************************************************************
** Function name        :   OS_psTaskHighestReady
** Descriptions         :   获取当前最高优先级且可运行的任务
** parameters           :   无
** Returned value       :   优先级最高的且可运行的任务
***********************************************************************************************************/
OS_tsTask * OS_psTaskHighestReady (void) 
{
    uint32_t u32HighestPrio = OS_u32BitmapGetFirstSet(&sTaskPrioBitmap);
    OS_tsNode *psNode = OS_psListGetFirstNode(&(sTaskTableList[u32HighestPrio]));
    return OS_psNodeParent(psNode, OS_tsTask, sNodeLink);
}









/**********************************************************************************************************
** Function name        :   初始化调度器
** Descriptions         :   无
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskSchedInit (void)
{
	uint8_t i = 0;
    u8SchedLockCount = 0;
    OS_vBitmapInit(&sTaskPrioBitmap);

    for(i = 0; i < STUPIDOS_PRO_COUNT; i++)
    {
        OS_vListInit(&(sTaskTableList[i]));
    }
}

/**********************************************************************************************************
** Function name        :   OS_vTaskSchedDisable
** Descriptions         :   禁止任务调度
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskSchedDisable (void) 
{
    uint32_t u32Status = OS_u32TaskEnterCritical();

    if (u8SchedLockCount < 255) 
    {
        u8SchedLockCount++;
    }

    OS_vTaskExitCritical(u32Status);
}

/**********************************************************************************************************
** Function name        :   OS_vTaskSchedEnable
** Descriptions         :   允许任务调度
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskSchedEnable (void) 
{
    uint32_t u32Status = OS_u32TaskEnterCritical();

    if (u8SchedLockCount > 0) 
    {
        if (--u8SchedLockCount == 0) 
        {
            OS_vTaskSched(); 
        }
    }

    OS_vTaskExitCritical(u32Status);
}

void OS_vTaskSchedRdy(OS_tsTask *psTask) 
{
    OS_vListAddFirst(&(sTaskTableList[psTask->u32Prio]), &(psTask->sNodeLink));
    OS_vBitmapSet(&sTaskPrioBitmap, psTask->u32Prio);
}

void OS_vTaskSchedUnRdy(OS_tsTask *psTask) 
{
    OS_vListRemove(&(sTaskTableList[psTask->u32Prio]), &(psTask->sNodeLink));
    if(OS_u32ListGetCount(&(sTaskTableList[psTask->u32Prio])) == 0)
    {
        OS_vBitmapClear(&sTaskPrioBitmap, psTask->u32Prio);
    }
}

void OS_vTaskSchedRemove(OS_tsTask *psTask) 
{
    OS_vListRemove(&(sTaskTableList[psTask->u32Prio]), &(psTask->sNodeLink));
    if(OS_u32ListGetCount(&(sTaskTableList[psTask->u32Prio])) == 0)
    {
        OS_vBitmapClear(&sTaskPrioBitmap, psTask->u32Prio);
    }
}

/**********************************************************************************************************
** Function name        :   OS_vTaskSched
** Descriptions         :   任务调度接口。tinyOS通过它来选择下一个具体的任务，然后切换至该任务运行。
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskSched(void) 
{   
    OS_tsTask * psTempTask;

    // 进入临界区，以保护在整个任务调度与切换期间，不会因为发生中断导致currentTask和nextTask可能更改
    uint32_t status = OS_u32TaskEnterCritical();

    // 如何调度器已经被上锁，则不进行调度，直接退bm
    if (u8SchedLockCount > 0) 
    {
        OS_vTaskExitCritical(status);
        return;
    }

    // 找到优先级最高的任务，如果其优先级比当前任务的还高，那么就切换到这个任务
    psTempTask = OS_psTaskHighestReady();
    if (psTempTask != OS_psCurrentTask) 
    {
        OS_psNextTask = psTempTask;
        OS_vTaskSwitch();   
    }

    // 退出临界区
    OS_vTaskExitCritical(status); 
}








/**********************************************************************************************************
** Function name        :   OS_vTimeTaskDelayInit
** Descriptions         :   初始化任务延时机制
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTimeTaskDelayInit(void)
{
    OS_vListInit(&sTaskDelayList);
}

/**********************************************************************************************************
** Function name        :   OS_vTimeTaskDelayWait
** Descriptions         :   将任务加入延时队列中
** input parameters     :   task    需要延时的任务
**                          ticks   延时的ticks
** output parameters    :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTimeTaskDelayWait(OS_tsTask *psTask, uint32_t u32DelayTicksPra)
{
    psTask->u32DelayTicks = u32DelayTicksPra;
    OS_vListAddLast(&sTaskDelayList, &(psTask->sNodeDelay));
    psTask->u8State |= STUPIDOS_TASK_STATE_DELAYED; 
}
/**********************************************************************************************************
** Function name        :   OS_vTimeTaskWakeUp
** Descriptions         :   将延时的任务从延时队列中唤醒
** input parameters     :   task  需要唤醒的任务
** output parameters    :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTimeTaskWakeUp(OS_tsTask *psTask)
{
    OS_vListRemove(&sTaskDelayList, &(psTask->sNodeDelay));
    psTask->u8State &= ~STUPIDOS_TASK_STATE_DELAYED;
}

/**********************************************************************************************************
** Function name        :   OS_vTimeTaskWakeUp
** Descriptions         :   将延时的任务从延时队列中移除
** input parameters     :   task  需要唤醒的任务
** output parameters    :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTimeTaskRemove(OS_tsTask *psTask)
{
    OS_vListRemove(&sTaskDelayList, &(psTask->sNodeDelay));
    psTask->u8State &= ~STUPIDOS_TASK_STATE_DELAYED;
}










/**********************************************************************************************************
** Function name        :   OS_vTaskSystemTickHandler
** Descriptions         :   系统时钟节拍处理。
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskSystemTickHandler () 
{
    // 检查所有任务的delayTicks数，如果不0的话，减1。
    OS_tsNode *psNode;  

    uint32_t status = OS_u32TaskEnterCritical();

    for(psNode = sTaskDelayList.sHeadNode.psNextNode; psNode != &(sTaskDelayList.sHeadNode); psNode = psNode->psNextNode)
    {
        OS_tsTask * psTask = OS_psNodeParent(psNode, OS_tsTask, sNodeDelay);
        if (--psTask->u32DelayTicks == 0) 
        {
            // 将任务从延时队列中移除
            OS_vTimeTaskWakeUp(psTask);

            // 将任务恢复到就绪状态
            OS_vTaskSchedRdy(psTask);            
        }
    }

    if(--OS_psCurrentTask->u32Slice == 0)
    {
        if(OS_u32ListGetCount(&(sTaskTableList[OS_psCurrentTask->u32Prio])) > 0)
        {
            OS_psListRemoveFirst(&(sTaskTableList[OS_psCurrentTask->u32Prio]));
            OS_vListAddLast(&(sTaskTableList[OS_psCurrentTask->u32Prio]), &(OS_psCurrentTask->sNodeLink));

            OS_psCurrentTask->u32Slice = STUPIDOS_SLICE_MAX;
        }
    }

    OS_vTaskExitCritical(status);

    // 这个过程中可能有任务延时完毕(delayTicks = 0)，进行一次调度。
    OS_vTaskSched();
}



