/**************************************************************************************************
**************************�����߳�************************************************************
***************************************************************************************************/
#ifndef __ACTIONEXECUTE_H
#define __ACTIONEXECUTE_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "led.h"										//��LED��ģ�ⴥ��

#define maxDelayNum 20  						//������ʱ�б��С
typedef struct
{
	bool isActionUsed;
	uint16_t PinID;
	byte IOOutput;
	uint32_t delayOverTime;
}ActionBuffer;


uint8_t ActionExecuteTask_init(void);

#endif

