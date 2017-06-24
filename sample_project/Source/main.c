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
    OS_vTaskInit(&sTaskIdle, idleTaskEntry, (void *)0, STUPIDOS_PRO_COUNT - 1, &idleTaskEnv[1024]);
    
    // 这里，不再指定先运行哪个任务，而是自动查找最高优先级的任务运行
    OS_psNextTask = OS_psTaskHighestReady();

    // 切换到nextTask， 这个函数永远不会返回
    OS_vTaskRunFirst();
    return 0;
}

