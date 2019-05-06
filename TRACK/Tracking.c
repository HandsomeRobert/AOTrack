/**************************************************************************************************
*************************************跟踪线程******************************************************
***************************************************************************************************/
#include "Tracking.h"
#include <math.h>

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
	byte Module_i = 0;			//遍历控制变量
	byte object_i = 0;
	static __IO int64_t encoderNumber = 0;     					// 编码器计数值
	static __IO int64_t encoder1Number = 0;
	static __IO int64_t encoder2Number = 0;
	static __IO int64_t encoderDelivered = 0;
	
	static ObjectList pActionList;
	static ObjectList pActionNextTemp;											//存储删掉结点之前的值
	static ModuleQueueItem* moduleQueueTemp;						//定义一个往队列中填充数据的暂放指针
	uint32_t TimeCountStart = 0;												//用于计算线程运行时间
	uint16_t timeCount = 0;
	BaseType_t err;

	moduleQueueTemp = mymalloc(SRAMEX, sizeof(ModuleQueueItem));//分配内存初始化
	pActionList			= mymalloc(SRAMEX, sizeof(struct LNode));//分配内存初始化
	
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
				
				pActionList = ObjectInModuleList[Module_i]->next;  		//（ObjectInModuleList[Module_i]为尾指针，也为表头结点）->next指向第一个元素
				while (pActionList != ObjectInModuleList[Module_i])  	//pModuleList未到表头
				{
					//printf("Module_PositionTest222===> Module[%d] \n", Module_i);
					
					if(encoderNumber > pActionList->Object.TargetValue)			//判断动作是否到达指定位置
					{	
						object_i = 0;					
						printf("Object[%d], ActionTrigger Position is : %lld \n", pActionList->Object.ObjectID, encoderNumber);//头结点为空，所以会进来一次...
						//xTaskSuspendAll()();
						switch (pActionList->Object.ActionType)
						{
							case TypeActionTriggerCamera:		xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerCamera, eSetValueWithOverwrite, NULL);	//发送通知去触发相机
																							TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionTriggerCamera);//通知PC已经触发了相机
																							pActionNextTemp = pActionList->next;
																							vTaskSuspendAll();		//进入临界区防止被打断
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//从跟踪段列表中删除执行了的动作
																							xTaskResumeAll();		//退出临界区
																							break;
							
							case TypeActionObjectTakeOver: 				
																							TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionObjectTakeOver);//通知PC已经传递了对象
																							switch (ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionObjectTakeOver.DestinationModule].Encoder)
																							{
																								case Encoder_1: encoderDelivered = encoder1Number;break;
																								case Encoder_2: encoderDelivered = encoder2Number;;break;
																							}
																							moduleQueueTemp->DelieverdEncoderNum 	= encoderDelivered;
																							moduleQueueTemp->DelieverdObjectID		=	pActionList->Object.ObjectID;			
																							
																							err = xQueueSend(ModuleQueue[ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionObjectTakeOver.DestinationModule], moduleQueueTemp, 0);		//往所要传递到的跟踪抖游列中发送数据，传递数据和对象ID	
																							if(err==errQUEUE_FULL) printf("Queue is Full Send Failed!\r\n");
																							pActionNextTemp = pActionList->next;	//必须先将pActionList指向下一个元素，不然先释放pActionList的话会导致pActionList从已被删除的区域指向next																										
																							vTaskSuspendAll();		//进入临界区防止被打断
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//从跟踪段列表中删除执行了的动作
																							printf("ModuleCount==%d,  Module[%d]===>Destination[%d] \n", Module_Count, Module_i, ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionObjectTakeOver.DestinationModule);
																							xTaskResumeAll();			//退出临界区
																							break;
																										
							case TypeActionSetOutput:							
																							xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerOutput, eSetValueWithOverwrite, NULL);
																							TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionSetOutput);//通知PC已经触发了输出
																							pActionNextTemp = pActionList->next;
																							vTaskSuspendAll();		//进入临界区防止被打断
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//从跟踪段列表中删除执行了的动作
																							xTaskResumeAll();		//退出临界区
																							break;//发送通知去触发输出
																										
							case TypeActionTriggerSensor:		xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerSensor, eSetValueWithOverwrite, NULL);	
																							TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionTriggerSensor);//通知PC已经触发了信号采集
																							pActionNextTemp = pActionList->next;
																							vTaskSuspendAll();		//进入临界区防止被打断
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//从跟踪段列表中删除执行了的动作
																							xTaskResumeAll();		//退出临界区
																							break;//发送通知去触发传感器采集
																										
							case TypeActionPushOut:					TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionPushOut);//通知PC已经触发了相机
																																												
																							/******遍历寻找此对象在缓冲区数组的位置， 此处亦须释放对应的ObjectBuffer对象，否则无法再创建对象，无法往ObjectBuffer内填充值*****/
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
																								xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerPush, 	 eSetValueWithOverwrite, NULL);//发送通知去触发剔除																							
																							}
																							
																							pActionNextTemp = pActionList->next;
																							vTaskSuspendAll();		//进入临界区防止被打断
																							ObjectBuffer[object_i].objectAliveFlag = false;					//释放被占有的对象
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//从跟踪段列表中删除执行了的动作
																							xTaskResumeAll();		//退出临界区
																							break;
																										
							case TypeActionRequestMachineData:	
																							TCPSendDataChar(ModuleConfig[Module_i].ActionInstanceConfig[pActionList->Object.ActionNumber].Item_ActionTriggerCamera.ClientID, strActionRequestMachineData);//通知PC已经请求了机器数据
																							pActionNextTemp = pActionList->next;
																							vTaskSuspendAll();		//进入临界区防止被打断
																							ListDeletePointItem(&ObjectInModuleList[Module_i], pActionList);//从跟踪段列表中删除执行了的动作
																							xTaskResumeAll();		//退出临界区
																							break;							
						}	
					pActionList = pActionNextTemp;					//下一轮循环递增pActionNextTemp = pActionList->next，即被删除前的指向的下一个结点,因为pActionList已从列表中删除，再去让它指向下一个元素就会错乱
					}
					else
						pActionList = pActionList->next;
					//xTaskResumeAll();
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

