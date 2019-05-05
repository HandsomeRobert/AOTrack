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
void TCPSendDataBase(byte clientID, char *pData)
{
	err_t err;
	int i = 0;
	for(i = 0;i<ClientNum;i++)
	{
		if(Session[i].ClientID == clientID)
		{
			err = netconn_write(Session[i].NetConnSend ,pData,strlen(pData),NETCONN_COPY); //��������sizeof(tcp_server_sendbuf)
			if(err != ERR_OK) printf("Send data Failed,Please check it in DataTransferManage.c \r\n");
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
	static err_t recv_err;
	uint32_t data_len = 0;
	int i_cycle;
	struct netbuf *recvbuf;
	struct pbuf *q;
	static ip_addr_t ipaddr;
	static u16_t 			port;
	u8 remot_addr[4];
	
	while(1)
	{
		//���ݽ��ս���
		if(ClientNum > 0)	//��client����
		{
			for(i_cycle = 0; i_cycle<ClientNum;i_cycle++)	
			{	
				if((recv_err = netconn_recv(Session[i_cycle].NetConnRecv, &recvbuf)) == ERR_OK)  	//���յ�����
				{	
					netconn_getaddr(Session[i_cycle].NetConnRecv,&ipaddr,&port,0); 	//��ȡԶ��IP��ַ�Ͷ˿ں�					
					remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
					remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
					remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
					remot_addr[0] = (uint8_t)(ipaddr.addr);
					
					taskENTER_CRITICAL();  //���ж�
					
					memset(DataTransferManage_recvbuf,0,DATA_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����,������void *payload��һ����ָ��
					{
						//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (DATA_RX_BUFSIZE-data_len)) memcpy(DataTransferManage_recvbuf+data_len,q->payload,(DATA_RX_BUFSIZE-data_len));//��������
						else memcpy(DataTransferManage_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;
						if(data_len > DATA_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					taskEXIT_CRITICAL();  //���ж�
					
					if(enQueue(Session[i_cycle].QueueRecv, DataTransferManage_recvbuf, data_len))		//���յ������ݷ��뻺����
					{
						printf("Inset Element to ReceiveBuffer[%d] queue successful! \r\n", i_cycle);
					}
					data_len=0;  //������ɺ�data_lenҪ���㡣
					printf("�ӵ���������Client[%d]\n", i_cycle);
					printf("%s\r\n",DataTransferManage_recvbuf);  //ͨ�����ڷ��ͽ��յ�������
					netbuf_delete(recvbuf);//һ��Ҫ������һ��!!!!��Ȼ���ڴ�й©������
				}
			}			
		}
	vTaskDelay(100);//100ms��������
	}
}
