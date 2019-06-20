#include "FreeRTOS.h"								//操作系统OS
#include "task.h"
#include "malloc.h"
#include "mpu.h"										//内存保护

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "bsp_encoder.h"						//	编码器	
#include "LightSignal.h"						//光电和PWM
#include "pcf8574.h"								//以太网、蜂鸣器驱动
#include "ActionExecute.h"					//输出动作初始化
#include "timer.h"										//基本定时器定时

#include "sdram.h"									//SDRAM驱动
//#include "w25qxx.h"									//W25QXX QPI模式驱动FLASH	 
//#include "nand.h" 								//NAND驱动
#include "sdmmc_sdcard.h"						//SD卡驱动
//#include "ftl.h"  								//NAND FLASH FTL层算法代码
#include "ff.h"											//FATFS文件系统代码
#include "exfuns.h"									//FATFS扩展代码

#include "lwip_comm.h" 							//lwip协议栈初始化
#include "lwip/netif.h"
#include "lwipopts.h"								//lwip协议栈裁剪配置

#include "ParametersLoad.h"					//参数读取管理
#include "TaskManage.h"							//统一管理优先级和堆栈大小
#include "TcpPacketServer.h"				// Server端程序

#include "DataTransferManage.h"			//多SOCKET数据传输统一管理
#include "SemaphoreManage.h"				//统一管理所有信号量的定义和创建
#include "Tracking.h"								//跟踪线程
#include "ScanIO.h"
#include "DataProcess.h"						//数据处理线程

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
extern __IO uint64_t EncoderNumber;

