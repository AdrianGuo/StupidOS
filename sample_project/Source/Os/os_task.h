/*************************************** Copyright (c)******************************************************
** File name            :   os_task.h
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
#ifndef OS_TASK_H
#define OS_TASK_H

#define STUPIDOS_TASK_STATE_RDY                   0
#define STUPIDOS_TASK_STATE_DESTROYED             (1 << 0)
#define STUPIDOS_TASK_STATE_DELAYED               (1 << 1)
#define STUPIDOS_TASK_STATE_SUSPEND               (1 << 2)

//高16位用于等待事件
#define STUPIDOS_TASK_WAIT_MASK                   (0xFF << 16)

// 前置声明
struct OS_sEventControl;

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
	uint32_t u32State;
	OS_tsNode	sNodeDelay;
    // 任务延时计数器
    uint32_t u32DelayTicks;
	
	OS_tsNode	sNodeLink;
	uint32_t u32Slice;
	
	uint32_t u32SuspendCount;
	
	// 任务被删除时调用的清理函数
	void (* OS_vClean) (void *pvParam);
	
	// 传递给清理函数的参数
	void *pvCleanParam;
	
    // 请求删除标志，非0表示请求删除
    uint8_t u8RequestDeleteFlag;

	// 任务正在等待的事件类型
    struct OS_sEventControl *psWaitEvent;

    // 等待事件的消息存储位置
    void *pvEventMsg;

    // 等待事件的结果
    uint32_t u32WaitEventResult;
}OS_tsTask;

typedef struct OS_tTaskInfo{
    // 任务延时计数器
    uint32_t u32DelayTicks;

    // 任务的优先级
    uint32_t u32Prio;

    // 任务当前状态
    uint32_t u32State;

    // 当前剩余的时间片
    uint32_t u32Slice;

    // 被挂起的次数
    uint32_t u32SuspendCount;
}OS_tsTaskInfo;

void OS_vTaskInit (OS_tsTask * psTask, void (*pvEntry)(void *), void *pvParam, uint32_t u32Prio, uint32_t * pu32Stack);

void OS_vTaskSuspend (OS_tsTask * psTask);
void OS_vTaskWakeUp (OS_tsTask * psTask);

void OS_vTaskSetCleanCallFunc (OS_tsTask * psTask, void (*OS_vCleanParam)(void * pvParamParam), void * pvParamParam);
void OS_vTaskForceDelete (OS_tsTask * psTask); 
void OS_vTaskRequestDelete (OS_tsTask * psTask); 
uint8_t OS_u8TaskIsRequestedDeleted (void); 
void OS_vTaskDeleteItSelf (void); 

void OS_vTaskGetInfo(OS_tsTask *psTask, OS_tsTaskInfo *psTaskInfo);


#endif
