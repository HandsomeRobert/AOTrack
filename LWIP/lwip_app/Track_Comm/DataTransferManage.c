/**************************************************************************************************
**************************�����շ������߳�******************************************************
***************************************************************************************************/

#include "DataTransferManage.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "TcpPacketServer.h"
#include "stdbool.h"

uint8_t DataTransferManage_recvbuf[DATA_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����

char *DataTransferManage_SendBuf="Apollo STM32F4/F7 NETCONN TCP Server send data\r\n";	
uint8_t DataSendFlag;

TaskHandle_t DataTransferManageTask_Handler;		//������
static void DataTransferManage(void *arg);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPSendPacket(byte clientID, Packet* packet)
{
	err_t err;
	int i = 0;
	for(i = 0;i<ClientNum;i++)
	{
		if(Session[i].ClientID == clientID)
		{
			err = netconn_write(Session[i].NetConnSend ,packet,packet->TotalDataSize,NETCONN_COPY); //!!!��������sizeof(tcp_server_sendbuf)
			if(err != ERR_OK) printf("Send data in TCPSendDataChar Failed,Please check it in DataTransferManage.c \r\n");
			myfree(SRAMEX, packet);
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
			err = netconn_write(Session[i].NetConnSend ,pData,strlen(pData),NETCONN_COPY); //��������sizeof(tcp_server_sendbuf)
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
			err = netconn_write(Session[i].NetConnSend , pData, dataSize, NETCONN_COPY); //��������sizeof(tcp_server_sendbuf)
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


//����SOCKET�����߳�
//����ֵ:0 �����ɹ�
//		���� ����ʧ��
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
	int i_cycle;
	struct netbuf *recvbuf;
	struct pbuf *q;
	byte* dataRecvBuffer;
	byte* dataRecvBufferTemp;
//	static err_t recv_err;
//	static ip_addr_t ipaddr;
//	static u16_t 			port;
//	u8 remot_addr[4];
	
	while(1)
	{
		//���ݽ��ս���
		if(ClientNum > 0)	//��client����
		{
			for(i_cycle = 0; i_cycle<ClientNum;i_cycle++)	
			{	
				if((netconn_recv(Session[i_cycle].NetConnRecv, &recvbuf)) == ERR_OK)  	//���յ�����
				{	
/*					
					netconn_getaddr(Session[i_cycle].NetConnRecv,&ipaddr,&port,0); 	//��ȡԶ��IP��ַ�Ͷ˿ں�					
					remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
					remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
					remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
					remot_addr[0] = (uint8_t)(ipaddr.addr);
*/					
					
					taskENTER_CRITICAL();  //���ж�	
					data_len = recvbuf->p->tot_len;
					dataRecvBuffer = (byte* )mymalloc(SRAMEX, data_len);
					dataRecvBufferTemp = dataRecvBuffer;
					
					for(q=recvbuf->p;q!=NULL;q=q->next)
					{
						memcpy(dataRecvBufferTemp, q->payload, q->len);
						dataRecvBufferTemp = dataRecvBufferTemp + q->len;
					}
					q = NULL;
					dataRecvBufferTemp = NULL;
					taskEXIT_CRITICAL();  //���ж�
								
//��������			/*��Ӳ����ᵼ��Xispekvision�ظ���/��TCP Connection*/ ����Ϊ����SOCKET����ķ������ݣ�112��112��112.������������������������������������
					if(enQueue(Session[i_cycle].QueueRecv, DataTransferManage_recvbuf, data_len))		//���յ������ݷ��뻺����
					{
//						printf("Inset Element to ReceiveBuffer[%d] queue successful! \r\n", i_cycle);
					}
					

//					printf("�ӵ���������Client[%d]\n", i_cycle);
					printf("%s\r\n",dataRecvBuffer);  //ͨ�����ڷ��ͽ��յ�������	
					
					data_len=0;  //������ɺ�data_lenҪ���㡣					
					myfree(SRAMEX, dataRecvBuffer);
					dataRecvBuffer = NULL;
					netbuf_delete(recvbuf);//һ��Ҫ������һ��!!!!��Ȼ���ڴ�й©������
					recvbuf = NULL;
				}
			}			
		}
	vTaskDelay(100);//100ms��������
	}
}


