/*************************************** Copyright (c)******************************************************
** File name            :   os_task.c
** Latest modified Date :   
** Latest Version       :   0.1
** Descriptions         :   stupidOS���������ӿ�ʵ��
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
** Function name        :   OS_vTaskInit
** Descriptions         :   ��ʼ������ṹ
** parameters           :   task        Ҫ��ʼ��������ṹ
** parameters           :   entry       �������ں���
** parameters           :   param       ���ݸ���������в���
** Returned value       :   ��
***********************************************************************************************************/
void OS_vTaskInit (OS_tsTask * psTask, void (*pvEntry)(void *), void *pvParam, uint32_t u32Prio, uint32_t * pu32Stack)
{
    // Ϊ�˼򻯴��룬tinyOS������������ʱ�л�����һ�����񣬻��������й������ڲ�ͬ�������л�
    // ��ִ�еĲ��������ȱ��浱ǰ��������л���������CPU�Ĵ���ֵ���Ķ�ջ��(����Ѿ��������������Ļ�)��Ȼ����
    // ȡ������һ������Ķ�ջ��ȡ��֮ǰ�����л���������Ȼ��ָ���CPU�Ĵ���
    // �����л���֮ǰ��û�����й�����������Ϊ������һ������ٵġ������ֳ���Ȼ��ʹ�ø��ֳ��ָ���

    // ע���������㣺
    // 1������Ҫ�õ��ļĴ�����ֱ�����˼Ĵ����ţ�������IDE����ʱ�鿴Ч����
    // 2��˳���ܱ䣬Ҫ���PendSV_Handler�Լ�CPU���쳣�Ĵ������������
    *(--pu32Stack) = (unsigned long)(1<<24);                // XPSR, ������Thumbģʽ���ָ���Thumb״̬����ARM״̬����
    *(--pu32Stack) = (unsigned long)pvEntry;                // �������ڵ�ַ
    *(--pu32Stack) = (unsigned long)0x14;                   // R14(LR), ���񲻻�ͨ��return xxx�����Լ�������δ��
    *(--pu32Stack) = (unsigned long)0x12;                   // R12, δ��
    *(--pu32Stack) = (unsigned long)0x3;                    // R3, δ��
    *(--pu32Stack) = (unsigned long)0x2;                    // R2, δ��
    *(--pu32Stack) = (unsigned long)0x1;                    // R1, δ��
    *(--pu32Stack) = (unsigned long)pvParam;                // R0 = pvParam, �����������ں���
    *(--pu32Stack) = (unsigned long)0x11;                   // R11, δ��
    *(--pu32Stack) = (unsigned long)0x10;                   // R10, δ��
    *(--pu32Stack) = (unsigned long)0x9;                    // R9, δ��
    *(--pu32Stack) = (unsigned long)0x8;                    // R8, δ��
    *(--pu32Stack) = (unsigned long)0x7;                    // R7, δ��
    *(--pu32Stack) = (unsigned long)0x6;                    // R6, δ��
    *(--pu32Stack) = (unsigned long)0x5;                    // R5, δ��
    *(--pu32Stack) = (unsigned long)0x4;                    // R4, δ��

    psTask->psStack = pu32Stack;                                // �������յ�ֵ

    psTask->u32Prio = u32Prio;   								// ������������ȼ�     
    psTask->u32Slice = STUPIDOS_SLICE_MAX;
                              
	OS_vNodeInit(&(psTask->sNodeLink));
	
	psTask->u32DelayTicks = 0;
	OS_vNodeInit(&(psTask->sNodeDelay));
	
	psTask->u32SuspendCount = 0;
	
	OS_vTaskSchedRdy(psTask);
	
    psTask->u8State |=  STUPIDOS_TASK_STATE_RDY;
}


/**********************************************************************************************************
** Function name        :   OS_vTaskSuspend
** Descriptions         :   ����ָ��������
** parameters           :   task        �����������
** Returned value       :   ��
***********************************************************************************************************/
void OS_vTaskSuspend (OS_tsTask * psTask) 
{
    uint32_t u32Status = OS_u32TaskEnterCritical();

    // ��������Ѿ�������ʱ״̬���������
    if (!(psTask->u8State & STUPIDOS_TASK_STATE_DELAYED)) 
    {
        // ���ӹ������������������ִ�е�һ�ι������ʱ���ſ����Ƿ�
        // Ҫִ�������л�����
        if (++psTask->u32SuspendCount <= 1)
        {
            // ���ù����־
            psTask->u8State |= STUPIDOS_TASK_STATE_SUSPEND;

            // ����ʽ�ܼ򵥣����ǽ���Ӿ����������Ƴ��������������Ͳ��ᷢ����
            // Ҳ��û���л�������������
            OS_vTaskSchedUnRdy(psTask);

            // ��Ȼ���������������Լ�����ô���л�����������
            if (psTask == OS_psCurrentTask)
            {
                OS_vTaskSched();
            }
        }
    }

    OS_vTaskExitCritical(u32Status);
}


/**********************************************************************************************************
** Function name        :   OS_vTaskWakeUp
** Descriptions         :   ���ѱ����������
** parameters           :   task        �����ѵ�����
** Returned value       :   ��
***********************************************************************************************************/
void OS_vTaskWakeUp (OS_tsTask * psTask)
{
    // �����ٽ���
    uint32_t u32Status = OS_u32TaskEnterCritical();

     // ��������Ƿ��ڹ���״̬
    if (psTask->u8State & STUPIDOS_TASK_STATE_SUSPEND)
    {
        // �ݼ�������������Ϊ0�ˣ�����������־��ͬʱ���ý������״̬
        if (--psTask->u32SuspendCount == 0) 
        {
            // ��������־
            psTask->u8State &= ~STUPIDOS_TASK_STATE_SUSPEND;

            // ͬʱ������Żؾ���������
            OS_vTaskSchedRdy(psTask);

            // ���ѹ����У������и������ȼ������������ִ��һ���������
            OS_vTaskSched();
        }
    }

    // �˳��ٽ���
    OS_vTaskExitCritical(u32Status);
}

