/**************************************************************************************************
**************************���������߳�******************************************************
***************************************************************************************************/
#ifndef __PARAMETERSLOAD_H
#define __PARAMETERSLOAD_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "ff.h"											//FATFS�ļ�ϵͳ����
#include "exfuns.h"									//FATFS��չ����
#include "sdmmc_sdcard.h"						//SD������
#include "sdram.h"									//SDRAM����
#include "ezxml.h"
#include "MyList.h"									//���ڴ������ٶ��б�

#define maxTrackingModule  8 				//��������������ٶ�����
#define maxActionSingleModule  15 				//������������������

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////�ڲ�ʹ�ýӿ�/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////��������������ͽṹ
//���ٶ�ͨ������
struct TrackingConfiguration
{
	uint32_t ConfigurationID;				//����ʶ��ID
	uint8_t  HostClientID;					//������ͻ���ID
	uint8_t  SimClientID;						//����ͻ���ID
	uint16_t Timer;									//ʱ��
	
	uint8_t  ReadySignal;						//�����źţ���IO���
	uint8_t  ErrorSignal;						//�����źţ���IO���
	uint8_t  MessageSignal;					//��Ϣ�ź��źţ���IO���
	uint8_t  WarningSignal;					//�����źţ���IO���
};
///////��������ActionList
//ѯ�ʻ������ݶ���
struct propActionRequestMachineData
{
	bool			ActionTriggerFlag;				//����ȷ���Ƿ�Ӷ����б��п����˶����ˡ�
	uint32_t ConfigurationID;
	char* 	 Name;									//����
	uint32_t TargetValue;						//Ŀ��ֵ�������������ĸ�ֵ�����˶�����
	uint16_t HighestTargetValueAdjust;//���ٲ���ֵ
};
//�����������
struct propActionSetOutput
{
	bool			ActionTriggerFlag;				//����ȷ���Ƿ�Ӷ����б��п����˶����ˡ�
	uint32_t ConfigurationID;
	char* 	 Name;
	uint32_t TargetValue;						//Ŀ��ֵ�������������ĸ�ֵ�����˶�����
	uint16_t HighestTargetValueAdjust;
	
	uint16_t DigitalOutput;					//�ź�������ĸ�IO
	uint16_t OutputDuration;				//��or�͵�ƽ�źŵĳ���ʱ��
	bool     OutputInvert;					//�źŷ�ת�����ߵ͵�ƽ��ת�Ƿ�
};
//���ݶ���
struct propActionObjectTakeOver
{
	bool			ActionTriggerFlag;				//����ȷ���Ƿ�Ӷ����б��п����˶����ˡ�
	uint32_t ConfigurationID;
	char* 	 Name;
	uint32_t TargetValue;
	uint8_t  HighestTargetValueAdjust;
	uint8_t  DestinationModule;			 //���ݶ����ĸ����ٶ�
};
//�����������
struct propActionTriggerCamera
{
	bool			ActionTriggerFlag;				//����ȷ���Ƿ�Ӷ����б��п����˶����ˡ�
	uint32_t ConfigurationID;
	char* 	 Name;
	uint32_t TargetValue;							//Ŀ��ֵ
	uint8_t  HighestTargetValueAdjust;
	
	uint8_t  ClientID;								//������ID���������ĸ�Inspector����eg:192.168.66.11
	uint16_t CameraID;								//���ID��ʹ���ĸ����
	uint8_t  ImageIndex;							//ͼ����ţ�������ĵڼ���ͼ
	uint16_t DigitalOutput;						//��������ź�������ĸ�IO
	uint16_t OutputDuration;					//�����źų���ʱ��
};
//����IO�ɼ�����
struct propActionTriggerSensor
{
	bool			ActionTriggerFlag;				//����ȷ���Ƿ�Ӷ����б��п����˶����ˡ�
	uint32_t ConfigurationID;
	char* 	 Name;
	uint32_t TargetValue;							//Ŀ��ֵ
	uint8_t  HighestTargetValueAdjust;
	
	uint8_t  ClientID;								//������ID���������ĸ�Inspector����eg:192.168.66.11
	uint8_t  SensorID;								//������ID��ʹ���ĸ��������ɼ�
};
//�޳�����
struct propActionPushOut
{
	bool			ActionTriggerFlag;				//����ȷ���Ƿ�Ӷ����б��п����˶����ˡ�
	uint32_t ConfigurationID;
	char*    Name;
	uint32_t TargetValue;
	uint8_t  HighestTargetValueAdjust;
	
	bool     Active;									//�Ƿ񴥷��޳�
	uint16_t DigitalOutput;						//�޳��ź�������ĸ�IO
	uint16_t PushLength;							//�޳��źų���
	uint8_t  PushLine;								//�����������޳���
	uint8_t  MinResultNum;						//���ٽ����
};

//ִ�ж���
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

//���ٶξ�������
struct ModuleConfiguration				//���ٶ�����
{
	uint32_t ConfigurationID;				//����ʶ��ID
	char*    Name;									//���ٶ�����
	uint8_t  ModuleID;							//�˶θ��ٶ�ID
	uint8_t  LineID;								//���߱��ID
	bool 		 CreateObjectFlag;			//�Ƿ񴴽������־
	
	uint16_t ModuleTrigger;					//ģ�鴥���ź���Դ�������ĸ�����ṩ�����ź�
	uint8_t  ModuleTriggerEdge;			//����źŵĴ����أ�������OR�½���
	uint8_t  HighActive;						//������ƽ����/�͵�ƽ
	
	uint16_t Encoder;								//��������ţ�ʹ���ĸ�������
	uint8_t  EncoderSignal;					//�������źţ�һ���������ã�Ϊ0
	uint8_t  EncoderEdge;						//�������ź��أ�������/�½��ؼ�����
	/*Tracking ����*/
	uint16_t TrackingWindow;				//!!!���ٴ��ڣ��������ٶζ����ڶ��ٸ�EncoderCounts�󵽴����һ������ģ������һ������ģ��֮��ñ����������ֵ��Χ������
	uint16_t DoubleTrigger;					//˫���������������ڹ�磬��DoubleTrigger�ڴ�����������Ϊ�ظ�������������һ��
	uint16_t Debounce;							//�źŷ�����С��Debounce����������ֵ����Ϊ�и����󴥷�
	
	uint16_t HighestSpeed;					//����ٶȲ���
	uint16_t LowestSpeed;						//����ٶȲ���
	
	uint16_t ObjectWidth;						//�����ȣ�һ��Ĭ��Ϊ45
	uint16_t ObjectInterval;				//��������һ��Ĭ��Ϊ60
	uint16_t NumberOfActions;				//��������	
	struct ActionInstance ActionInstanceConfig[maxActionSingleModule];		//����˶θ��ٶεĶ���
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////remain to be complement.../////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////�ⲿʹ�ýӿ�/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
extern struct TrackingConfiguration TrackingConfig;											//����һ�����ø��ٶι��õ����ã��������и��ٶεĹ�������
extern struct ModuleConfiguration		ModuleConfig[maxTrackingModule];		//������ٶλ������飬ÿ��Ԫ�غ���һ�����ٶε���Ϣ�ͱ����ٶε���������
//extern struct ActionInstance						ActionInstanceConfig[maxActionSingleModule];	//���嵥�����ٶ���������������
extern uint8_t Module_Count;

uint8_t ParametersLoadTask_init(void);

#endif

