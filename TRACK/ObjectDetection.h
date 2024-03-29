/**************************************************************************************************
**************************跟踪线程************************************************************
***************************************************************************************************/
#ifndef __OBJECTDETECTION_H
#define __OBJECTDETECTION_H

#include "TaskManage.h"							//任务句柄资源统一管理位置
#include "HardwareList.h"						//硬件资源代号定义
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "bsp_encoder.h"						//	编码器	
#include "LightSignal.h"						//光电
//#include "led.h"									//用LED来模拟触发
#include "DataTransferManage.h"
#include "TcpPacketServer.h"				//获得ClientNum
#include "ParametersLoad.h"					//加载配置参数

#define maxTrackingObjects  64			//最大维护多少个跟踪对象
#define ModuleQueueLength		20			//每个跟踪模块的信息队列长度

#define	NoOutput												0
#define	TypeOutputDigital								1
#define	TypeOutputResult								2
typedef struct 
{
	byte 	ClientID;
	byte	ModuleID;
	__IO 	uint32_t ObjectID;
	__IO 	int64_t BornEncoderNum;			//64Bit来存储可溢出的编码器值
	char 	*pData;
	bool  ProcessedResult;
	
	bool 	objectAliveFlag;						//用于判断对象是否完成了跟踪过程，是否仍生存
}ObjectInfo;


typedef struct
{
	__IO 	uint32_t ObjectID;
//	__IO 	int64_t RefEncoderNum;			//64Bit来存储可溢出的编码器值
	__IO 	int64_t TargetValue;				//目标值
	enum enumActionType	ActionType;									//动作类型
	byte  ActionNumber;								//此动作是跟踪段上的第几个动作
	byte	OutputType;									//输出类型
	byte	OutputChannel;							//输出通道
	bool	IsActionAlive;
}StctActionListItem;


typedef struct
{
	__IO 	uint32_t 	DelieverdObjectID;				
	__IO 	int64_t 	DelieverdEncoderNum;	
	
}ModuleQueueItem;

//extern ObjectList ObjectInModuleList[maxTrackingModule];								//定义一个跟踪段列表指针数组
extern __IO ObjectInfo ObjectBuffer[maxTrackingObjects];										//创建最大只能保存maxTrackingObjects个对象的数组
extern __IO	uint32_t GlobalObjectID;																	//定义一个全局对象ID，__IO表示直接从地址处取值，取得最新值，允许所有软件硬件修改此值
extern QueueHandle_t	ModuleQueue[maxTrackingModule];				//创建信息队列用于接收信息	
extern StctActionListItem ObjectInModuleList[maxTrackingModule][maxTrackingObjects];//二维数组用于存储所有动作列表
uint8_t ObjectDetectionTask_init(void);

#endif

