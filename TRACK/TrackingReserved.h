/**************************************************************************************************
**************************�����߳�************************************************************
***************************************************************************************************/
#ifndef __TRACKING_H
#define __TRACKING_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "bsp_encoder.h"						//	������	
//#include "LightSignal.h"					//���
//#include "led.h"									//��LED��ģ�ⴥ��
#include "DataTransferManage.h"
#include "TcpPacketServer.h"				//���ClientNum
#include "ParametersLoad.h"					//�������ò���
#include "ObjectDetection.h"				//���󴴽�����

//#define defaultPositionRange 3		//Ĭ�������ﶯ��ִ�д��ı�������ֵƫ��
#define Message_TrrigerCamera 1			//������Ӧ����Ϣֵ
#define Message_TrrigerSensor 2
#define Message_TrrigerOutput 3
#define Message_TrrigerPush 	4

uint8_t TrackingTask_init(void);

#endif

