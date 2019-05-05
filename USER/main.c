#include "FreeRTOS.h"								//����ϵͳOS
#include "task.h"
#include "malloc.h"
#include "mpu.h"										//�ڴ汣��

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "bsp_encoder.h"						//	������	
#include "LightSignal.h"						//����PWM
#include "pcf8574.h"								//��̫��������������
#include "ActionExecute.h"					//���������ʼ��

#include "sdram.h"									//SDRAM����
//#include "w25qxx.h"									//W25QXX QPIģʽ����FLASH	 
//#include "nand.h" 								//NAND����
#include "sdmmc_sdcard.h"						//SD������
//#include "ftl.h"  								//NAND FLASH FTL���㷨����
#include "ff.h"											//FATFS�ļ�ϵͳ����
#include "exfuns.h"									//FATFS��չ����

#include "lwip_comm.h" 							//lwipЭ��ջ��ʼ��
#include "lwip/netif.h"
#include "lwipopts.h"								//lwipЭ��ջ�ü�����

#include "ParametersLoad.h"					//������ȡ����
#include "TaskManage.h"							//ͳһ�������ȼ��Ͷ�ջ��С
#include "TcpPacketClient.h" 				//Client�˳���
#include "TcpPacketServer.h"				// Server�˳���
#include "UdpPacket.h"							//UDP�˳���

#include "DataTransferManage.h"			//��SOCKET���ݴ���ͳһ����
#include "SemaphoreManage.h"				//ͳһ���������ź����Ķ���ʹ���
#include "Tracking.h"								//�����߳�
#include "ObjectDetection.h"				//���ٶ�������߳�
#include "MyList.h"
#include "ScanIO.h"
#include "DataProcess.h"						//���ݴ����߳�

void Start_task(void *pvParameters);
TaskHandle_t StartTask_Handler;

void StatusIndicate_task(void *p_arg);
TaskHandle_t StatusIndicateTask_Handler;

TaskHandle_t DataAcquisitionTask_Handler;
void DataAcquisition_task(void *p_arg);

TaskHandle_t SimulationTimerTask_Handler;
void SimaulationTimer_task(void);
/******
TaskHandle_t EncoderValueTask_Handler;
void EncoderValue_task();
******/
uint32_t Timer = 0;
extern __IO int64_t EncoderNumber;

/********************************************************************************
***********************************�����****************************************
********************************************************************************/
int main(void)
{
	u32 total,free;									//SD����������ʣ������
	
	Write_Through();                //����ǿ��͸д��
	MPU_Memory_Protection();        //������ش洢�����Լ���SDRAM��Ϊ�ڲ��洢������SDRAM�ķ���Ȩ�ޣ��������HardFault
	Cache_Enable();                 //��L1-Cache
	HAL_Init();				        			//��ʼ��HAL��
	Stm32_Clock_Init(432,25,2,9);   //����ʱ��,216Mhz 
	delay_init(216);                //��ʱ��ʼ��
	uart_init(115200);		       	 	//���ڳ�ʼ��
	LED_Init();
	
	SDRAM_Init();                   //��ʼ��SDRAM 32MB
	PCF8574_Init();             		//��ʼ��PCF8574����ʼ����̫����λ�˿�
	
 	my_mem_init(SRAMIN);		    		//��ʼ���ڲ��ڴ��512KB,����SRAMINΪ384KB
	my_mem_init(SRAMEX);		    		//��ʼ���ⲿ�ڴ��32MB
	my_mem_init(SRAMDTCM);		    	//��ʼ��CCM�ڴ��  				 SRAMINΪ128KB LWIP���������ڴ��
	
/////////////////////////////////////////////////////////////////////////////////////////////////	
/////////////////////////�ļ�ϵͳ��SDcard,RAM, SDRAM/////////////////////////////////////////////	
////////////////////////////////////////////////////////////////////////////////////////////////
 	while(SD_Init())								//���ϼ��SD������ʼ��
	{
		delay_ms(500);					
		LED0_Toggle;//DS0��˸
	}
 	exfuns_init();									//Ϊfatfs��ر��������ڴ�				 
  f_mount(fs[0],"0:",1); 					//����SD�� 

	while(exf_getfree("0:",&total,&free))					//�õ�SD������������ʣ������
	{
		printf("SD Read Failed...Retrying\n");
		delay_ms(100);
	}
	printf("SD Total Size:%d  MB\n SD Free Size:%d  MB\n", total, free);
	
/////////////////////////////////////////END//////////////////////////////////////////////////////
	printf("--> Encoder AND Light Signal Initialing <-- \n");
	ENCODER_TIM3_Init();
	ENCODER_TIM8_Init();
	
  TIM2_CH1_Cap_Init(0XFFFFFFFF,108-1); 			//��1MHZ��Ƶ�ʼ���������������ߵ�ƽ
	TIM5_CH1_Cap_Init(0XFFFFFFFF,108-1); 			//��1MHZ��Ƶ�ʼ���������������ߵ�ƽ

	CreateSemaphore();												//���������������ź���
	/*�ݲ�д����*/
	//WriteDownIOInfo();			//д��IO��Ϣ��SD����
	
	xTaskCreate((TaskFunction_t)Start_task,
							(const char*  )"Start_task",
							(uint16_t     )START_STK_SIZE,
							(void*        )NULL,
							(UBaseType_t  )START_TASK_PRIO,
							(TaskHandle_t*)&StartTask_Handler);
							
	vTaskStartScheduler();								
}

