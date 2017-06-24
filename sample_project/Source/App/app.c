/*************************************** Copyright (c)******************************************************
** File name            :   app.c
** Latest modified Date :   2016-06-01
** Latest Version       :   0.1
** Descriptions         :   主文件，包含应用代码
**
**--------------------------------------------------------------------------------------------------------
** Created by           :   
** Created date         :   
** Version              :   
** Descriptions         :   The original version
**
**--------------------------------------------------------------------------------------------------------
** Copyright            :   
** Author Blog          :   
**********************************************************************************************************/

#include "stupidOS.h"
#include "ARMCM3.h"

/**********************************************************************************************************
** 应用示例
** 有两个任务，分别执行task1Entry和task2Entry。功能是分别对相应的变量进行周期性置0置1.
**********************************************************************************************************/
int32_t s32Task1Flag;
OS_tsList sList;
OS_tsNode sNode[8];

void task1Entry (void * param) 
{
	// int i = 0;
    OS_vSetSysTickPeriod(10);
	
	// // 简单的测试链表的头部插入与删除结点操作
    // // 其它接口的测试，请自行编写代码
    // OS_vListInit(&sList);
    // for (i = 0; i < 8; i++) 
    // {
    //     OS_vNodeInit(&sNode[i]);
    //     OS_vListAddFirst(&sList, &sNode[i]);
    // }

    // for (i = 0; i < 8; i++) 
    // {
    //     OS_psListRemoveFirst(&sList);
    // }
		
    for (;;) 
    {
        s32Task1Flag = 1;
        OS_vTaskDelay(1);
        s32Task1Flag = 0;
        OS_vTaskDelay(1);
    }
}

void delay ()
{
    int i;
    for (i = 0; i < 0xFF; i++) {}
}

int32_t s32task2Flag;
void task2Entry (void * param) 
{
    for (;;) 
    {
        s32task2Flag = 1;
        delay();
        s32task2Flag = 0;
        delay();
    }
}

int32_t s32task3Flag;
void task3Entry (void * param) 
{
    for (;;) 
    {
        s32task3Flag = 1;
        delay();
        s32task3Flag = 0;
        delay();
    }
}

// 任务1和任务2的任务结构，以及用于堆栈空间
OS_tsTask sTask1;
OS_tsTask sTask2;
OS_tsTask sTask3;
OS_tTaskStack task1Env[1024];
OS_tTaskStack task2Env[1024];
OS_tTaskStack task3Env[1024];



void APP_vTaskInit()
{
    // 初始化任务1和任务2结构，传递运行的起始地址，想要给任意参数，以及运行堆栈空间
    OS_vTaskInit(&sTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
    OS_vTaskInit(&sTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
    OS_vTaskInit(&sTask3, task3Entry, (void *)0x33333333, 1, &task3Env[1024]);
}

