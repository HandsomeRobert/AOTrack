/**************************************************************************************************
**************************���ݴ����߳�***********************************************************
***************************************************************************************************/
#ifndef __DATAPROCESS_H
#define __DATAPROCESS_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "DataTransferManage.h"
#include "TcpPacketServer.h"				//���ClientNum
//#include "ParametersLoad.h"					//�������ò���
//#include "ObjectDetection.h"				//���󴴽�����

//System
#define ActionHeartBeat 				1
#define ActionWarmRestart  			2
#define ActionColdRestart 			3
#define ActionReconfiguration  	4

#define ActionStartIOLive  			10
#define ActionGetConnectedClientID 11
#define ActionErrorMessage  		20
#define ActionErrorAcknowledge  21

//Tracking
#define ActionObjectRunIn  			100
#define ActionObjectRunOut  		101
#define ActionObjectDelete  		102
#define ActionTriggerCamera  		103
#define ActionTriggerIOSensor  	104
#define ActionGetMachineData  	105
#define ActionSetPushResult  		106
#define ActionSetUserResult  		107
#define ActionStartTracking  		120
#define ActionObjectFallDown  	130

//Automation
#define ActionSetPLCVariable  	200
#define ActionStartControl  		201

//Diagnostics
#define ActionSetTrackingMode  	301
#define ActionObjectPosition  	302
#define ActionObjectWidth  			303
#define ActionRequestModuleInfo 304
#define ActionRequestPLCInfo  	305
#define ActionTrackingDummy  		306




uint8_t DataProcessTask_init(void);

#endif

