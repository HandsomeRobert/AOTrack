/**************************************************************************************************
**************************�����߳�************************************************************
***************************************************************************************************/
#ifndef __TRACKING_H
#define __TRACKING_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "bsp_encoder.h"						//	������	
#include "LightSignal.h"						//���
#include "led.h"										//��LED��ģ�ⴥ��
#include "DataTransferManage.h"
#include "TcpPacketServer.h"
#include "ParametersLoad.h"					//�������ò���

struct ObjectTracking 
{
	byte 	ClientID;
	short ObjectID;
	bool 	CreateObjectFlag;						//default = false
	__IO 	int64_t BornEncoderNum;					//64Bit���洢������ı�����ֵ
	byte 	CameraID;
	char 	*pData;
	bool  ProcessedResult;

	/***���ݶ�����ʹ�õı�־λ***/
	bool	objectDelieverdFlag;				//���ڱ�Ǵ˶����Ƿ񱻴��ݳ�ȥ��
	byte	objectModulePosition;							//��¼�������ڵĸ��ٶ�
	//byte	delieverDestination;				//��¼������Ҫ���ݵ��ĸ��ٶ
	__IO 	int64_t deliverEncoderNum;	//���ݶ����ı�����ֵ
	
	bool 	objectAliveFlag;																							//�����ж϶����Ƿ�����˸��ٹ��̣��Ƿ�������
	//bool	modulePosition[maxTrackingModule];														//����һ�����ٶ����ڸ��ٶε�λ�ã�����������������������
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!modulePosition ��objectPostition���ظ����壬�ǵ��޸�
	bool 	triggerPosition[maxTrackingModule][maxActionSingleModule];		//����һ�����ڼ�¼���ٶ����ڶ�Ӧ��λ���Ƿ񱻴����Ķ�ά����
	
	__IO 	int64_t ModuleReferenceEncoderNum[maxTrackingModule];					//�������ÿ�����ٶζ���һ���������ο�ֵ

};

extern __IO int64_t EncoderNumber;

#define maxTrackingObjects  64			//���ά�����ٸ����ٶ���
//#define defaultPositionRange 3		//Ĭ�������ﶯ��ִ�д��ı�������ֵƫ��
#define Message_TrrigerCamera 1			//������Ӧ����Ϣֵ
#define Message_TrrigerSensor 2
#define Message_TrrigerOutput 3
#define Message_TrrigerPush 	4


uint8_t TrackingTask_init(void);

#endif