void Start_task(void *pvParameters)
{
	//taskENTER_CRITICAL();																//��������ٽ籣������ֹ���������
	xSemaphoreGive(OnLoadParametersFromPC);							//�����ź������ڼ��ز���������
				
	while(lwip_comm_init()) 														//lwipЭ��ջ��ʼ�� 
	{
		vTaskDelay(500);																	//����500ms������һ��
		printf("LWIP Initialing Failed\n");
	}
	printf("LWIP Initialing Successfully\n");
	
	while(!ParametersLoadTask_init())										//������ȡ��ʼ��	
	{
		vTaskDelay(500);
		printf("ParametersLoad Initialing Failed\n");
	}
	printf("ParametersLoad Initialing Successfully\n");
	
	
	while(!TCPServerListenCycleTask_init())							//TCP����˼�����ʼ�����̴߳���	
	{
		vTaskDelay(500);
		printf("TCPSeverListenTask Initialing Failed\n");
	}
	printf("TCPSeverListenTask Initialing Successfully\n");
	
	while(!DataProcessTask_init())								//TCP SOCKET�����շ������̳߳�ʼ��
	{
		vTaskDelay(500);
		printf("DataProcessThread Initialing Failed\n");
	}
	printf("DataProcessThread Initialing Successfully\n");

	while(!DataTransferManageTask_init())								//TCP SOCKET�����շ������̳߳�ʼ��
	{
		vTaskDelay(500);
		printf("DataTransferManageThread Initialing Failed\n");
	}
	printf("DataTransferManageThread Initialing Successfully\n");
	
	while(!ObjectDetectionTask_init())									//ObjectDetion�̳߳�ʼ��
	{
		vTaskDelay(500);
		printf("ObjectDetectionThread Initialing Failed\n");
	}
	printf("ObjectDetectionThread Initialing Successfully\n");
	
	while(!TrackingTask_init())													//Tracking�̳߳�ʼ��
	{
		vTaskDelay(500);
		printf("TrackingThread Initialing Failed\n");
	}
	printf("TrackingThread Initialing Successfully\n");
		
	while(!ActionExecuteTask_init())										//ActionTrriger������ʼ��
	{
		vTaskDelay(500);
		printf("ActionExecuteThread Initialing Failed\n");
	}
	printf("ActionExecuteThread Initialing Successfully\n");
	
/*	
//	//TCP�ͻ��˳�ʼ�����̴߳���	
//	taskENTER_CRITICAL();
//	while(!tcp_client_init())
//	{
//		vTaskDelay(500);
//		printf("TCP_Client Initialing Failed\n");
//	}
//	printf("TCP_Client Initialing Successfully\n");

////UDP�̳߳�ʼ��	
//	while(!udp_demo_init())
//	{
//		delay_ms(500);
//		printf("UDP_Demo Initialing Failed\n");
//	}
//	printf("UDP_Demo Initialing Successfully\n");
*/	
	xTaskCreate((TaskFunction_t)StatusIndicate_task,
							(const char*  )"StatusIndicate_task",
							(uint16_t     )StatusIndicate_STK_SIZE,
							(void*        )NULL,
							(UBaseType_t  )StatusIndicate_TASK_PRIO,
							(TaskHandle_t*)&StatusIndicateTask_Handler);
/******							
	xTaskCreate((TaskFunction_t)EncoderValue_task,//��������ֵ��ѯ����
							(const char*  )"EncoderValue_task",
							(uint16_t     )EncoderValue_STK_SIZE,
							(void*        )NULL,
							(UBaseType_t  )EncoderValue_TASK_PRIO,
							(TaskHandle_t*)&EncoderValueTask_Handler);						
******/							
	xTaskCreate((TaskFunction_t)DataAcquisition_task,
							(const char*  )"DataAcquisiton_task",
							(uint16_t     )DataAcquisition_STK_SIZE,
							(void*        )NULL,
							(UBaseType_t  )DataAcquisition_TASK_PRIO,
							(TaskHandle_t*)&DataAcquisitionTask_Handler);
		
	xTaskCreate((TaskFunction_t)SimaulationTimer_task,
							(const char*  )"SimaulationTimer_task",
							(uint16_t     )SimulationTimer_STK_SIZE,
							(void*        )NULL,
							(UBaseType_t  )SimulationTimer_TASK_PRIO,
							(TaskHandle_t*)&SimulationTimerTask_Handler);							

#if LWIP_DHCP
	lwip_comm_dhcp_creat(); 															//����DHCP����
#endif							
							
	vTaskDelete(StartTask_Handler);
	//taskEXIT_CRITICAL();							
}

void StatusIndicate_task(void *p_arg)
{
	while(1)
	{
		LED1_Toggle;
		//printf("EncoderNum==>%llu\n", EncoderNumber);
//printf("Test Status Indicate\n");
		vTaskDelay(1000);
	}
}
/******
//��������ѯ����
void EncoderValue_task()
{
	while(1)
	{
		EncoderNumber = (OverflowCount*CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder);
		vTaskDelay(1);
	}
}
******/

void DataAcquisition_task(void * p_arg)
{

	while(1)
	{
    DataSendFlag |= LWIP_SEND_DATA;
		//tcp_client_flag |= LWIP_SEND_DATA;
		//udp_flag |= LWIP_SEND_DATA;
		
//		//PWM������
//		TIM_SetTIM3Compare4(TIM_GetTIM3Capture4()+1); 
//		if(TIM_GetTIM3Capture4()==300)
//		TIM_SetTIM3Compare4(0);
		vTaskDelay(1000);
	}
}

void SimaulationTimer_task(void)
{
	while(1)
	{
		Timer++;
		vTaskDelay(10);																	//Timer 10ms��һ��
	}
}

#ifdef DEBUG

void vAssertCalled( uint32_t ulLine, const char *pcFile )
{
	printf("AssetError:%s,%d\r\n", pcFile, ulLine);
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )			//�����ջ������Ӻ���
{
	printf("TaskOverFlow:%s.\r\n", pcTaskName);
}
#endif