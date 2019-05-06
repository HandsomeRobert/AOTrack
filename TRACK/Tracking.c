/**************************************************************************************************
*************************************�����߳�******************************************************
***************************************************************************************************/
#include "Tracking.h"
#include <math.h>

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
	byte Module_i = 0;			//�������Ʊ���
	byte object_i = 0;
	static __IO int64_t encoderNumber = 0;     					// ����������ֵ
	static __IO int64_t encoder1Number = 0;
	static __IO int64_t encoder2Number = 0;
	static __IO int64_t encoderDelivered = 0;
	
	static ObjectList pActionList;
	static ObjectList pActionNextTemp;											//�洢ɾ�����֮ǰ��ֵ
	static ModuleQueueItem* moduleQueueTemp;						//����һ����������������ݵ��ݷ�ָ��
	uint32_t TimeCountStart = 0;												//���ڼ����߳�����ʱ��
	uint16_t timeCount = 0;
	BaseType_t err;

	moduleQueueTemp = mymalloc(SRAMEX, sizeof(ModuleQueueItem));//�����ڴ��ʼ��
	pActionList			= mymalloc(SRAMEX, sizeof(struct LNode));//�����ڴ��ʼ��
	
/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����******************************************************************************				
*************************************************************************************************************************************************************/	
	while(1)
	{	
		TimeCountStart = xTaskGetTickCount();
		
		if( ClientNum > 0)															//�м�����Inspection����
		{
			encoder1Number = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
			encoder2Number = (OverflowCount_Encoder8*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder8);
			
			for(Module_i = 0;Module_i < Module_Count;Module_i++)
			{
				switch (ModuleConfig[Module_i].Encoder)
				{
					case Encoder_1: encoderNumber = encoder1Number;break;
					case Encoder_2: encoderNumber = encoder2Number;break;
				}
				
				pActionList = ObjectInModuleList[Module_i]->next;  		//��ObjectInModuleList[Module_i]Ϊβָ�룬ҲΪ��ͷ��㣩->nextָ���һ��Ԫ��
				while (pActionList != ObjectInModuleList[Module_i])  	//pModuleListδ����ͷ
				{
					//printf("Module_PositionTest222===> Module[%d] \n", Module_i);
					
					if(encoderNumber > pActionList->Object.TargetValue)			//�ж϶����Ƿ񵽴�ָ��λ��
					{	
						object_i = 0;					
						printf("Object[%d], ActionTrigger Position is : %lld \n", pActionList->Object.ObjectID, encoderNumber);//ͷ���Ϊ�գ����Ի����һ��...
						//xTaskSuspendAll()();
						switch (pActionList->Object.ActionType)
						{
							case TypeActionTriggerCamera:		xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerCamera, eSetValueWithOverwrite, NULL);	//����֪ͨȥ�������
																							TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionTriggerCamera);//֪ͨPC�Ѿ����������
																							pActionNextTemp = pActionList->next;
																							vTaskSuspendAll();		//�����ٽ�����ֹ�����
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//�Ӹ��ٶ��б���ɾ��ִ���˵Ķ���
																							xTaskResumeAll();		//�˳��ٽ���
																							break;
							
							case TypeActionObjectTakeOver: 				
																							TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionObjectTakeOver);//֪ͨPC�Ѿ������˶���
																							switch (ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionObjectTakeOver.DestinationModule].Encoder)
																							{
																								case Encoder_1: encoderDelivered = encoder1Number;break;
																								case Encoder_2: encoderDelivered = encoder2Number;;break;
																							}
																							moduleQueueTemp->DelieverdEncoderNum 	= encoderDelivered;
																							moduleQueueTemp->DelieverdObjectID		=	pActionList->Object.ObjectID;			
																							
																							err = xQueueSend(ModuleQueue[ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionObjectTakeOver.DestinationModule], moduleQueueTemp, 0);		//����Ҫ���ݵ��ĸ��ٶ������з������ݣ��������ݺͶ���ID	
																							if(err==errQUEUE_FULL) printf("Queue is Full Send Failed!\r\n");
																							pActionNextTemp = pActionList->next;	//�����Ƚ�pActionListָ����һ��Ԫ�أ���Ȼ���ͷ�pActionList�Ļ��ᵼ��pActionList���ѱ�ɾ��������ָ��next																										
																							vTaskSuspendAll();		//�����ٽ�����ֹ�����
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//�Ӹ��ٶ��б���ɾ��ִ���˵Ķ���
																							printf("ModuleCount==%d,  Module[%d]===>Destination[%d] \n", Module_Count, Module_i, ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionObjectTakeOver.DestinationModule);
																							xTaskResumeAll();			//�˳��ٽ���
																							break;
																										
							case TypeActionSetOutput:							
																							xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerOutput, eSetValueWithOverwrite, NULL);
																							TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionSetOutput);//֪ͨPC�Ѿ����������
																							pActionNextTemp = pActionList->next;
																							vTaskSuspendAll();		//�����ٽ�����ֹ�����
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//�Ӹ��ٶ��б���ɾ��ִ���˵Ķ���
																							xTaskResumeAll();		//�˳��ٽ���
																							break;//����֪ͨȥ�������
																										
							case TypeActionTriggerSensor:		xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerSensor, eSetValueWithOverwrite, NULL);	
																							TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionTriggerSensor);//֪ͨPC�Ѿ��������źŲɼ�
																							pActionNextTemp = pActionList->next;
																							vTaskSuspendAll();		//�����ٽ�����ֹ�����
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//�Ӹ��ٶ��б���ɾ��ִ���˵Ķ���
																							xTaskResumeAll();		//�˳��ٽ���
																							break;//����֪ͨȥ�����������ɼ�
																										
							case TypeActionPushOut:					TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionPushOut);//֪ͨPC�Ѿ����������
																																												
																							/******����Ѱ�Ҵ˶����ڻ����������λ�ã� �˴������ͷŶ�Ӧ��ObjectBuffer���󣬷����޷��ٴ��������޷���ObjectBuffer�����ֵ*****/
																							while(ObjectBuffer[object_i].ObjectID != pActionList->Object.ObjectID)
																							{
																								object_i++;
																								if(object_i > (maxTrackingObjects - 1)) 
																								{
																									printf("Cannot Find The consistent ObjectID in ObjectBuffer failed. \n");
																									break;
																								}	
																							}
																							if(ObjectBuffer[object_i].ProcessedResult == false)	
																							{
																								xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerPush, 	 eSetValueWithOverwrite, NULL);//����֪ͨȥ�����޳�																							
																							}
																							
																							pActionNextTemp = pActionList->next;
																							vTaskSuspendAll();		//�����ٽ�����ֹ�����
																							ObjectBuffer[object_i].objectAliveFlag = false;					//�ͷű�ռ�еĶ���
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//�Ӹ��ٶ��б���ɾ��ִ���˵Ķ���
																							xTaskResumeAll();		//�˳��ٽ���
																							break;
																										
							case TypeActionRequestMachineData:	
																							TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionRequestMachineData);//֪ͨPC�Ѿ������˻�������
																							pActionNextTemp = pActionList->next;
																							vTaskSuspendAll();		//�����ٽ�����ֹ�����
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//�Ӹ��ٶ��б���ɾ��ִ���˵Ķ���
																							xTaskResumeAll();		//�˳��ٽ���
																							break;							
						}	
					pActionList = pActionNextTemp;					//��һ��ѭ������pActionNextTemp = pActionList->next������ɾ��ǰ��ָ�����һ�����,��ΪpActionList�Ѵ��б���ɾ������ȥ����ָ����һ��Ԫ�ؾͻ����
					}
					else
						pActionList = pActionList->next;
					//xTaskResumeAll();
				}																		
			}
/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����END******************************************************************************				
*************************************************************************************************************************************************************/				
			timeCount = xTaskGetTickCount()- TimeCountStart;
			if(timeCount > 0)
			{
				printf("Tracking Thread Running Time ==>%d \n", timeCount);//
			}
		}
	vTaskDelay(1);																								//������1ms�Ļ�����һֱ���������	
	}	
}

