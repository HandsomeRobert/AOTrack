/**************************************************************************************************
*************************************�����߳�******************************************************
***************************************************************************************************/
#include "Tracking.h"
#include <math.h>
#include "TCPProtocol.h"
#include "timer.h"

extern uint32_t Timer;			//ʹ������������ķ���Timer

TaskHandle_t TrackingThread_Handler;
static void TrackingThread(void *arg);

char* strCreateObject 	 				 	= "<=================Detect someting Create Object===================>";
char* strActionTriggerCamera 			= "<<<<<<<<<<<<Successfully Trigger Camera!!!>>>>>>>>>>>>>>>>>";
char* strActionTriggerSensor 			= "<<<<<<<<<<<<Successfully Trigger Sensor!!!>>>>>>>>>>>>>>>>>";
char* strActionPushOut 			 			= "<<<<<<<<<<<<Successfully Trigger Pusher!!!>>>>>>>>>>>>>>>>>";
char* strActionRequestMachineData = "<<<<<<<<<<<<Successfully RequestMachineData!!!>>>>>>>>>>>>>>>>>";
char* strActionSetOutput 					= "<<<<<<<<<<<<Successfully ActionSetOutput!!!>>>>>>>>>>>>>>>>>";
char* strActionObjectTakeOver 		= "<<<<<<<<<<<<Successfully ActionObjectTakeOver!!!>>>>>>>>>>>>>>>>>";



//������������
//����ֵ:0 �������񴴽��ɹ�
//		���� �������񴴽�ʧ��
uint8_t TrackingTask_init(void)
{
	uint8_t res;
	taskENTER_CRITICAL();              
	res = xTaskCreate((TaskFunction_t)TrackingThread,
					(const char*  )"TracingTread",
					(uint16_t     )TRACKING_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )TRACKING_TASK_PRIO,
					(TaskHandle_t*)&TrackingThread_Handler);
	taskEXIT_CRITICAL();

	return res;
}

