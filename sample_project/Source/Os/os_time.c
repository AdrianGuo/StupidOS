/*************************************** Copyright (c)******************************************************
** File name            :   os_time.c
** Latest modified Date :   
** Latest Version       :   0.1
** Descriptions         :   stupidOS的时间管理接口
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
