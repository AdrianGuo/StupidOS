/*************************************** Copyright (c)******************************************************
** File name            :   os_time.c
** Latest modified Date :   
** Latest Version       :   0.1
** Descriptions         :   stupidOS��ʱ�����ӿ�
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
** Descriptions         :   ʹ��ǰ���������ʱ״̬��
** parameters           :   delay ��ʱ���ٸ�ticks
** Returned value       :   ��
***********************************************************************************************************/
void OS_vTaskDelay (uint32_t u32Delay) {
    // �����ٽ������Ա�������������������л��ڼ䣬������Ϊ�����жϵ���currentTask��nextTask���ܸ���
    uint32_t u32Status = OS_u32TaskEnterCritical();
    // ������ʱֵ��������ʱ����
    OS_vTimeTaskDelayWait(OS_psCurrentTask, u32Delay);
    // ������Ӿ��������Ƴ�
    OS_vTaskSchedUnRdy(OS_psCurrentTask);

    // Ȼ����������л����л�����һ�����񣬻��߿�������
    // delayTikcs����ʱ���ж����Զ���1.������0ʱ�����л������������С�
    OS_vTaskSched();

    OS_vTaskExitCritical(u32Status);
}
