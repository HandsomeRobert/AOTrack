/**************************************************************************************************
**************************跟踪线程************************************************************
***************************************************************************************************/
#ifndef __ACTIONEXECUTE_H
#define __ACTIONEXECUTE_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "led.h"										//用LED来模拟触发

#define maxDelayNum 20  						//动作延时列表大小
typedef struct
{
	bool isActionUsed;
	uint16_t PinID;
	byte IOOutput;
	uint32_t delayOverTime;
}ActionBuffer;


uint8_t ActionExecuteTask_init(void);

#endif

