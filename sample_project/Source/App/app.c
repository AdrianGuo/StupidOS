/*************************************** Copyright (c)******************************************************
** File name            :   app.c
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   应用主文件，包含应用代码
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

/**********************************************************************************************************
** 应用示例
** 有两个任务，分别执行task1Entry和task2Entry。功能是分别对相应的变量进行周期性置0置1.
**********************************************************************************************************/


// 任务1和任务2的任务结构，以及用于堆栈空间
OS_tsTask sTask1;
OS_tsTask sTask2;
OS_tsTask sTask3;
OS_tsTask sTask4;
OS_tTaskStack task1Env[1024];
OS_tTaskStack task2Env[1024];
OS_tTaskStack task3Env[1024];
OS_tTaskStack task4Env[1024];

OS_tsSem sSem1;
OS_tsSem sSem2;

int32_t s32Task1Flag;
void task1Entry (void * param) 
{
    OS_vSetSysTickPeriod(10);
	
    // 最大10个信号量计数，初始化值为0
    OS_vSemInit(&sSem1, 0, 10);
	
    for (;;) 
    {
        if(OS_psCurrentTask->u32WaitEventResult != E_OS_ERROR_DEL)
        {
            OS_u32SemWait(&sSem1, 10);
        }

        s32Task1Flag = 1;
        OS_vTaskDelay(1);
        s32Task1Flag = 0;
        OS_vTaskDelay(1);
    }
}

int32_t s32task2Flag;
void task2Entry (void * param) 
{
    uint32_t u32Sem1Deled = 0;
	for (;;) 
    {  
        s32task2Flag = 1;
        OS_vTaskDelay(1);
        s32task2Flag = 0;
        OS_vTaskDelay(1);

        if(!u32Sem1Deled)
        {
            OS_u32SemDestroy(&sSem1);
            u32Sem1Deled = 1;
        }
    }
}

int32_t s32task3Flag;
void task3Entry (void * param) 
{
    for (;;) 
    {
        s32task3Flag = 1;
        OS_vTaskDelay(1);
        s32task3Flag = 0;
        OS_vTaskDelay(1);
    }
}



int32_t s32task4Flag;
void task4Entry (void * param) 
{
    for (;;) 
    { 	
        s32task4Flag = 1;
        OS_vTaskDelay(1);
        s32task4Flag = 0;
        OS_vTaskDelay(1);
    }
}



void APP_vTaskInit()
{
    // 初始化任务1和任务2结构，传递运行的起始地址，想要给任意参数，以及运行堆栈空间
    OS_vTaskInit(&sTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
    OS_vTaskInit(&sTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
    OS_vTaskInit(&sTask3, task3Entry, (void *)0x33333333, 1, &task3Env[1024]);
    OS_vTaskInit(&sTask4, task4Entry, (void *)0x44444444, 1, &task4Env[1024]);
}

