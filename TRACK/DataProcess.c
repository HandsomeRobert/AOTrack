/**************************************************************************************************
*************************************���ݴ����߳�***************************************************
***************************************************************************************************/
#include "DataProcess.h"
#include "malloc.h"
#include "Tracking.h"
#include "timer.h"

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
	byte i=0 ,j=0;
//	byte err = 0;
	int* pInt;
	byte* pByte;
	static int packetID 	= 0;
	static int packetType 	= 0;
	static int packetDataSize 	= 0;
	static int actionID = 0;
	static int lineID 	= 0;
	static int objectID = 0;
	static int moduleID = 0;
	static int encoder 	= 0;
	static int commandDataSize = 0;
	int timeCount;
	
	bool isGetCompletePacket = false;
	int packetSize = 0;
	struct pbuf *q;
	struct netbuf *recvbuf;
	int singleData_len = 0;
	int existSize = 0;
	
//	bool dataWait = true;
	
	byte* pCommandData 	= (byte*)mymalloc(SRAMEX, 128);		//��PC��������Ϣ
	
	while(1)
	{
		timeCount	= __HAL_TIM_GET_COUNTER(&TIM6_Handler);
		
		for(i = 0;i < ClientNum;i++)
		{
			
/*************�������ݴ���****************************
*****************************************************/	
			/*****store the buffer to PHead and PTail******/			
			if((netconn_recv(Session[i].NetConnRecv, &recvbuf)) == ERR_OK)  	//���յ�����
			{						
				taskENTER_CRITICAL();  //���ж�	

				singleData_len = recvbuf->p->tot_len;					
				
				if(PTail[i] + singleData_len > RecvAreaRange[i])
				{
					existSize = PTail[i] - PHead[i];
					mymemcpy(Session[i].BufferRecvArea, PHead[i], existSize);			
					PHead[i] = Session[i].BufferRecvArea;
					PTail[i] = Session[i].BufferRecvArea + existSize;
					
					for(q=recvbuf->p;q!=NULL;q=q->next)
					{
						mymemcpy(PTail[i], q->payload, q->len);
						PTail[i] = PTail[i] + q->len;
					}
				}
				else
				{
					for(q=recvbuf->p;q!=NULL;q=q->next)
					{
						mymemcpy(PTail[i], q->payload, q->len);
						PTail[i] = PTail[i] + q->len;					//Move Tail Pointer
					}					
				}
					
				q = NULL;	
				taskEXIT_CRITICAL();  //���ж�
				netbuf_delete(recvbuf);//һ��Ҫ������һ��!!!!��Ȼ���ڴ�й©������
				recvbuf = NULL;							
			}	
			
			existSize = PTail[i] - PHead[i];
			/****Process the Receive Buffer******/
			if(existSize > PACKET_HEADER_SIZE || existSize == PACKET_HEADER_SIZE)// Receive a Full Packet Head
			{
				packetSize = *(int*)(PHead[i] + 12) + PACKET_HEADER_SIZE + 4;//4 is EndWord size
	
/*Test*/			if(packetSize != 44)
								q = NULL;

				if(existSize > packetSize || existSize==packetSize)//>=
				{
					isGetCompletePacket = true;
					
					pInt 			= (int*)PHead[i];
					PHead[i] = PHead[i] + packetSize;
				}
			}

	
			
			if(isGetCompletePacket)
			{
				isGetCompletePacket = false;

				if((*pInt) == 0x47424B50 && *(pInt+packetSize/4) == 0x44454B50)//ȷ�������Ƿ���Ч������ͷ
				{
					pInt++;//������ͷ
					packetID 		= *pInt++;
					packetType 		= *pInt++;	
					packetDataSize 		= *pInt++;
					actionID 	= *pInt++;
					lineID 	= *pInt++;
					objectID 	= *pInt++;
					moduleID 	= *pInt++;
					encoder 	= *pInt++;
					commandDataSize 	= *pInt++;
//////					if(commandDataSize > 0)// check whether Command have Data
//////						mymemcpy(pCommandData, pInt, commandDataSize);////////////////////////////////////Receive Data Not acquired Complete, result in commandDataSize overflow read, A very large value!!!
				
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
						default:break;					
					}
				}
			}
		}
		
//////		timeCount	= __HAL_TIM_GET_COUNTER(&TIM6_Handler) - timeCount;
//////		if(timeCount > 100)
//////		{
//////			printf("DataProcess Thread Time ==>%d \n", timeCount);//106us
//////		}
		vTaskDelay(1);
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
