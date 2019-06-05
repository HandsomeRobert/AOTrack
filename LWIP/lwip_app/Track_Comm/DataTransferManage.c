/**************************************************************************************************
**************************数据收发管理线程******************************************************
***************************************************************************************************/

#include "DataTransferManage.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "TcpPacketServer.h"
#include "stdbool.h"
#include "timer.h"

TaskHandle_t DataTransferManageTask_Handler;		//任务句柄
static void DataTransferManage(void *arg);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WriteDataToBufferSend(byte Session_i, byte* pData, byte data_len)
{
	byte j = 0;
	while(Session[Session_i].BufferSend[j].IsBufferAlive)
	{
		j++;
		if(j > MaxBufferLength)
		{
			printf("Client[%d]==Session[%d].BufferSend is full , waiting to be free \r\n", Session[Session_i].ClientID, Session_i);
			j = 0;
			break;
		}
	}
	mymemcpy(Session[Session_i].BufferSend[j].pBufferData, pData, data_len);
	Session[Session_i].BufferSend[j].IsBufferAlive = true;
}

void TCPSendPacket(byte clientID, Packet* packet)
{
	err_t err;
	int i = 0;
////	uint16_t timeCount = 0;
	for(i = 0;i<ClientNum;i++)
	{
		if(Session[i].ClientID == clientID)
		{
////		timeCount 	= __HAL_TIM_GET_COUNTER(&TIM6_Handler);			//包头大小        +   包数据大小     + 包尾大小
			//NETCONN_NOFLAG:2830 - 5082  ==> NETCONN_COPY:3424-->4824 NETCONN_NOCOPY::3454-->5016 NETCONN_MORE::3424-->4956
			err = netconn_write(Session[i].NetConnSend ,packet,(PACKET_HEADER_SIZE + packet->DataSize + 4),NETCONN_MORE); //!!!发送数据sizeof(tcp_server_sendbuf)
////			timeCount 	= __HAL_TIM_GET_COUNTER(&TIM6_Handler) - timeCount;		//耗时70us
////			printf("netconn_write time  ==>%d \n", timeCount);
			if(err != ERR_OK) printf("Send data in TCPSendPacket Failed,Please check it in DataTransferManage.c code[%d] \r\n", err);
			//myfree(SRAMEX, packet);
		}
	}		
}

void TCPSendDataChar(byte clientID, char *pData)
{
	err_t err;
	int i = 0;
	for(i = 0;i<ClientNum;i++)
	{
		if(Session[i].ClientID == clientID)
		{
			err = netconn_write(Session[i].NetConnSend ,pData,strlen(pData),NETCONN_COPY); //发送数据sizeof(tcp_server_sendbuf)
			if(err != ERR_OK) printf("Send data in TCPSendDataChar Failed,Please check it in DataTransferManage.c \r\n");
		}
	}		
}

void TCPSendDataByte(byte clientID, byte *pData, int dataSize)
{
	err_t err;
	int i = 0;
	for(i = 0;i<ClientNum;i++)
	{
		if(Session[i].ClientID == clientID)
		{
			err = netconn_write(Session[i].NetConnSend , pData, dataSize, NETCONN_COPY); //发送数据sizeof(tcp_server_sendbuf)
			if(err != ERR_OK) printf("Send data in TCPSendDataByte Failed,Please check it in DataTransferManage.c \r\n");
		}
	}		
}

void UDPSendDataBase()
{
	
}

//void TCPSendPacket(byte clientID, struct Packet packet)
//{
//	
//}


//创建SOCKET管理线程
//返回值:0 创建成功
//		其他 创建失败
uint8_t DataTransferManageTask_init(void)
{
	uint8_t res;
	
	taskENTER_CRITICAL();              
	res = xTaskCreate((TaskFunction_t)DataTransferManage,
					(const char*  )"DataTransferManageTask",
					(uint16_t     )DataTransferManage_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )DataTransferManage_TASK_PRIO,
					(TaskHandle_t*)&DataTransferManageTask_Handler);
	taskEXIT_CRITICAL();
	
	return res;
}

