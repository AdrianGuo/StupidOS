/*************************************** Copyright (c)******************************************************
** File name            :   main.c
** Latest modified Date :   2016-06-01
** Latest Version       :   0.1
** Descriptions         :   主文件，包含应用代码
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
	OS_vListAddLast(&(sTaskTableList[u32Prio]), &(psTask->sNodeLink));
	
	psTask->u32DelayTicks = 0;
	OS_vNodeInit(&(psTask->sNodeDelay));

    OS_vBitmapSet(&sTaskPrioBitmap, u32Prio);                  // 标记优先级位置中的相应位
	
    psTask->u8State |=  STUPIDOS_TASK_STATE_RDY;
}

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

/**********************************************************************************************************
** Function name        :   OS_vTaskDelay
** Descriptions         :   使当前任务进入延时状态。
** parameters           :   delay 延时多少个ticks
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskDelay (uint32_t u32Delay) {
    // 进入临界区，以保护在整个任务调度与切换期间，不会因为发生中断导致currentTask和nextTask可能更改
    uint32_t u32Status = OS_u32TaskEnterCritical();
    // 设置延时值，插入延时队列
    OS_vTimeTaskDelayWait(OS_psCurrentTask, u32Delay);
    // 将任务从就绪表中移除
    OS_vTaskSchedUnRdy(OS_psCurrentTask);

    // 然后进行任务切换，切换至另一个任务，或者空闲任务
    // delayTikcs会在时钟中断中自动减1.当减至0时，会切换回来继续运行。
    OS_vTaskSched();

    OS_vTaskExitCritical(u32Status);
}

/*********************************************************************************************************
** 系统时钟节拍定时器System Tick配置
** 在我们目前的环境（模拟器）中，系统时钟节拍为12MHz
** 请务必按照本教程推荐配置，否则systemTick的值就会有变化，需要查看数据手册才了解
**********************************************************************************************************/
void OS_vSetSysTickPeriod(uint32_t ms)
{
  SysTick->LOAD  = ms * SystemCoreClock / 1000 - 1; 
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
  SysTick->VAL   = 0;                           
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk; 
}

/**********************************************************************************************************
** Function name        :   SysTick_Handler
** Descriptions         :   SystemTick的中断处理函数。
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void SysTick_Handler () 
{
    OS_vTaskSystemTickHandler();
}