//Tracking�����߳�
static void TrackingThread(void *arg)
{	
	byte Module_i = 0, Action_i = 0;												//�������Ʊ���
	byte object_i = 0, i_cycle 	= 0;
	int data_len = 0;
	static __IO int64_t encoderNumber 		= 0;     					// ����������ֵ
	static __IO int64_t encoder1Number 		= 0;
	static __IO int64_t encoder2Number 		= 0;
	static __IO int64_t encoderDelivered 	= 0;
	
	static bool isActionIn = false;
//	static propActionRequestMachineData* 	pTempRequestMachineData = NULL;
//	static propActionSetOutput* 					pTempSetOutput = NULL;
	static propActionObjectTakeOver* 			pTempObjectTakeOver = NULL;
	static propActionTriggerCamera* 			pTempTriggerCamera = NULL;
	static propActionTriggerSensor* 			pTempTriggerSensor = NULL;
//	static propActionPushOut* 						pTempPushOut = NULL;

	static ModuleQueueItem* moduleQueueTemp;						//����һ����������������ݵ��ݷ�ָ��
	static Packet* pPacket;															//���ݰ��׵�ַ
	uint32_t TimeCountStart = 0;												//���ڼ����߳�����ʱ��
	uint32_t TimeCountStartRTOS=0;
	
	uint32_t TestTime1=0;
	uint32_t TestTime2=0;
	uint32_t TestTime3=0;
	uint32_t TestTime4=0;
	uint32_t TestTime5=0;
	
	
	uint16_t timeCount = 0;
	uint16_t timeCount_for_cycle 	= 0;
	uint16_t timeCountTime_Module = 0;
	BaseType_t err;
OverflowCount_TIM6 = 0;

	moduleQueueTemp = mymalloc(SRAMEX, sizeof(ModuleQueueItem));//�����ڴ��ʼ��
	pPacket 				= mymalloc(SRAMEX, 256);										//Ϊ���ݰ�����һ���̶���256�ֽڵ�������ʱ�洢��
	
/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����******************************************************************************				
*************************************************************************************************************************************************************/	
	while(1)
	{	
		TimeCountStart 	= OverflowCount_TIM6*0XFFFF +__HAL_TIM_GET_COUNTER(&TIM6_Handler);//0.7us
		TimeCountStartRTOS = xTaskGetTickCount();
		
		if( ClientNum > 0)															//�м�����Inspection����
		{
			encoder1Number = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);//15us
			encoder2Number = (OverflowCount_Encoder8*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder8);//15us

			for(Module_i = 0;Module_i < Module_Count;Module_i++)
			{ 
				switch (ModuleConfig[Module_i].Encoder)
				{
					case Encoder_1: encoderNumber = encoder1Number;break;
					case Encoder_2: encoderNumber = encoder2Number;break;
				}	
				
				for(Action_i = 0; Action_i < maxTrackingObjects/2; Action_i++)//����ȽϺ�ʱmaxTrackingObjects 64��1420us, 32: 
				{
					if(ObjectInModuleList[Module_i][Action_i].IsActionAlive)
					{
						if(encoderNumber > ObjectInModuleList[Module_i][Action_i].TargetValue)
						{	
							isActionIn = true;
							
							switch(ObjectInModuleList[Module_i][Action_i].ActionType)		//13us
							{
								case ActRequestMachineData: 							
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								break;
								
								case ActSetOutput					: 							
									xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerOutput, eSetValueWithOverwrite, NULL);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								break;
								
								case ActObjectTakeOver		:								
/*ConsumeTime:20us*/pTempObjectTakeOver = (propActionObjectTakeOver*)ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].pActionConfig;
								  switch (ModuleConfig[pTempObjectTakeOver->DestinationModule].Encoder)
									{
										case Encoder_1: encoderDelivered = encoder1Number;break;
										case Encoder_2: encoderDelivered = encoder2Number;;break;
									}
									moduleQueueTemp->DelieverdEncoderNum 	= encoderDelivered;
									moduleQueueTemp->DelieverdObjectID		=	ObjectInModuleList[Module_i][Action_i].ObjectID;			
									//moduleQueueTemp->ClientID							= 
									
									err = xQueueSend(ModuleQueue[pTempObjectTakeOver->DestinationModule], moduleQueueTemp, 0);		//
/*ConsumeTime:292usALL*/if(err==errQUEUE_FULL) printf("Queue is Full Send Failed!\r\n");
									//��Ҫ���͵����ݴ��ݵ�����buffer
									
									//1. ��ʱ1022us(��̬�����ڴ��)��2. 86us������pPacket�ģ�
/*ConsumeTime:96usALL*/pPacket = CreateObjectRunOutPacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber, pTempObjectTakeOver->DestinationModule);

									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;//��ʱ24us
									i_cycle = 0;
									while(Session[i_cycle].ClientID != ClientServer)
									{
										i_cycle++;
										if(i_cycle > ClientNum)
										{
											printf("Cannot find the session ClientServer[%d],Retrying...\r\n", ClientServer);
											i_cycle = 0;
										}
									}			
/*ConsumeTime:160usALL*/WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);
								
/*ConsumeTime:550usALL*/ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
									break;
								
								case ActTriggerCamera			: 						
/*ConsumeTime:32us*/pTempTriggerCamera = (propActionTriggerCamera*)ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].pActionConfig;

/*ConsumeTime:720us*/xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerCamera, eSetValueWithOverwrite, NULL);	//����֪ͨȥ�������

/*ConsumeTime:100us*/pPacket = CreateTriggerCameraPacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber,
																											pTempTriggerCamera->CameraID, 1);

									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4; /*ConsumeTime:160us*///��������WriteDataToBufferSend��ʱ160us
									i_cycle  = 0;
									while(Session[i_cycle].ClientID != ClientServer)//pTempTriggerCamera->ClientID
									{
										i_cycle++;
										if(i_cycle > ClientNum)
										{
											printf("Cannot find the session pTempTriggerCamera->ClientID[%d],Retrying...\r\n", pTempTriggerCamera->ClientID);
											i_cycle = 0;
										}
									}			
