/*************************************** Copyright (c)******************************************************
** File name            :   app.c
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   Ӧ�����ļ�������Ӧ�ô���
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
** Ӧ��ʾ��
** ���������񣬷ֱ�ִ��task1Entry��task2Entry�������Ƿֱ����Ӧ�ı���������������0��1.
**********************************************************************************************************/


// ����1������2������ṹ���Լ����ڶ�ջ�ռ�
OS_tsTask sTask1;
OS_tsTask sTask2;
OS_tsTask sTask3;
OS_tsTask sTask4;
OS_tTaskStack task1Env[1024];
OS_tTaskStack task2Env[1024];
OS_tTaskStack task3Env[1024];
OS_tTaskStack task4Env[1024];



int32_t s32Task1Flag;

void task1DestroyFunc (void * param) 
{
    s32Task1Flag = 0;
}

void task1Entry (void * param) 
{
	OS_tsTaskInfo sTaskInfo;
	
    OS_vSetSysTickPeriod(10);
	
    OS_vTaskSetCleanCallFunc(&sTask1, task1DestroyFunc, (uint8_t *)0);

    for (;;) 
    {
		OS_vTaskGetInfo(OS_psCurrentTask, &sTaskInfo);
		
		OS_vTaskGetInfo(&sTask2, &sTaskInfo);
        s32Task1Flag = 1;
        OS_vTaskDelay(1);
        s32Task1Flag = 0;
        OS_vTaskDelay(1);
    }
}

int32_t s32task2Flag;
void task2Entry (void * param) 
{
    int task1Deleted = 0;

	for (;;) 
    {
        s32task2Flag = 1;
        OS_vTaskDelay(1);
        s32task2Flag = 0;
        OS_vTaskDelay(1);

        if(!task1Deleted)
        {
            OS_vTaskForceDelete(&sTask1);
            task1Deleted = 1;
        }
    }
}

int32_t s32task3Flag;
void task3Entry (void * param) 
{
    for (;;) 
    {
        // ����Ƿ�Ҫ��ɾ�������Լ�
        if (OS_u8TaskIsRequestedDeleted())
        {
            // ��һЩ������
            s32task3Flag = 0;

            // Ȼ������ɾ���Լ�
            OS_vTaskDeleteItSelf();
        }

        s32task3Flag = 1;
        OS_vTaskDelay(1);
        s32task3Flag = 0;
        OS_vTaskDelay(1);
    }
}



int32_t s32task4Flag;
void task4Entry (void * param) 
{
    int task3Deleted = 0;
    for (;;) 
    {
        s32task4Flag = 1;
        OS_vTaskDelay(1);
        s32task4Flag = 0;
        OS_vTaskDelay(1);

        if(!task3Deleted)
        {
            OS_vTaskRequestDelete(&sTask3);
            task3Deleted = 1;
        }
    }
}



void APP_vTaskInit()
{
    // ��ʼ������1������2�ṹ���������е���ʼ��ַ����Ҫ������������Լ����ж�ջ�ռ�
    OS_vTaskInit(&sTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
    OS_vTaskInit(&sTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
    OS_vTaskInit(&sTask3, task3Entry, (void *)0x33333333, 1, &task3Env[1024]);
    OS_vTaskInit(&sTask4, task4Entry, (void *)0x44444444, 1, &task4Env[1024]);
}

