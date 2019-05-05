/**************************************************************************************************
**************************�����߳�************************************************************
***************************************************************************************************/
#ifndef __OBJECTDETECTION_H
#define __OBJECTDETECTION_H

#include "TaskManage.h"							//��������Դͳһ����λ��
#include "HardwareList.h"						//Ӳ����Դ���Ŷ���
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "bsp_encoder.h"						//	������	
#include "LightSignal.h"						//���
//#include "led.h"									//��LED��ģ�ⴥ��
#include "DataTransferManage.h"
#include "TcpPacketServer.h"				//���ClientNum
#include "ParametersLoad.h"					//�������ò���
#include "MyList.h"									//���ٶζ����б�

#define maxTrackingObjects  64			//���ά�����ٸ����ٶ���
#define ModuleQueueLength		20			//ÿ������ģ�����Ϣ���г���

typedef struct 
{
	byte 	ClientID;
	__IO 	uint32_t ObjectID;
	__IO 	int64_t BornEncoderNum;			//64Bit���洢������ı�����ֵ
	char 	*pData;
	bool  ProcessedResult;
	
	bool 	objectAliveFlag;						//�����ж϶����Ƿ�����˸��ٹ��̣��Ƿ�������
}ObjectInfo;

typedef struct
{
	__IO 	uint32_t 	DelieverdObjectID;				
	__IO 	int64_t 	DelieverdEncoderNum;	
	
}ModuleQueueItem;

extern ObjectList ObjectInModuleList[maxTrackingModule];								//����һ�����ٶ��б�ָ������
extern __IO ObjectInfo ObjectBuffer[maxTrackingObjects];										//�������ֻ�ܱ���maxTrackingObjects�����������
extern __IO	uint32_t GlobalObjectID;																	//����һ��ȫ�ֶ���ID��__IO��ʾֱ�Ӵӵ�ַ��ȡֵ��ȡ������ֵ�������������Ӳ���޸Ĵ�ֵ
extern QueueHandle_t	ModuleQueue[maxTrackingModule];				//������Ϣ�������ڽ�����Ϣ	

uint8_t ObjectDetectionTask_init(void);

#endif

