/**************************************************************************************************
*************************************数据处理线程******************************************************
***************************************************************************************************/
#include "DataProcess.h"
#include "malloc.h"

TaskHandle_t DataProcessTask_Handler;
static void DataProcessThread(void *arg);

//创建数据处理
//返回值:0数据处理任务创建成功
//		其他 数据处理创建失败
uint8_t DataProcessTask_init(void)
{
	uint8_t res;
	taskENTER_CRITICAL();              
	res = xTaskCreate((TaskFunction_t)DataProcessThread,
					(const char*  )"DataProcessThread",
					(uint16_t     )DataProcess_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )DataProcess_TASK_PRIO,
					(TaskHandle_t*)&DataProcessTask_Handler);
	taskEXIT_CRITICAL();

	return res;
}

//数据处理线程
static void DataProcessThread(void *arg)
{
	byte i =0;
	byte err = 0;
	int* pInt;
	byte* pByte;
	static int lineID = 0;
	static int actionID = 0;
	static int objectID = 0;
	static int moduleID = 0;
	static int encoder = 0;
	static int dataSize = 0;
	
	int* pRecvData = (int*)mymalloc(SRAMEX, TCP_Queue_MAXBUFSIZE);		//从PC来的完整数据包
	byte* pData 	 = (byte*)mymalloc(SRAMEX, TCP_Queue_MAXBUFSIZE);		//从PC传来的信息
	
	while(1)
	{
		for(i = 0;i < ClientNum;i++)
		{
			if(isEmpityQueue(Session[i].QueueRecv) == FALSE)
			{
				if(deQueue(Session[i].QueueRecv, &pRecvData))		//出队成功
				{
/*Test*Test*/	if((byte)*pRecvData == '1') xSemaphoreGive(OnSysRestart);		//用于测试数据接收
					
					pInt 			= (int*)pRecvData;
					lineID 		= *pInt++;
					actionID 	= *pInt++;
					objectID 	= *pInt++;
					moduleID 	= *pInt++;
					encoder 	= *pInt++;
					dataSize 	= *pInt++;
					
					pByte = (byte*)pInt;
					pData = pByte;							//去除包头后的实际数据
					
					switch(actionID)						//来自PC的控制命令
					{	//System
						case ActionHeartBeat: 	break;
						case ActionWarmRestart: break;
						case ActionColdRestart: xSemaphoreGive(OnSysRestart);break;
						case ActionReconfiguration: break;
						case ActionStartIOLive: break;
						case ActionGetConnectedClientID: break;
						case ActionErrorMessage: break;
						case ActionErrorAcknowledge: break;
						//Tracking
						case ActionObjectRunIn: break;
						case ActionObjectRunOut: break;
						case ActionObjectDelete: break;
						case ActionTriggerCamera: break;
						case ActionTriggerIOSensor: break;
						case ActionGetMachineData: break;
						case ActionSetPushResult: break;
						case ActionSetUserResult: break;
						case ActionStartTracking: break;
						case ActionObjectFallDown: break;
						//Automation
						case ActionSetPLCVariable: break;
						case ActionStartControl: break;
						//Diagnostics
						case ActionSetTrackingMode: break;
						case ActionObjectPosition: break;
						case ActionObjectWidth: break;
						case ActionRequestModuleInfo: break;
						case ActionRequestPLCInfo: break;
						case ActionTrackingDummy: break;
						
					}
				}
				else printf("Out Queue[%d] failed! \n", i);								
			}
		}
		
		vTaskDelay(10);
	}																					
}

