/*************************************** Copyright (c)******************************************************
** File name            :   os_mbox.c
** Latest modified Date :   
** Latest Version       :   0.1
** Descriptions         :   stupidOS的邮箱实现
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
#include "stupidOS.h"
#include "os_mbox.h"

/**********************************************************************************************************
** Function name        :   OS_vMboxInit
** Descriptions         :   初始化邮箱
** parameters           :   psMbox 等待初始化的邮箱
** parameters           :   ppvMsgBuffer 消息存储缓冲区
** parameters           :   u32MaxCount 最大计数
** Returned value       :   无
***********************************************************************************************************/
void OS_vMboxInit (OS_tsMbox * psMbox, void ** ppvMsgBuffer, uint32_t u32MaxCount)
{
    OS_vEventInit(&psMbox->sEvent, E_OS_EVENT_TYPE_MBOX);

    psMbox->ppvMsgBuffer = ppvMsgBuffer;
    psMbox->u32MaxCount = u32MaxCount;
    psMbox->u32Count = 0;
    psMbox->u32Read = 0;
    psMbox->u32Write = 0;
}


/**********************************************************************************************************
** Function name        :   OS_u32MboxWait
** Descriptions         :   等待邮箱, 获取一则消息
** parameters           :   psMbox 等待的邮箱
** parameters           :   ppvMsgBuffermsg 消息存储缓存区
** parameters           :   u32WaitTicks 最大等待的ticks数，为0表示无限等待
** Returned value       :   等待结果,E_OS_ERROR_NO_ERROR,E_OS_ERROR_TIME_OUT,E_OS_ERROR_RESOURCE_UNAVALIABLE
***********************************************************************************************************/
uint32_t OS_u32MboxWait (OS_tsMbox * psMbox, void **ppvMsgBuffermsg, uint32_t u32WaitTicks) 
{
    uint32_t u32Status = OS_u32TaskEnterCritical();
    if(psMbox->u32Count > 0)
    {
        --psMbox->u32Count;
        /*如果有消息，则取出一个消息地址*/
        *ppvMsgBuffermsg = psMbox->ppvMsgBuffer[psMbox->u32Read++];

        if(psMbox->u32Read >= psMbox->u32MaxCount)
        {
            psMbox->u32Read = 0;
        }
        OS_vTaskExitCritical(u32Status);
        return E_OS_ERROR_NO_ERROR;
    }
    else
    {
        OS_vEventWait(&psMbox->sEvent, OS_psCurrentTask, (void *)0, E_OS_EVENT_TYPE_MBOX, u32WaitTicks);
        OS_vTaskExitCritical(u32Status);

        OS_vTaskSched();

        /*任务再次切换至此时，取出等待事件消息*/
        *ppvMsgBuffermsg = OS_psCurrentTask->pvEventMsg;

        return OS_psCurrentTask->u32WaitEventResult;
    }
}

/**********************************************************************************************************
** Function name        :   OS_u32MboxNoWaitGet
** Descriptions         :   获取一则消息，如果没有消息，则立即退回
** parameters           :   psMbox 获取消息的邮箱
** parameters           :   ppvMsgBuffermsg 消息存储缓存区
** Returned value       :   获取结果, E_OS_ERROR_RESOURCE_UNAVALIABLE，E_OS_ERROR_NO_ERROR
***********************************************************************************************************/
uint32_t OS_u32MboxNoWaitGet (OS_tsMbox * psMbox, void **ppvMsgBuffermsg)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();

    if(psMbox->u32Count > 0)
    {
        --psMbox->u32Count;
        /*如果有消息，则取出一个消息地址*/
        *ppvMsgBuffermsg = psMbox->ppvMsgBuffer[psMbox->u32Read++];

        if(psMbox->u32Read >= psMbox->u32MaxCount)
        {
            psMbox->u32Read = 0;
        }
        OS_vTaskExitCritical(u32Status);
        return E_OS_ERROR_NO_ERROR;
    }
    else
    {
        OS_vTaskExitCritical(u32Status);
        return E_OS_ERROR_RESOURCE_UNAVALIABLE;
    }
}

