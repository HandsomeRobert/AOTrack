/**************************************************************************************************
*************************************���ݴ����߳�******************************************************
***************************************************************************************************/
#include "DataProcess.h"
#include "malloc.h"

TaskHandle_t DataProcessTask_Handler;
static void DataProcessThread(void *arg);

//�������ݴ���
//����ֵ:0���ݴ������񴴽��ɹ�
//		���� ���ݴ�����ʧ��
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

//���ݴ����߳�
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
	
	int* pRecvData = (int*)mymalloc(SRAMEX, TCP_Queue_MAXBUFSIZE);		//��PC�����������ݰ�
	byte* pData 	 = (byte*)mymalloc(SRAMEX, TCP_Queue_MAXBUFSIZE);		//��PC��������Ϣ
	
	while(1)
	{
		for(i = 0;i < ClientNum;i++)
		{
			if(isEmpityQueue(Session[i].QueueRecv) == FALSE)
			{
				if(deQueue(Session[i].QueueRecv, &pRecvData))		//���ӳɹ�
				{
/*Test*Test*/	if((byte)*pRecvData == '1') xSemaphoreGive(OnSysRestart);		//���ڲ������ݽ���
					
					pInt 			= (int*)pRecvData;
					lineID 		= *pInt++;
					actionID 	= *pInt++;
					objectID 	= *pInt++;
					moduleID 	= *pInt++;
					encoder 	= *pInt++;
					dataSize 	= *pInt++;
					
					pByte = (byte*)pInt;
					pData = pByte;							//ȥ����ͷ���ʵ������
					
					switch(actionID)						//����PC�Ŀ�������
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