/*ConsumeTime:160us*/WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);
								
/*ALL ConsumeTime:1002us*/ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								break;
								
								case ActTriggerSensor			: /*ConsumeTime:938us*/
									pTempTriggerSensor = (propActionTriggerSensor*)ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].pActionConfig;
									xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerSensor, eSetValueWithOverwrite, NULL);	
								
									pPacket = CreateTriggerIOSensorPacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber, pTempTriggerSensor->SensorID);
								
									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
									i_cycle  = 0;
									while(Session[i_cycle].ClientID != ClientServer)//pTempTriggerCamera->ClientID
									{
										i_cycle++;
										if(i_cycle > ClientNum)
										{
											printf("Cannot find the session pTempTriggerSensor->ClientID[%d],Retrying...\r\n", pTempTriggerCamera->ClientID);
											i_cycle = 0;
										}
									}			
									WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);
				
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
									break;
								
								case ActPushOut						: /*ConsumeTime:256us*/				
									pPacket = CreateObjectDeletePacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber);				
								
									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
									i_cycle  = 0;
									while(Session[i_cycle].ClientID != ClientServer)
									{
										i_cycle++;
										if(i_cycle > ClientNum)
										{
											printf("Cannot find the session ClientServer->ClientID[%d],Retrying...\r\n", ClientServer);
											i_cycle = 0;
										}
									}			
									WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);							
								
									/******����Ѱ�Ҵ˶����ڻ����������λ�ã� �˴������ͷŶ�Ӧ��ObjectBuffer���󣬷����޷��ٴ��������޷���ObjectBuffer�����ֵ*****/
									object_i = 0 ;
									while(ObjectBuffer[object_i].ObjectID != ObjectInModuleList[Module_i][Action_i].ObjectID)
									{
										object_i++;
										if(object_i > (maxTrackingObjects - 1)) 
										{
											printf("Cannot Find The consistent ObjectID in ObjectBuffer failed In Tracking. \n");
											break;
										}	
									}
									if(ObjectBuffer[object_i].ProcessedResult == false)	
									{
										xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerPush, 	 eSetValueWithOverwrite, NULL);//����֪ͨȥ�����޳�																							
									}
									ObjectBuffer[object_i].objectAliveFlag = false;									//�ͷű�ռ�еĶ���
																		
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;		//�ͷ�ռ�еĶ���
									break;
								
								default:break;
							}
						break;
						}
					}
				}			
			
//printf("Get for whole List Cycle Time ==>%d \n", __HAL_TIM_GET_COUNTER(&TIM6_Handler) - timeCount_for_cycle);//����440usһ��ѭ����3��Ϊ1320us
				
				if(isActionIn == true)	//�����������Moduleѭ��
				{
					isActionIn = false;
					break;
				}
			
			}
//printf("Sending Module whole cycle Spending time  ==>%d \n", __HAL_TIM_GET_COUNTER(&TIM6_Handler) - timeCountTime_Module);//1378us
			

/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����END******************************************************************************				
*************************************************************************************************************************************************************/				

/***����ʱ��ͳ��***/
			
			TimeCountStart 	=  OverflowCount_TIM6*0XFFFF + __HAL_TIM_GET_COUNTER(&TIM6_Handler) - TimeCountStart;		//11us
			TimeCountStartRTOS = xTaskGetTickCount() - TimeCountStartRTOS;
			
//////			if(TimeCountStart > 800 && TimeCountStart<65536)//��֪Ϊ�λ���ִ���65536+TimeCountStart(����)�ļ���ֵ�������...
//////			{	
//////				printf("TrackingTime ==>%d \n", TimeCountStart);//No-Load:1430us   Load:1600
//////				printf("TrackingTimeRTOSSSS ==>%d \n", TimeCountStartRTOS);
//////			}	

		}
	vTaskDelay(1);																								//������1ms�Ļ�����һֱ���������	
	}	
}

