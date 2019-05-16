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
	byte Module_i = 0, Action_i = 0;			//�������Ʊ���
	byte object_i = 0;
	
	static __IO int64_t encoderNumber = 0;     					// ����������ֵ
	static __IO int64_t encoder1Number = 0;
	static __IO int64_t encoder2Number = 0;
	static __IO int64_t encoderDelivered = 0;
	
	static uint16_t timeCountTime = 0;
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
	uint16_t timeCount = 0;
	BaseType_t err;

	moduleQueueTemp = mymalloc(SRAMEX, sizeof(ModuleQueueItem));//�����ڴ��ʼ��

	
/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����******************************************************************************				
*************************************************************************************************************************************************************/	
	while(1)
	{	
//		TimeCountStart = xTaskGetTickCount();
		timeCountTime 	= __HAL_TIM_GET_COUNTER(&TIM6_Handler);//0.7us
		
		if( ClientNum > 0)															//�м�����Inspection����
		{
			encoder1Number = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);//1.5us
			encoder2Number = (OverflowCount_Encoder8*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder8);//1.5us
			
			for(Module_i = 0;Module_i < Module_Count;Module_i++)
			{
				switch (ModuleConfig[Module_i].Encoder)
				{
					case Encoder_1: encoderNumber = encoder1Number;break;
					case Encoder_2: encoderNumber = encoder2Number;break;
				}

				for(Action_i = 0; Action_i < maxTrackingObjects;Action_i++)//����ȽϺ�ʱ
				{
					if(ObjectInModuleList[Module_i][Action_i].IsActionAlive)
					{
						if(encoderNumber > ObjectInModuleList[Module_i][Action_i].TargetValue)
						{	
							isActionIn = true;
							
							switch(ObjectInModuleList[Module_i][Action_i].ActionType)		//13us
							{
								case ActRequestMachineData: 							
									//printf("ActRequestMachineData Triggered, Object[%d], Encoder=[%lld] \n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								break;
								
								case ActSetOutput					: 							
									//printf("ActSetOutput Triggered, Object[%d], Encoder=[%lld]\n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerOutput, eSetValueWithOverwrite, NULL);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								break;
								
								case ActObjectTakeOver		:								
									//printf("ActObjectTakeOver Triggered, Object[%d], Encoder=[%lld]\n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									
									pTempObjectTakeOver = (propActionObjectTakeOver*)ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].pActionConfig;
									switch (ModuleConfig[pTempObjectTakeOver->DestinationModule].Encoder)
									{
										case Encoder_1: encoderDelivered = encoder1Number;break;
										case Encoder_2: encoderDelivered = encoder2Number;;break;
									}
									moduleQueueTemp->DelieverdEncoderNum 	= encoderDelivered;
									moduleQueueTemp->DelieverdObjectID		=	ObjectInModuleList[Module_i][Action_i].ObjectID;			
									//moduleQueueTemp->ClientID							= 
									
									err = xQueueSend(ModuleQueue[pTempObjectTakeOver->DestinationModule], moduleQueueTemp, 0);		//
									if(err==errQUEUE_FULL) printf("Queue is Full Send Failed!\r\n");
									
									pPacket = CreateObjectRunOutPacket(1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber, pTempObjectTakeOver->DestinationModule);
									TCPSendPacket(ClientServer, pPacket);
									//printf("ModuleCount==%d,  Module[%d]===>Destination[%d] \n", Module_Count, Module_i, pTempObjectTakeOver->DestinationModule);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
									break;
								
								case ActTriggerCamera			: 						
									//printf("ActTriggerCamera Triggered, Object[%d], Encoder=[%lld]\n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									pTempTriggerCamera = (propActionTriggerCamera*)ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].pActionConfig;
									xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerCamera, eSetValueWithOverwrite, NULL);	//����֪ͨȥ�������
									
									pPacket = CreateTriggerCameraPacket(1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber,
																											pTempTriggerCamera->CameraID, 1);
									TCPSendPacket(pTempTriggerCamera->ClientID, pPacket);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								break;
								
								case ActTriggerSensor			: 
									pTempTriggerSensor = (propActionTriggerSensor*)ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].pActionConfig;
									//printf("ActTriggerSensor Triggered, Object[%d], Encoder=[%lld]\n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerSensor, eSetValueWithOverwrite, NULL);	
								
									pPacket = CreateTriggerIOSensorPacket(1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber, pTempTriggerSensor->SensorID);
									TCPSendPacket(pTempTriggerSensor->ClientID, pPacket);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
									break;
								
								case ActPushOut						: 						
									//printf("ActPushOut Triggered, Object[%d], Encoder=[%lld]\n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									pPacket = CreateObjectDeletePacket(1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber);																					
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
			
				if(isActionIn == true)	//�����������Moduleѭ��
				{
					isActionIn = false;
					break;
				}
			
			}
/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����END******************************************************************************				
*************************************************************************************************************************************************************/				

//			timeCountTime 	= __HAL_TIM_GET_COUNTER(&TIM6_Handler) - timeCountTime;		//11us
//			if(timeCountTime > 300)
//			{
//				printf("TrackingTime ==>%d \n", timeCountTime);
//			}
			
			
//			timeCount = xTaskGetTickCount()- TimeCountStart;
//			if(timeCount > 0)
//			{
//				printf("Tracking Thread Running Time ==>%d \n", timeCount);//
//			}
		}
	vTaskDelay(1);																								//������1ms�Ļ�����һֱ���������	
	}	
}

