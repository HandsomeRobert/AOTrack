/**************************************************************************************************
**************************跟踪线程************************************************************
***************************************************************************************************/
#ifndef __TRACKING_H
#define __TRACKING_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "bsp_encoder.h"						//	编码器	
#include "LightSignal.h"						//光电
#include "led.h"										//用LED来模拟触发
#include "DataTransferManage.h"
#include "TcpPacketServer.h"
#include "ParametersLoad.h"					//加载配置参数

struct ObjectTracking 
{
	byte 	ClientID;
	short ObjectID;
	bool 	CreateObjectFlag;						//default = false
	__IO 	int64_t BornEncoderNum;					//64Bit来存储可溢出的编码器值
	byte 	CameraID;
	char 	*pData;
	bool  ProcessedResult;

	/***传递动作所使用的标志位***/
	bool	objectDelieverdFlag;				//用于标记此对象是否被传递出去了
	byte	objectModulePosition;							//记录对象所在的跟踪段
	//byte	delieverDestination;				//记录对象所要传递到的跟踪�
	__IO 	int64_t deliverEncoderNum;	//传递动作的编码器值
	
	bool 	objectAliveFlag;																							//用于判断对象是否完成了跟踪过程，是否仍生存
	//bool	modulePosition[maxTrackingModule];														//定义一个跟踪对象处于跟踪段的位置！！！！！！！！！！！！
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!modulePosition 和objectPostition有重复定义，记得修改
	bool 	triggerPosition[maxTrackingModule][maxActionSingleModule];		//定义一个用于记录跟踪对象在对应的位置是否被触发的二维数据
	
	__IO 	int64_t ModuleReferenceEncoderNum[maxTrackingModule];					//给对象的每个跟踪段定义一个编码器参考值

};

extern __IO int64_t EncoderNumber;

#define maxTrackingObjects  64			//最大维护多少个跟踪对象
//#define defaultPositionRange 3		//默认允许到达动作执行处的编码器数值偏差
#define Message_TrrigerCamera 1			//动作对应的消息值
#define Message_TrrigerSensor 2
#define Message_TrrigerOutput 3
#define Message_TrrigerPush 	4


uint8_t TrackingTask_init(void);

#endif

