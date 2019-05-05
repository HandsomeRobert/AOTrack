/**************************************************************************************************
**************************跟踪线程************************************************************
***************************************************************************************************/
#ifndef __ACTIONEXECUTE_H
#define __ACTIONEXECUTE_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "led.h"										//用LED来模拟触发

uint8_t ActionExecuteTask_init(void);

#endif

