/*************************************** Copyright (c)******************************************************
** File name            :   os_cpu.c
** Latest modified Date :   
** Latest Version       :   0.1
** Descriptions         :   stupidOS����CPU��صĴ���
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



/*********************************************************************************************************
** ϵͳʱ�ӽ��Ķ�ʱ��System Tick����
** ������Ŀǰ�Ļ�����ģ�������У�ϵͳʱ�ӽ���Ϊ12MHz
** ����ذ��ձ��̳��Ƽ����ã�����systemTick��ֵ�ͻ��б仯����Ҫ�鿴�����ֲ���˽�
**********************************************************************************************************/
void OS_vSetSysTickPeriod(uint32_t ms)
{
  SysTick->LOAD  = ms * SystemCoreClock / 1000 - 1; 
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
  SysTick->VAL   = 0;                           
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk; 
}

/**********************************************************************************************************
** Function name        :   SysTick_Handler
** Descriptions         :   SystemTick���жϴ�������
** parameters           :   ��
** Returned value       :   ��
***********************************************************************************************************/
void SysTick_Handler () 
{
    OS_vTaskSystemTickHandler();
}
