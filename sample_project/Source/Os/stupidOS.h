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

// 任务管理头文件
#include "os_task.h"

// 事件控制头文件
#include "os_event.h"

// 信号量头文件
#include "os_sem.h"

// 邮箱头文件
#include "os_mbox.h"

typedef enum OS_eError{
	E_OS_ERROR_NO_ERROR,					// 没有错误
	E_OS_ERROR_TIME_OUT,					// 等待超时
	E_OS_ERROR_RESOURCE_UNAVALIABLE,		// 资源不可用
	E_OS_ERROR_DEL,							// 被删除
	E_OS_ERROR_RESOURCE_FULL,				// 资源已满
}OS_teError;



// 当前任务：记录当前是哪个任务正在运行
extern OS_tsTask * OS_psCurrentTask;
// 下一个将即运行的任务：在进行任务切换前，先设置好该值，然后任务切换过程中会从中读取下一任务信息
extern OS_tsTask * OS_psNextTask;


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



void OS_vTaskSchedRemove(OS_tsTask *psTask);
void OS_vTimeTaskRemove(OS_tsTask *psTask);




#endif /* STUPIDOS_H */