static void DataTransferManage(void *arg)
{
	uint32_t data_len = 0;
	byte i, j;
	err_t err;
	int i_cycle;
	struct netbuf *recvbuf;
	struct pbuf *q;
	byte* 	dataRecvBuffer;
	byte* 	dataRecvBufferTemp;
//	byte clientID;
	Packet* pPacketTemp;
	int timeCount = 0;
	int timeCOuntFreeRTOS=0;

	dataRecvBuffer = (byte* )mymalloc(SRAMEX, 256);
	
	while(1)
	{
		//数据接收进程
		if(ClientNum > 0)	//有client接入
		{
			timeCOuntFreeRTOS = xTaskGetTickCount();
			timeCount = OverflowCount_TIM6*65536 + __HAL_TIM_GET_COUNTER(&TIM6_Handler);
			for(i_cycle = 0; i_cycle<ClientNum;i_cycle++)	
			{	
/*************接收数据处理****************************/
				if((netconn_recv(Session[i_cycle].NetConnRecv, &recvbuf)) == ERR_OK)  	//接收到数据
				{						
					taskENTER_CRITICAL();  //关中断	
					data_len = recvbuf->p->tot_len;
					dataRecvBufferTemp = dataRecvBuffer;
					
					for(q=recvbuf->p;q!=NULL;q=q->next)
					{
						memcpy(dataRecvBufferTemp, q->payload, q->len);
						dataRecvBufferTemp = dataRecvBufferTemp + q->len;
					}
					q = NULL;
					dataRecvBufferTemp = NULL;
					taskEXIT_CRITICAL();  //开中断
					
/***********************入队***************************/	
					j = 0;
					while(Session[i_cycle].BufferRecv[j].IsBufferAlive)
					{
						j++;
						if(j > MaxBufferLength)
						{
							printf("BufferRecv is full , waiting to be free \r\n");
							j = 0;
							break;
						}
					}
					mymemcpy(Session[i_cycle].BufferRecv[j].pBufferData, dataRecvBuffer, data_len);
					Session[i_cycle].BufferRecv[j].IsBufferAlive = true;

//					printf("接到数据来自Client[%d]\n", i_cycle);
					printf("%s\r\n", dataRecvBuffer);  //通过串口发送接收到的数据	
					
					data_len=0;  //复制完成后data_len要清零。					
					//myfree(SRAMEX, dataRecvBuffer);
					dataRecvBuffer = NULL;
					netbuf_delete(recvbuf);//一定要加上这一句!!!!不然会内存泄漏！！！
					recvbuf = NULL;
				}
				
/*************发送数据处理****************************/
				for(j = 0;j<MaxBufferLength;j++)
				{
					if(Session[i_cycle].BufferSend[j].IsBufferAlive)
					{
						Session[i_cycle].BufferSend[j].IsBufferAlive = false;
						
						pPacketTemp = (Packet*)Session[i_cycle].BufferSend[j].pBufferData;																																					//包头大小        +   包数据大小     + 包尾大小
						err = netconn_write(Session[i_cycle].NetConnSend ,pPacketTemp,(PACKET_HEADER_SIZE + pPacketTemp->DataSize + 4),NETCONN_COPY); //!!!发送数据sizeof(tcp_server_sendbuf)					
						if(err != ERR_OK) printf("Send data in Failed, use Sessio[%d] bufferPosition[%d]Please check it in DataTransferManage.c ERROR_Code:%d \r\n",i_cycle ,j ,err);						
					}
				}
/**************END*************************************/				
			}			
//////			timeCount = __HAL_TIM_GET_COUNTER(&TIM6_Handler) - timeCount;
//////			timeCOuntFreeRTOS = xTaskGetTickCount() - timeCOuntFreeRTOS;
//////			printf("DataTransferManage Thread Runtime==>%d\r\n", timeCount);// time=32232us
//////			printf("DataTransferManage Thread FreeRTOS==>%d\r\n", timeCOuntFreeRTOS);
		
		}
	vTaskDelay(1);//1ms后再启用
	}
}


