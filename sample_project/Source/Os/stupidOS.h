/*************************************** Copyright (c)******************************************************
** File name            :   stupidOS.h
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   stupidOS的核心头文件。包含了所有关键数据类型的定义，还有核心的函数。
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
#ifndef STUPIDOS_H
#define STUPIDOS_H

// 标准头文件，里面包含了常用的类型定义，如uint32_t
#include <stdint.h>

// tinyOS的内核库文件
#include "os_lib.h"

// TinyOS的配置文件
#include "os_config.h"

#define STUPIDOS_TASK_STATE_RDY                   0
#define STUPIDOS_TASK_STATE_DELAYED               (1 << 1)

// Cortex-M的堆栈单元类型：堆栈单元的大小为32位，所以使用uint32_t
typedef uint32_t OS_tTaskStack;

// 任务结构：包含了一个任务的所有信息
typedef struct OS_sTask {
	// 任务所用堆栈的当前堆栈指针。每个任务都有他自己的堆栈，用于在运行过程中存储临时变量等一些环境参数
	// 在tinyOS运行该任务前，会从stack指向的位置处，会读取堆栈中的环境参数恢复到CPU寄存器中，然后开始运行
	// 在切换至其它任务时，会将当前CPU寄存器值保存到堆栈中，等待下一次运行该任务时再恢复。
	// stack保存了最后保存环境参数的地址位置，用于后续恢复
    OS_tTaskStack * psStack;
    // 任务的优先级
    uint32_t u32Prio;
	uint8_t u8State;
	OS_tsNode	sNodeDelay;
    // 任务延时计数器
    uint32_t u32DelayTicks;
	OS_tsNode	sNodeLink;
	uint32_t u32Slice;
}OS_tsTask;

// 当前任务：记录当前是哪个任务正在运行
extern OS_tsTask * OS_psCurrentTask;
// 下一个将即运行的任务：在进行任务切换前，先设置好该值，然后任务切换过程中会从中读取下一任务信息
extern OS_tsTask * OS_psNextTask;

void OS_vTaskInit (OS_tsTask * psTask, void (*pvEntry)(void *), void *pvParam, uint32_t u32Prio, uint32_t * pu32Stack);
uint32_t OS_u32TaskEnterCritical (void);
void OS_vTaskExitCritical (uint32_t status);
void OS_vTaskRunFirst (void); 
void OS_vTaskSwitch (void);
OS_tsTask * OS_psTaskHighestReady (void);
void OS_vTaskSchedInit (void);
void OS_vTaskSchedDisable (void);
void OS_vTaskSchedEnable (void);
void OS_vTaskSchedRdy(OS_tsTask *psTask);
void OS_vTaskSchedUnRdy(OS_tsTask *psTask);
void OS_vTaskSched (void);

void OS_vTimeTaskDelayInit(void);
void OS_vTimeTaskDelayWait(OS_tsTask *psTask, uint32_t u32DelayTicksPra);
void OS_vTimeTaskWakeUp(OS_tsTask *psTask);

void OS_vTaskSystemTickHandler (void);
void OS_vTaskDelay (uint32_t u32Delay);
void OS_vSetSysTickPeriod(uint32_t ms);

#endif /* STUPIDOS_H */
