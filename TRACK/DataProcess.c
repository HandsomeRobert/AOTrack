/**************************************************************************************************
*************************************���ݴ����߳�******************************************************
***************************************************************************************************/
#include "DataProcess.h"
#include "malloc.h"
#include "ObjectDetection.h"

TaskHandle_t DataProcessTask_Handler;
static void DataProcessThread(void *arg);
static void PCGetConnectedClientIDs(byte clientID, struct PacketServerSession session[], int clientNum);
static void STM32GetPushResult(int lineID, int objectID, int pushline, int priority);
static void STM32SetTrackingMode(int lineID, int mode, int interval);
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
	static int lineID 	= 0;
	static int actionID = 0;
	static int objectID = 0;
	static int moduleID = 0;
	static int encoder 	= 0;
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
						case PCCmdActionHeartBeat: 		break;
						case PCCmdActionWarmRestart: 	break;
						case PCCmdActionColdRestart: 		xSemaphoreGive(OnSysRestart);							break;
						case PCCmdActionReconfiguration: 		break;//xSemaphoreGive(OnLoadParametersFromPC);
						case PCCmdActionStartIOLive: 	break;
						case PCCmdActionGetConnectedClientID: PCGetConnectedClientIDs(0, Session, ClientNum);break;
						case PCCmdActionErrorMessage: break;
						case PCCmdActionErrorAcknowledge: break;
						//Tracking
						case PCCmdActionObjectRunIn: break;
						case PCCmdActionObjectRunOut: break;
						case PCCmdActionObjectDelete: break;
						case PCCmdActionTriggerCamera: break;
						case PCCmdActionTriggerIOSensor: break;
						case PCCmdActionGetMachineData: break;
						case PCCmdActionSetPushResult: STM32GetPushResult(lineID, objectID, moduleID, encoder);break;//
						case PCCmdActionSetUserResult: break;
						case PCCmdActionStartTracking: break;
						case PCCmdActionObjectFallDown: break;
						//Automation
						case PCCmdActionSetPLCVariable: break;
						case PCCmdActionStartControl: break;
						//Diagnostics
						case PCCmdActionSetTrackingMode: STM32SetTrackingMode(lineID, objectID, moduleID);break;
						case PCCmdActionObjectPosition: break;
						case PCCmdActionObjectWidth: break;
						case PCCmdActionRequestModuleInfo: break;
						case PCCmdActionRequestPLCInfo: break;
						case PCCmdActionTrackingDummy: break;
						
					}
				}
				else printf("Outueue[%d] failed! \n", i);								
			}
		}
		
		vTaskDelay(10);
	}																					
}

static void PCGetConnectedClientIDs(byte clientID, struct PacketServerSession session[], int clientNum)
{
		byte* pByte;
//	int clientIDs[clientNum];
	byte i = 0;
	for(i=0; i < clientNum; i++)
	{
		*pByte = session[i].ClientID;
		 pByte++;
	}
	
	TCPSendDataByte(clientID , pByte, clientNum);		//�ж��ٸ��ͻ��˾��ж��ٸ�Client
}

static void STM32GetPushResult(int lineID, int objectID, int pushline, int priority)
{
	byte object_i = 0;
	//��������objectID�ڶ��󻺳������λ��
	while(ObjectBuffer[object_i].ObjectID != objectID)
	{
		object_i++;
		if(object_i > (maxTrackingObjects - 1)) //��������ĩβ��δ�ҵ�
		{
			printf("Cannot Find The consistent ObjectID in ObjectBuffer failed. In DataProcess \n");
			break;
		}	
	}
	
	ObjectBuffer[object_i].ProcessedResult = false;			//����Ľ��Ϊ�޳�
}	

/// <summary>
/// PC����STM32����ģʽ
/// </summary>
/// <param name="mode"></param>
/// <param name="interval"></param>
/// [ObjectID]
/// 0: 	TrackingNone 			ֹͣ����
///	1: 	TrackingProduction 		��׼����ģʽ
///	2:	TrackingOneObject		��ϸ���ģʽ��������ϸ��ٶ���
///	3:	TrackingMultiObject		��ϸ���ģʽ��һ�����˸��ٶ��������ڽ������Զ�������һ����ϸ��ٶ���
///	4:	TrackingLearning		ѧϰ����ģʽ�����������ٹ����У�ֻ������һ�����ٶ��󾭹��������
///	[ModuleID]
/// interval: the module position packet will be sent 
        /// every number of tracking cycles  
static void STM32SetTrackingMode(int lineID, int mode, int interval)
{
	//Reamin to be done!!!
}
