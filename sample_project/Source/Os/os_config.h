/*************************************** Copyright (c)******************************************************
** File name            :   os_config.h
** Latest modified Date :   2017-06-24
** Latest Version       :   
** Descriptions         :   包含stupidOS裁剪配置参数
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
#ifndef OS_CONFIG_H
#define OS_CONFIG_H

#define	STUPIDOS_PRO_COUNT				32						// StupidOS任务的优先级序号
#define STUPIDOS_SLICE_MAX				10						// 每个任务最大运行的时间片计数

#define STUPIDOS_IDLETASK_STACK_SIZE              1024

#endif /* OS_CONFIG_H */