/********************************************************************************
***********************************主入口****************************************
********************************************************************************/
int main(void)
{
	u32 total,free;									//SD卡总容量和剩余容量
	
	Write_Through();                //开启强制透写！
	MPU_Memory_Protection();        //保护相关存储区域以及将SDRAM设为内部存储器，打开SDRAM的访问权限，避免出现HardFault
	Cache_Enable();                 //打开L1-Cache
	HAL_Init();				        			//初始化HAL库
	Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
	delay_init(216);                //延时初始化
	uart_init(115200);		       	 	//串口初始化
	LED_Init();
	
	SDRAM_Init();                   //初始化SDRAM 32MB
	PCF8574_Init();             		//初始化PCF8574，初始化以太网复位端口
	
 	my_mem_init(SRAMIN);		    		//初始化内部内存池512KB,其中SRAMIN为384KB
	my_mem_init(SRAMEX);		    		//初始化外部内存池32MB
	my_mem_init(SRAMDTCM);		    	//初始化CCM内存池  				 SRAMIN为128KB LWIP必须打开这个内存池
	
/////////////////////////////////////////////////////////////////////////////////////////////////	
/////////////////////////文件系统与SDcard,RAM, SDRAM/////////////////////////////////////////////	
////////////////////////////////////////////////////////////////////////////////////////////////
 	while(SD_Init())								//不断检测SD卡并初始化
	{
		delay_ms(500);					
		LED0_Toggle;//DS0闪烁
	}
 	exfuns_init();									//为fatfs相关变量申请内存				 
  f_mount(fs[0],"0:",1); 					//挂载SD卡 

	while(exf_getfree("0:",&total,&free))					//得到SD卡的总容量和剩余容量
	{
		printf("SD Read Failed...Retrying\n");
		delay_ms(100);
	}
	printf("SD Total Size:%d  MB\n SD Free Size:%d  MB\n", total, free);
	
/////////////////////////////////////////END//////////////////////////////////////////////////////
	printf("--> Encoder AND Light Signal Initialing <-- \n");
	ENCODER_TIM3_Init();
	ENCODER_TIM8_Init();
	
  TIM2_CH1_Cap_Init(0XFFFFFFFF,108-1); 			//以1MHZ的频率计数来捕获光电脉冲高电平
	TIM5_CH1_Cap_Init(0XFFFFFFFF,108-1); 			//以1MHZ的频率计数来捕获光电脉冲高电平
	TIM6_Init(0XFFFF, 108-1);								//16位定时器以1MHZ的频率计数来定时，也就是定时器1us计数一次
	
	CreateSemaphore();												//声明并创建所有信号量
	/*暂不写数据*/
	//WriteDownIOInfo();			//写下IO信息到SD卡中
	
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
	//taskENTER_CRITICAL();																//进入代码临界保护区防止被意外进入
	xSemaphoreGive(OnLoadParametersFromPC);							//给出信号量用于加载参数！！！
				
	while(lwip_comm_init()) 														//lwip协议栈初始化 
	{
		vTaskDelay(500);																	//挂起，500ms后再试一次
		printf("LWIP Initialing Failed\n");
	}
	printf("LWIP Initialing Successfully\n");
	
	while(!ParametersLoadTask_init())										//参数读取初始化	
	{
		vTaskDelay(500);
		printf("ParametersLoad Initialing Failed\n");
	}
	printf("ParametersLoad Initialing Successfully\n");
	
	
	while(!TCPServerListenCycleTask_init())							//TCP服务端监听初始化及线程创建	
	{
		vTaskDelay(500);
		printf("TCPSeverListenTask Initialing Failed\n");
	}
	printf("TCPSeverListenTask Initialing Successfully\n");
	
	while(!DataProcessTask_init())								//TCP SOCKET数据收发控制线程初始化
	{
		vTaskDelay(500);
		printf("DataProcessThread Initialing Failed\n");
	}
	printf("DataProcessThread Initialing Successfully\n");

	while(!DataTransferManageTask_init())								//TCP SOCKET数据收发控制线程初始化
	{
		vTaskDelay(500);
		printf("DataTransferManageThread Initialing Failed\n");
	}
	printf("DataTransferManageThread Initialing Successfully\n");
		
	while(!TrackingTask_init())													//Tracking线程初始化
	{
		vTaskDelay(500);
		printf("TrackingThread Initialing Failed\n");
	}
	printf("TrackingThread Initialing Successfully\n");
		
	while(!ActionExecuteTask_init())										//ActionTrriger动作初始化
	{
		vTaskDelay(500);
		printf("ActionExecuteThread Initialing Failed\n");
	}
	printf("ActionExecuteThread Initialing Successfully\n");
	

	xTaskCreate((TaskFunction_t)StatusIndicate_task,
							(const char*  )"StatusIndicate_task",
							(uint16_t     )StatusIndicate_STK_SIZE,
							(void*        )NULL,
							(UBaseType_t  )StatusIndicate_TASK_PRIO,
							(TaskHandle_t*)&StatusIndicateTask_Handler);
/******							
	xTaskCreate((TaskFunction_t)EncoderValue_task,//编码器数值查询任务
							(const char*  )"EncoderValue_task",
							(uint16_t     )EncoderValue_STK_SIZE,
							(void*        )NULL,
							(UBaseType_t  )EncoderValue_TASK_PRIO,
							(TaskHandle_t*)&EncoderValueTask_Handler);						
******/							
							
	xTaskCreate((TaskFunction_t)SimaulationTimer_task,
							(const char*  )"SimaulationTimer_task",
							(uint16_t     )SimulationTimer_STK_SIZE,
							(void*        )NULL,
							(UBaseType_t  )SimulationTimer_TASK_PRIO,
							(TaskHandle_t*)&SimulationTimerTask_Handler);							

#if LWIP_DHCP
	lwip_comm_dhcp_creat(); 															//创建DHCP任务
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

void SimaulationTimer_task(void)
{
	while(1)
	{
		Timer++;
		vTaskDelay(10);																	//Timer 10ms加一次
	}
}

#ifdef DEBUG

void vAssertCalled( uint32_t ulLine, const char *pcFile )
{
	printf("AssetError:%s,%d\r\n", pcFile, ulLine);
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )			//任务堆栈溢出钩子函数
{
	printf("TaskOverFlow:%s.\r\n", pcTaskName);
}
#endif
