/**************************************************************************************************
**************************数据处理线程***********************************************************
***************************************************************************************************/
#ifndef __DATAPROCESS_H
#define __DATAPROCESS_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "DataTransferManage.h"
#include "TcpPacketServer.h"				//获得ClientNum
//#include "ParametersLoad.h"					//加载配置参数
//#include "ObjectDetection.h"				//对象创建进程

//System
#define PCCmdActionHeartBeat 							1
#define PCCmdActionWarmRestart  					2
#define PCCmdActionColdRestart 						3
#define PCCmdActionReconfiguration  			4

#define PCCmdActionStartIOLive  					10
#define PCCmdActionGetConnectedClientID 	11
#define PCCmdActionErrorMessage  					20
#define PCCmdActionErrorAcknowledge  			21

//Tracking
#define PCCmdActionObjectRunIn  					100
#define PCCmdActionObjectRunOut  					101
#define PCCmdActionObjectDelete  					102
#define PCCmdActionTriggerCamera  				103
#define PCCmdActionTriggerIOSensor  			104
#define PCCmdActionGetMachineData  				105
#define PCCmdActionSetPushResult  				106
#define PCCmdActionSetUserResult  				107
#define PCCmdActionStartTracking  				120
#define PCCmdActionObjectFallDown  				130

//Automation
#define PCCmdActionSetPLCVariable  				200
#define PCCmdActionStartControl  					201

//Diagnostics
#define PCCmdActionSetTrackingMode  			301
#define PCCmdActionObjectPosition  				302
#define PCCmdActionObjectWidth  					303
#define PCCmdActionRequestModuleInfo 			304
#define PCCmdActionRequestPLCInfo  				305
#define PCCmdActionTrackingDummy  				306


uint8_t DataProcessTask_init(void);

#endif

