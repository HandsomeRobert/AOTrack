/**************************************************************************************************
*************************************动作执行线程******************************************************
***************************************************************************************************/
#include "ActionExecute.h"
#include "Tracking.h"
#include "HardwareList.h"

TaskHandle_t ActionExecuteTask_Handler;
static void ActionExecuteThread(void );


//创建动作执行线程
//返回值:0 跟踪任务创建成功
//		其他 跟踪任务创建失败
uint8_t ActionExecuteTask_init(void)
{
	uint8_t res;
	taskENTER_CRITICAL();              
	res = xTaskCreate((TaskFunction_t)ActionExecuteThread,
					(const char*  )"ActionExecuteTread",
					(uint16_t     )ActionExecute_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )ActionExecute_TASK_PRIO,
					(TaskHandle_t*)&ActionExecuteTask_Handler);
	taskEXIT_CRITICAL();

	return res;
}

//ActionExecute跟踪线程
static void ActionExecuteThread(void)
{
	BaseType_t actionType = 0;
	uint16_t PinID;
	ActionExecuteQueueItem* actionInfoTemp = mymalloc(SRAMEX, sizeof(ActionExecuteQueueItem));
	
//	static propActionRequestMachineData* 	pActionExcuteTempRequestMachineData = NULL;
	static propActionSetOutput* 					pActionExcuteTempSetOutput      = NULL;
//	static propActionObjectTakeOver* 			pActionExcuteTempObjectTakeOver = NULL;
	static propActionTriggerCamera* 			pActionExcuteTempTriggerCamera 	= NULL;
	static propActionTriggerSensor* 			pActionExcuteTempTriggerSensor 	= NULL;
	static propActionPushOut* 						pActionExcuteTempPushOut 				= NULL;
	
	GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_GPIOH_CLK_ENABLE();			//开启GPIOH时钟
	
  GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15; //PH7,8,9,11,12,13,14,15
  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
  GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
  GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
  HAL_GPIO_Init(GPIOH,&GPIO_Initure);
	
  HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7,GPIO_PIN_RESET);		//PH7置0
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_RESET);		//PH8置0 
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_RESET);		//PH9置0
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_RESET);	//PH11置0 
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12,GPIO_PIN_RESET);	//PH12置0
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_13,GPIO_PIN_RESET);	//PH13置0 
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_14,GPIO_PIN_RESET);	//PH14置0
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_15,GPIO_PIN_RESET);	//PH15置0 
	

	while(1)
	{
		if(ActionExecuteQueue != NULL)
		{
			if(xQueueReceive(ActionExecuteQueue, actionInfoTemp, 0) == pdTRUE)	//获取队列信息
			{
			actionType = actionInfoTemp->actionType;
			switch(actionType)
			{
				case Message_TrrigerCamera:									//Trriger Camera
				
						pActionExcuteTempTriggerCamera = (propActionTriggerCamera*)actionInfoTemp->pAction;
						
						switch(pActionExcuteTempTriggerCamera->CameraID)
						{
							case DigitalOutput_1: PinID = GPIO_PIN_7;break;
							case DigitalOutput_2: PinID = GPIO_PIN_8;break;
							case DigitalOutput_3: PinID = GPIO_PIN_9;break;
							case DigitalOutput_4: PinID = GPIO_PIN_11;break;						
							case DigitalOutput_5: PinID = GPIO_PIN_12;break;
							case DigitalOutput_6: PinID = GPIO_PIN_13;break;
							case DigitalOutput_7: PinID = GPIO_PIN_14;break;
							case DigitalOutput_8: PinID = GPIO_PIN_15;break;
							default:break;
						}				
						HAL_GPIO_WritePin(GPIOH,PinID,GPIO_PIN_SET);			//拉高电平
						vTaskDelay(pActionExcuteTempTriggerCamera->OutputDuration);
						HAL_GPIO_WritePin(GPIOH,PinID,GPIO_PIN_RESET);		//拉低电平
				
				break;
				
				case Message_TrrigerSensor:									//Trriger sensor
						pActionExcuteTempTriggerSensor = (propActionTriggerSensor*)actionInfoTemp->pAction;
				
						switch(pActionExcuteTempTriggerSensor->SensorID)
						{
							case DigitalOutput_1: PinID = GPIO_PIN_7;break;
							case DigitalOutput_2: PinID = GPIO_PIN_8;break;
							case DigitalOutput_3: PinID = GPIO_PIN_9;break;
							case DigitalOutput_4: PinID = GPIO_PIN_11;break;						
							case DigitalOutput_5: PinID = GPIO_PIN_12;break;
							case DigitalOutput_6: PinID = GPIO_PIN_13;break;
							case DigitalOutput_7: PinID = GPIO_PIN_14;break;
							case DigitalOutput_8: PinID = GPIO_PIN_15;break;
							default:break;
						}				
				break;
				
				case Message_TrrigerOutput:									//Trriger Output
					pActionExcuteTempSetOutput = (propActionSetOutput*)actionInfoTemp->pAction;
				
						switch(pActionExcuteTempSetOutput->DigitalOutput)
						{
							case DigitalOutput_1: PinID = GPIO_PIN_7;break;
							case DigitalOutput_2: PinID = GPIO_PIN_8;break;
							case DigitalOutput_3: PinID = GPIO_PIN_9;break;
							case DigitalOutput_4: PinID = GPIO_PIN_11;break;						
							case DigitalOutput_5: PinID = GPIO_PIN_12;break;
							case DigitalOutput_6: PinID = GPIO_PIN_13;break;
							case DigitalOutput_7: PinID = GPIO_PIN_14;break;
							case DigitalOutput_8: PinID = GPIO_PIN_15;break;
							default:break;
						}
						if(pActionExcuteTempSetOutput->OutputInvert)
						{
							HAL_GPIO_WritePin(GPIOH,PinID,GPIO_PIN_RESET);			//拉低电平
							vTaskDelay(pActionExcuteTempSetOutput->OutputDuration);
							HAL_GPIO_WritePin(GPIOH,PinID,GPIO_PIN_SET);				//拉高电平
						}
						else
						{
							HAL_GPIO_WritePin(GPIOH,PinID,GPIO_PIN_SET);			//拉高电平
							vTaskDelay(pActionExcuteTempSetOutput->OutputDuration);
							HAL_GPIO_WritePin(GPIOH,PinID,GPIO_PIN_RESET);		//拉低电平
						}
					break;
				
				case Message_TrrigerPush:										//Trriger Push
					pActionExcuteTempPushOut = (propActionPushOut*)actionInfoTemp->pAction;
				
						switch(pActionExcuteTempPushOut->DigitalOutput)
						{
							case DigitalOutput_1: PinID = GPIO_PIN_7;break;
							case DigitalOutput_2: PinID = GPIO_PIN_8;break;
							case DigitalOutput_3: PinID = GPIO_PIN_9;break;
							case DigitalOutput_4: PinID = GPIO_PIN_11;break;						
							case DigitalOutput_5: PinID = GPIO_PIN_12;break;
							case DigitalOutput_6: PinID = GPIO_PIN_13;break;
							case DigitalOutput_7: PinID = GPIO_PIN_14;break;
							case DigitalOutput_8: PinID = GPIO_PIN_15;break;
							default:break;
						}				
						HAL_GPIO_WritePin(GPIOH,PinID,GPIO_PIN_SET);			//拉高电平
						vTaskDelay(pActionExcuteTempPushOut->PushLength);
						HAL_GPIO_WritePin(GPIOH,PinID,GPIO_PIN_RESET);		//拉低电平
					break;															
			}	
		 }
		}
		
		vTaskDelay(1);
	}	
}

