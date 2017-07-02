/*************************************** Copyright (c)******************************************************
** File name            :   os_sem.c
** Latest modified Date :   
** Latest Version       :   0.1
** Descriptions         :   stupidOS的信号量实现
**
**--------------------------------------------------------------------------------------------------------
** Created by           :   Adrian
** Created date         :   2017-06-24
** Version              :   
** Descriptions         :   The original version
**
**--------------------------------------------------------------------------------------------------------
** Copyright            :   
** Author Blog          :   https://github.com/AdrianGuo
**********************************************************************************************************/
#include "os_sem.h"

void OS_vSemInit(OS_tsSem *psSem, uint32_t u32StartCount, uint32_t u32MaxCount)
{
    OS_vEventInit(&psSem->sEvent, E_OS_EVENT_TYPE_SEM);

    psSem->u32CountMax = u32MaxCount;
    
    if(u32MaxCount == 0)
    {
        psSem->u32Count = u32StartCount;
    }
    else
    {
        psSem->u32Count = (u32StartCount > u32MaxCount) ? u32MaxCount : u32StartCount;
    }
}
