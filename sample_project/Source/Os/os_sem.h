/*************************************** Copyright (c)******************************************************
** File name            :   os_sem.h
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   stupidOS的信号量实现
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
#ifndef OS_SEM_H
#define OS_SEM_H

#include "os_config.h"
#include "os_event.h"

typedef struct OS_sSem{
	OS_tsEventControl sEvent;
	uint32_t u32Count;
	uint32_t u32CountMax;
}OS_tsSem;

void OS_vSemInit(OS_tsSem *psSem, uint32_t u32StartCount, uint32_t u32MaxCount);
#endif

