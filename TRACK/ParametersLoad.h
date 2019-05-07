/**************************************************************************************************
**************************参数加载线程******************************************************
***************************************************************************************************/
#ifndef __PARAMETERSLOAD_H
#define __PARAMETERSLOAD_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "ff.h"											//FATFS文件系统代码
#include "exfuns.h"									//FATFS扩展代码
#include "sdmmc_sdcard.h"						//SD卡驱动
#include "sdram.h"									//SDRAM驱动
#include "ezxml.h"
#include "MyList.h"									//用于创建跟踪段列表

#define maxTrackingModule  8 				//定义允许的最大跟踪段数量
#define maxActionSingleModule  15 				//定义允许的最大动作数量

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////内部使用接口/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////定义各种数据类型结构
//跟踪段通用配置
struct TrackingConfiguration
{
	uint32_t ConfigurationID;				//配置识别ID
	uint8_t  HostClientID;					//主程序客户端ID
	uint8_t  SimClientID;						//仿真客户端ID
	uint16_t Timer;									//时钟
	
	uint8_t  ReadySignal;						//就绪信号，接IO输出
	uint8_t  ErrorSignal;						//故障信号，接IO输出
	uint8_t  MessageSignal;					//信息信号信号，接IO输出
	uint8_t  WarningSignal;					//警告信号，接IO输出
};
///////动作种类ActionList
//询问机器数据动作
struct propActionRequestMachineData
{
	bool			ActionTriggerFlag;				//用于确认是否从动作列表中开启此动作了。
	uint32_t ConfigurationID;
	char* 	 Name;									//名字
	uint32_t TargetValue;						//目标值（到编码器的哪个值触发此动作）
	uint16_t HighestTargetValueAdjust;//高速补偿值
};
//设置输出动作
struct propActionSetOutput
{
	bool			ActionTriggerFlag;				//用于确认是否从动作列表中开启此动作了。
	uint32_t ConfigurationID;
	char* 	 Name;
	uint32_t TargetValue;						//目标值（到编码器的哪个值触发此动作）
	uint16_t HighestTargetValueAdjust;
	
	uint16_t DigitalOutput;					//信号输出到哪个IO
	uint16_t OutputDuration;				//高or低电平信号的持续时间
	bool     OutputInvert;					//信号反转，即高低电平翻转是否
};
//传递动作
struct propActionObjectTakeOver
{
	bool			ActionTriggerFlag;				//用于确认是否从动作列表中开启此动作了。
	uint32_t ConfigurationID;
	char* 	 Name;
	uint32_t TargetValue;
	uint8_t  HighestTargetValueAdjust;
	uint8_t  DestinationModule;			 //传递对象到哪个跟踪段
};
//触发相机动作
struct propActionTriggerCamera
{
	bool			ActionTriggerFlag;				//用于确认是否从动作列表中开启此动作了。
	uint32_t ConfigurationID;
	char* 	 Name;
	uint32_t TargetValue;							//目标值
	uint8_t  HighestTargetValueAdjust;
	
	uint8_t  ClientID;								//检测程序ID，隶属于哪个Inspector程序，eg:192.168.66.11
	uint16_t CameraID;								//相机ID，使用哪个相机
	uint8_t  ImageIndex;							//图像序号，此相机的第几张图
	uint16_t DigitalOutput;						//相机触发信号输出到哪个IO
	uint16_t OutputDuration;					//触发信号持续时间
};
//触发IO采集动作
struct propActionTriggerSensor
{
	bool			ActionTriggerFlag;				//用于确认是否从动作列表中开启此动作了。
	uint32_t ConfigurationID;
	char* 	 Name;
	uint32_t TargetValue;							//目标值
	uint8_t  HighestTargetValueAdjust;
	
	uint8_t  ClientID;								//检测程序ID，隶属于哪个Inspector程序，eg:192.168.66.11
	uint8_t  SensorID;								//传感器ID，使用哪个传感器采集
};
//剔除动作
struct propActionPushOut
{
	bool			ActionTriggerFlag;				//用于确认是否从动作列表中开启此动作了。
	uint32_t ConfigurationID;
	char*    Name;
	uint32_t TargetValue;
	uint8_t  HighestTargetValueAdjust;
	
	bool     Active;									//是否触发剔除
	uint16_t DigitalOutput;						//剔除信号输出到哪个IO
	uint16_t PushLength;							//剔除信号长度
	uint8_t  PushLine;								//隶属于哪条剔除线
	uint8_t  MinResultNum;						//最少结果数
};

//执行动作
//////////
struct ActionInstance
{
	struct propActionRequestMachineData Item_ActionRequestMachineData;
	struct propActionSetOutput 					Item_ActionSetOutput;
	struct propActionObjectTakeOver 		Item_ActionObjectTakeOver;
	struct propActionTriggerCamera 			Item_ActionTriggerCamera;
	struct propActionTriggerSensor 			Item_ActionTriggerSensor;
	struct propActionPushOut 						Item_ActionPushOut;	
};

//跟踪段具体配置
struct ModuleConfiguration				//跟踪段设置
{
	uint32_t ConfigurationID;				//配置识别ID
	char*    Name;									//跟踪段名字
	uint8_t  ModuleID;							//此段跟踪段ID
	uint8_t  LineID;								//产线编号ID
	bool 		 CreateObjectFlag;			//是否创建对象标志
	
	uint16_t ModuleTrigger;					//模块触发信号来源，即由哪个光电提供触发信号
	uint8_t  ModuleTriggerEdge;			//光电信号的触发沿，上升沿OR下降沿
	uint8_t  HighActive;						//触发电平，高/低电平
	
	uint16_t Encoder;								//编码器编号，使用哪个编码器
	uint8_t  EncoderSignal;					//编码器信号，一般无需设置，为0
	uint8_t  EncoderEdge;						//编码器信号沿，（上升/下降沿计数）
	/*Tracking 设置*/
	uint16_t TrackingWindow;				//!!!跟踪窗口，两个跟踪段对象在多少个EncoderCounts后到达，即上一个跟踪模块与下一个跟踪模块之间得编码器允许差值范围！！！
	uint16_t DoubleTrigger;					//双触发保护，作用于光电，即DoubleTrigger内触发两次则认为重复触发，仅计算一次
	uint16_t Debounce;							//信号防抖，小于Debounce个编码器器值即认为有干扰误触发
	
	uint16_t HighestSpeed;					//最高速度补偿
	uint16_t LowestSpeed;						//最低速度补偿
	
	uint16_t ObjectWidth;						//对象宽度，一般默认为45
	uint16_t ObjectInterval;				//对象间隔，一般默认为60
	uint16_t NumberOfActions;				//动作个数	
	struct ActionInstance ActionInstanceConfig[maxActionSingleModule];		//定义此段跟踪段的对象
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////remain to be complement.../////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////外部使用接口/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
extern struct TrackingConfiguration TrackingConfig;											//定义一个所用跟踪段共用的设置，包含所有跟踪段的共用数据
extern struct ModuleConfiguration		ModuleConfig[maxTrackingModule];		//定义跟踪段缓冲数组，每个元素含有一个跟踪段的信息和本跟踪段的所有数据
//extern struct ActionInstance						ActionInstanceConfig[maxActionSingleModule];	//定义单个跟踪段允许的最大动作数量
extern uint8_t Module_Count;

uint8_t ParametersLoadTask_init(void);

#endif

