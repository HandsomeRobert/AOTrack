/**************************************************************************************************
**************************�����߳�************************************************************
***************************************************************************************************/
#ifndef __TRACKING_H
#define __TRACKING_H

#include "TaskManage.h"
#include "HardwareList.h"						//Ӳ����Դ���Ŷ���
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "bsp_encoder.h"						//	������	
#include "LightSignal.h"					//���
//#include "led.h"									//��LED��ģ�ⴥ��
#include "DataTransferManage.h"
#include "TcpPacketServer.h"				//���ClientNum
#include "ParametersLoad.h"					//�������ò���

//#define defaultPositionRange 3		//Ĭ�������ﶯ��ִ�д��ı�������ֵƫ��
#define Message_TrrigerCamera 1			//������Ӧ����Ϣֵ
#define Message_TrrigerSensor 2
#define Message_TrrigerOutput 3
#define Message_TrrigerPush 	4

#define maxTrackingObjects   	64			//���ά�����ٸ����ٶ���			
#define moduleQueueDepth			10			//ÿ������ģ�����Ϣ���г���

#define	NoOutput							0
#define	TypeOutputDigital			1
#define	TypeOutputResult			2

typedef struct 
{
	byte 	ClientID;
	byte	ModuleID;
	__IO 	uint32_t ObjectID;
	__IO 	int64_t BornEncoderNum;			//64Bit���洢������ı�����ֵ
	char 	*pData;
	bool  ProcessedResult;
	
	bool 	objectAliveFlag;						//�����ж϶����Ƿ�����˸��ٹ��̣��Ƿ�������
}ObjectInfo;


typedef struct
{
	__IO 	uint32_t ObjectID;
//	__IO 	int64_t RefEncoderNum;			//64Bit���洢������ı�����ֵ
	__IO 	int64_t TargetValue;				//Ŀ��ֵ
	enum enumActionType	ActionType;									//��������
	byte  ActionNumber;								//�˶����Ǹ��ٶ��ϵĵڼ�������
	byte	OutputType;									//�������
	byte	OutputChannel;							//���ͨ��
	bool	IsActionAlive;
}StctActionListItem;


typedef struct
{
	__IO 	uint32_t 	DelieverdObjectID;				
	__IO 	int64_t 	DelieverdEncoderNum;
//	int ClientID;
	
}ModuleQueueItem;

typedef struct
{
	byte actionType;
	void* pAction;
}ActionExecuteQueueItem;

extern QueueHandle_t	ModuleQueue[maxTrackingModule];				//������Ϣ�������ڽ�����Ϣ
extern QueueHandle_t  ActionExecuteQueue;								//��������ִ�е���Ϣ���ն���
extern __IO ObjectInfo ObjectBuffer[maxTrackingObjects];	  //�������ֻ�ܱ���maxTrackingObjects����������� 
uint8_t TrackingTask_init(void);

#endif

