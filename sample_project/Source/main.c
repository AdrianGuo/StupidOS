/*************************************** Copyright (c)******************************************************
** File name            :   main.c
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   主文件
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

#include "app.h"


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
    // 初始化延时队列
    OS_vTimeTaskDelayInit();

	APP_vTaskInit();

    // 创建空闲任务
    OS_vTaskInit(&sTaskIdle, idleTaskEntry, (void *)0, STUPIDOS_PRO_COUNT - 1, &idleTaskEnv[STUPIDOS_IDLETASK_STACK_SIZE]);
    
    // 这里，不再指定先运行哪个任务，而是自动查找最高优先级的任务运行
    OS_psNextTask = OS_psTaskHighestReady();

    // 切换到nextTask， 这个函数永远不会返回
    OS_vTaskRunFirst();
	
    return 0;
}

