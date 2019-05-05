/**************************************************************************************************
*************************************����ִ���߳�******************************************************
***************************************************************************************************/
#include "ActionExecute.h"

#define Message_TrrigerCamera 1			//������Ӧ����Ϣֵ
#define Message_TrrigerSensor 2
#define Message_TrrigerOutput 3
#define Message_TrrigerPush 	4

TaskHandle_t ActionExecuteTask_Handler;
static void ActionExecuteThread(void );


//��������ִ���߳�
//����ֵ:0 �������񴴽��ɹ�
//		���� �������񴴽�ʧ��
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

//ActionExecute�����߳�
static void ActionExecuteThread(void)
{
	BaseType_t notifyValue = 0;
	
	GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_GPIOH_CLK_ENABLE();			//����GPIOHʱ��
	
  GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15; //PH7,8,9,11,12,13,14,15
  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
  GPIO_Initure.Pull=GPIO_PULLUP;          //����
  GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
  HAL_GPIO_Init(GPIOH,&GPIO_Initure);
	
  HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7,GPIO_PIN_RESET);		//PH7��0
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_RESET);		//PH8��0 
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_RESET);		//PH9��0
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_RESET);	//PH11��0 
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12,GPIO_PIN_RESET);	//PH12��0
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_13,GPIO_PIN_RESET);	//PH13��0 
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_14,GPIO_PIN_RESET);	//PH14��0
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_15,GPIO_PIN_RESET);	//PH15��0 
	

	while(1)
	{
		notifyValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);	//��ȡ����ֵ֪ͨ

		switch(notifyValue)
		{
			case Message_TrrigerCamera:									//Trriger Camera
				
			//printf("TriggerCamera In ActionExecute.\n");
			HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7,GPIO_PIN_SET);			//���ߵ�ƽ
			vTaskDelay(10);
			HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7,GPIO_PIN_RESET);		//���͵�ƽ
			
			break;
			
			case Message_TrrigerSensor:									//Trriger sensor
				//printf("TriggerSensor In ActionExecute.\n");
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_SET);			//���ߵ�ƽ
				vTaskDelay(10);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_RESET);		//���͵�ƽ
			break;
			
			case Message_TrrigerOutput:									//Trriger Output
				//printf("TriggerOutput In ActionExecute.\n");
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_SET);			//���ߵ�ƽ
				vTaskDelay(10);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_RESET);		//���͵�ƽ
				break;
			
			case Message_TrrigerPush:										//Trriger Push
				//printf("TriggerPush In ActionExecute.\n");
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_SET);			//���ߵ�ƽ
				vTaskDelay(10);
				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_RESET);		//���͵�ƽ
				break;															
		}	
		vTaskDelay(1);
	}	
}