/**********************************************************************************************************
** Function name        :   OS_u32MboxNotify
** Descriptions         :   通知消息可用，唤醒等待队列中的一个任务，或者将消息插入到邮箱中
** parameters           :   psMbox 操作的信号量
** parameters           :   pvMsgBuffermsg 发送的消息
** parameters           :   u32NotifyOption 发送的选项
** Returned value       :   E_OS_ERROR_RESOURCE_FULL，E_OS_ERROR_NO_ERROR
***********************************************************************************************************/
uint32_t OS_u32MboxNotify (OS_tsMbox * psMbox, void * pvMsgBuffermsg, uint32_t u32NotifyOption)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();
    if(OS_u32EventWaitCount(&psMbox->sEvent) > 0)
    {
    	// 如果有的话，则直接唤醒位于队列首部（最先等待）的任务
        OS_tsTask * psTask = OS_psEventWakeUp(&psMbox->sEvent, (void *)pvMsgBuffermsg, E_OS_ERROR_NO_ERROR );
        
        // 如果这个任务的优先级更高，就执行调度，切换过去
        if (psTask->u32Prio < OS_psCurrentTask->u32Prio)
        {
            OS_vTaskSched(); 
    	}
    }
    else
    {
    	// 如果没有任务等待的话，将消息插入到缓冲区中
    	if (psMbox->u32Count >= psMbox->u32MaxCount) 
    	{
		    OS_vTaskExitCritical(u32Status);
		    return E_OS_ERROR_RESOURCE_FULL;
    	}

        /* 向前写，覆盖之前的，改变Read指针，但Write指针不改变 */
        if(u32NotifyOption & OS_MBOX_SEND_FRONT)
        {
            if(psMbox->u32Read <= 0)
            {
                psMbox->u32Read = psMbox->u32MaxCount -1;
            }
            else
            {
                psMbox->u32Read--;
            }
            psMbox->ppvMsgBuffer[psMbox->u32Read] = pvMsgBuffermsg;
        }
        else
        {
            /* 向后累加写 */
            psMbox->ppvMsgBuffer[psMbox->u32Write++] = pvMsgBuffermsg;
            if(psMbox->u32Write >= psMbox->u32MaxCount)
            {
                psMbox->u32Write = 0;
            }
        }
        psMbox->u32Count++;
    }
    OS_vTaskExitCritical(u32Status);
    return E_OS_ERROR_NO_ERROR;
}


/**********************************************************************************************************
** Function name        :   OS_vMboxFlush
** Descriptions         :   清空邮箱中所有消息
** parameters           :   psMbox 等待清空的邮箱
** Returned value       :   无
***********************************************************************************************************/
void OS_vMboxFlush (OS_tsMbox * psMbox)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();        
    
    // 如果队列中有任务等待，说明邮箱已经是空的了，不需要再清空
    if (OS_u32EventWaitCount(&psMbox->sEvent) == 0) 
    {
        psMbox->u32Read = 0;
        psMbox->u32Write = 0;
        psMbox->u32Count = 0;
    }

    OS_vTaskExitCritical(u32Status);
}

/**********************************************************************************************************
** Function name        :   OS_u32MboxDestroy
** Descriptions         :   销毁邮箱
** parameters           :   psMbox 需要销毁的邮箱
** Returned value       :   因销毁该信号量而唤醒的任务数量
***********************************************************************************************************/
uint32_t OS_u32MboxDestroy (OS_tsMbox * psMbox)
{       
    uint32_t u32Status = OS_u32TaskEnterCritical();  

    // 清空事件控制块中的任务
    uint32_t u32Count = OS_u32EventRemoveAll(&psMbox->sEvent, (void *)0, E_OS_ERROR_DEL);  
    
    OS_vTaskExitCritical(u32Status);
    
    // 清空过程中可能有任务就绪，执行一次调度
    if (u32Count > 0) 
    {
        OS_vTaskSched();
    } 
    return u32Count;  
}

/**********************************************************************************************************
** Function name        :   OS_vMboxGetInfo
** Descriptions         :   查询状态信息
** parameters           :   mbox 查询的邮箱
** parameters           :   info 状态查询存储的位置
** Returned value       :   无
***********************************************************************************************************/
void OS_vMboxGetInfo (OS_tsMbox * psMbox, OS_tsMboxInfo * psMboxInfo)
{
    uint32_t u32Status = OS_u32TaskEnterCritical();

    // 拷贝需要的信息
    psMboxInfo->u32Count = psMbox->u32Count;
    psMboxInfo->u32MaxCount = psMbox->u32MaxCount;
    psMboxInfo->u32TaskCount = OS_u32EventWaitCount(&psMbox->sEvent);

    OS_vTaskExitCritical(u32Status);
}
