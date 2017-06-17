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

// 所有任务的指针数组：简单起见，只使用两个任务
OS_tsTask * psTaskTable[STUPIDOS_PRO_COUNT];

// 调度锁计数器
uint8_t u8SchedLockCount;

// 延时队列
tsList sTaskDelayList;

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
    psTask->u32DelayTicks = 0;
    psTask->u32Prio = u32Prio;                                  // 设置任务的优先级
	
    psTask->u8State |=  STUPIDOS_TASK_STATE_RDY;
    OS_vNodeInit(&(psTask->sNodeDelay));

    psTaskTable[u32Prio] = psTask;                             // 填入任务优先级表
    OS_vBitmapSet(&sTaskPrioBitmap, u32Prio);                  // 标记优先级位置中的相应位
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
    return psTaskTable[u32HighestPrio];
}

/**********************************************************************************************************
** Function name        :   初始化调度器
** Descriptions         :   无
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskSchedInit (void)
{
    u8SchedLockCount = 0;
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
** Function name        :   OS_vTaskDelayInit
** Descriptions         :   初始化任务延时机制
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskDelayInit(void)
{
    OS_vListInit(&sTaskDelayList);
}

/**********************************************************************************************************
** Function name        :   OS_vTaskDelayWait
** Descriptions         :   将任务加入延时队列中
** input parameters     :   task    需要延时的任务
**                          ticks   延时的ticks
** output parameters    :   无
** Returned value       :   无
***********************************************************************************************************/
//void OS_vTaskDelayWait()
/**********************************************************************************************************
** Function name        :   tTimeTaskWakeUp
** Descriptions         :   将延时的任务从延时队列中唤醒
** input parameters     :   task  需要唤醒的任务
** output parameters    :   无
** Returned value       :   无
***********************************************************************************************************/


/**********************************************************************************************************
** Function name        :   OS_vTaskSystemTickHandler
** Descriptions         :   系统时钟节拍处理。
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void OS_vTaskSystemTickHandler () 
{
    // 检查所有任务的delayTicks数，如果不0的话，减1。
    int i;   
    uint32_t status = OS_u32TaskEnterCritical();

    for (i = 0; i < STUPIDOS_PRO_COUNT; i++) 
    {
        if (psTaskTable[i]->u32DelayTicks > 0)
        {
            psTaskTable[i]->u32DelayTicks--;
        }
        else 
        {
            OS_vBitmapSet(&sTaskPrioBitmap, i);
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
   // 配置好当前要延时的ticks数
    uint32_t u32Status = OS_u32TaskEnterCritical();
    OS_psCurrentTask->u32DelayTicks = u32Delay;
    OS_vBitmapClear(&sTaskPrioBitmap, OS_psCurrentTask->u32Prio);
    OS_vTaskExitCritical(u32Status);

    // 然后进行任务切换，切换至另一个任务，或者空闲任务
    // delayTikcs会在时钟中断中自动减1.当减至0时，会切换回来继续运行。
    OS_vTaskSched();
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

/**********************************************************************************************************
** 应用示例
** 有两个任务，分别执行task1Entry和task2Entry。功能是分别对相应的变量进行周期性置0置1.
**********************************************************************************************************/
int32_t s32Task1Flag;
tsList sList;
tsNode sNode[8];

void task1Entry (void * param) 
{
		int i = 0;
    OS_vSetSysTickPeriod(10);
	
	  // 简单的测试链表的头部插入与删除结点操作
    // 其它接口的测试，请自行编写代码
    OS_vListInit(&sList);
    for (i = 0; i < 8; i++) 
    {
        OS_vNodeInit(&sNode[i]);
        OS_vListAddFirst(&sList, &sNode[i]);
    }

    for (i = 0; i < 8; i++) 
    {
        OS_psListRemoveFirst(&sList);
    }
		
    for (;;) 
    {
        s32Task1Flag = 1;
        OS_vTaskDelay(1);
        s32Task1Flag = 0;
        OS_vTaskDelay(1);
    }
}

int32_t s32task2Flag;
void task2Entry (void * param) 
{
    for (;;) 
    {
        s32task2Flag = 1;
        OS_vTaskDelay(1);
        s32task2Flag = 0;
        OS_vTaskDelay(1);
    }
}

// 任务1和任务2的任务结构，以及用于堆栈空间
OS_tsTask sTask1;
OS_tsTask sTask2;
OS_tTaskStack task1Env[1024];
OS_tTaskStack task2Env[1024];

// 用于空闲任务的任务结构和堆栈空间
OS_tsTask sTaskIdle;
OS_tTaskStack idleTaskEnv[1024];

void idleTaskEntry (void * param) {
    for (;;)
    {
        // 空闲任务什么都不做
    }
}

int main () 
{
    // 优先初始化tinyOS的核心功能
    OS_vTaskSchedInit();

    // 初始化任务1和任务2结构，传递运行的起始地址，想要给任意参数，以及运行堆栈空间
    OS_vTaskInit(&sTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
    OS_vTaskInit(&sTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);

    // 创建空闲任务
    OS_vTaskInit(&sTaskIdle, idleTaskEntry, (void *)0, STUPIDOS_PRO_COUNT - 1, &idleTaskEnv[1024]);
    
    // 这里，不再指定先运行哪个任务，而是自动查找最高优先级的任务运行
    OS_psNextTask = OS_psTaskHighestReady();

    // 切换到nextTask， 这个函数永远不会返回
    OS_vTaskRunFirst();
    return 0;
}
