/**************************************************************************************************
**************************跟踪线程************************************************************
***************************************************************************************************/
#ifndef __TRACKING_H
#define __TRACKING_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "bsp_encoder.h"						//	编码器	
//#include "LightSignal.h"					//光电
//#include "led.h"									//用LED来模拟触发
#include "DataTransferManage.h"
#include "TcpPacketServer.h"				//获得ClientNum
#include "ParametersLoad.h"					//加载配置参数
#include "ObjectDetection.h"				//对象创建进程

//#define defaultPositionRange 3		//默认允许到达动作执行处的编码器数值偏差
#define Message_TrrigerCamera 1			//动作对应的消息值
#define Message_TrrigerSensor 2
#define Message_TrrigerOutput 3
#define Message_TrrigerPush 	4

uint8_t TrackingTask_init(void);

#endif

