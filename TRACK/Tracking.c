/**************************************************************************************************
*************************************跟踪线程******************************************************
***************************************************************************************************/
#include "Tracking.h"
#include <math.h>
#include "TCPProtocol.h"

extern uint32_t Timer;			//使用主函数定义的仿真Timer

TaskHandle_t TrackingThread_Handler;
static void TrackingThread(void *arg);

char* strCreateObject 	 				 	= "<=================Detect someting Create Object===================>";
char* strActionTriggerCamera 			= "<<<<<<<<<<<<Successfully Trigger Camera!!!>>>>>>>>>>>>>>>>>";
char* strActionTriggerSensor 			= "<<<<<<<<<<<<Successfully Trigger Sensor!!!>>>>>>>>>>>>>>>>>";
char* strActionPushOut 			 			= "<<<<<<<<<<<<Successfully Trigger Pusher!!!>>>>>>>>>>>>>>>>>";
char* strActionRequestMachineData = "<<<<<<<<<<<<Successfully RequestMachineData!!!>>>>>>>>>>>>>>>>>";
char* strActionSetOutput 					= "<<<<<<<<<<<<Successfully ActionSetOutput!!!>>>>>>>>>>>>>>>>>";
char* strActionObjectTakeOver 		= "<<<<<<<<<<<<Successfully ActionObjectTakeOver!!!>>>>>>>>>>>>>>>>>";



//创建跟踪任务
//返回值:0 跟踪任务创建成功
//		其他 跟踪任务创建失败
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

//Tracking跟踪线程
static void TrackingThread(void *arg)
{	
	byte Module_i = 0, Action_i = 0;			//遍历控制变量
	byte object_i = 0;
	
	static __IO int64_t encoderNumber = 0;     					// 编码器计数值
	static __IO int64_t encoder1Number = 0;
	static __IO int64_t encoder2Number = 0;
	static __IO int64_t encoderDelivered = 0;
	

	static ModuleQueueItem* moduleQueueTemp;						//定义一个往队列中填充数据的暂放指针
	static Packet* pPacket;															//数据包首地址
	uint32_t TimeCountStart = 0;												//用于计算线程运行时间
	uint16_t timeCount = 0;
	BaseType_t err;

	moduleQueueTemp = mymalloc(SRAMEX, sizeof(ModuleQueueItem));//分配内存初始化

	
/************************************************************************************************************************************************************			
*****************************************************************跟踪过程监控区******************************************************************************				
*************************************************************************************************************************************************************/	
	while(1)
	{	
		TimeCountStart = xTaskGetTickCount();
		
		if( ClientNum > 0)															//有检测程序Inspection连接
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

				for(Action_i = 0; Action_i < maxTrackingObjects;Action_i++)
				{
					if(ObjectInModuleList[Module_i][Action_i].IsActionAlive)
					{
						if(encoderNumber > ObjectInModuleList[Module_i][Action_i].TargetValue)
						{
							switch(ObjectInModuleList[Module_i][Action_i].ActionType)
							{
								case ActRequestMachineData: 							printf("ActRequestMachineData Triggered, Object[%d], Encoder=[%lld] \n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								break;
								
								case ActSetOutput					: 							printf("ActSetOutput Triggered, Object[%d], Encoder=[%lld]\n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerOutput, eSetValueWithOverwrite, NULL);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								break;
								
								case ActObjectTakeOver		:								printf("ActObjectTakeOver Triggered, Object[%d], Encoder=[%lld]\n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									switch (ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].Item_ActionObjectTakeOver.DestinationModule].Encoder)
									{
										case Encoder_1: encoderDelivered = encoder1Number;break;
										case Encoder_2: encoderDelivered = encoder2Number;;break;
									}
									moduleQueueTemp->DelieverdEncoderNum 	= encoderDelivered;
									moduleQueueTemp->DelieverdObjectID		=	ObjectInModuleList[Module_i][Action_i].ObjectID;			
									
									err = xQueueSend(ModuleQueue[ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].Item_ActionObjectTakeOver.DestinationModule], moduleQueueTemp, 0);		//?????????????????,???????ID	
									if(err==errQUEUE_FULL) printf("Queue is Full Send Failed!\r\n");
									
									printf("ModuleCount==%d,  Module[%d]===>Destination[%d] \n", Module_Count, Module_i, ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].Item_ActionObjectTakeOver.DestinationModule);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
									break;
								
								case ActTriggerCamera			: 						printf("ActTriggerCamera Triggered, Object[%d], Encoder=[%lld]\n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerCamera, eSetValueWithOverwrite, NULL);	//发送通知去触发相机
									pPacket = CreateTriggerCameraPacket(1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, ModuleConfig[Module_i].Encoder,
																											ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].Item_ActionTriggerCamera.CameraID, 1);
									TCPSendPacket(ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].Item_ActionTriggerCamera.ClientID, pPacket);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								break;
								
								case ActTriggerSensor			: 						printf("ActTriggerSensor Triggered, Object[%d], Encoder=[%lld]\n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerSensor, eSetValueWithOverwrite, NULL);	
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
									break;
								
								case ActPushOut						: 						printf("ActPushOut Triggered, Object[%d], Encoder=[%lld]\n", ObjectInModuleList[Module_i][Action_i].ObjectID, encoderNumber);
									pPacket = CreateObjectDeletePacket(1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, ModuleConfig[Module_i].Encoder);																					
									/******遍历寻找此对象在缓冲区数组的位置， 此处亦须释放对应的ObjectBuffer对象，否则无法再创建对象，无法往ObjectBuffer内填充值*****/
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
										xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerPush, 	 eSetValueWithOverwrite, NULL);//发送通知去触发剔除																							
									}
									ObjectBuffer[object_i].objectAliveFlag = false;									//释放被占有的对象
									
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;		//释放占有的动作
									break;
								
								default:break;
							}
						}
					}
				}			
			}
/************************************************************************************************************************************************************			
*****************************************************************跟踪过程监控区END******************************************************************************				
*************************************************************************************************************************************************************/				
			timeCount = xTaskGetTickCount()- TimeCountStart;
			if(timeCount > 0)
			{
				printf("Tracking Thread Running Time ==>%d \n", timeCount);//
			}
		}
	vTaskDelay(1);																								//不阻塞1ms的话，会一直跑这个程序	
	}	
}

