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

uint8_t DataTransferManage_recvbuf[DATA_RX_BUFSIZE];	//TCP客户端接收数据缓冲区

char *DataTransferManage_SendBuf="Apollo STM32F4/F7 NETCONN TCP Server send data\r\n";	
uint8_t DataSendFlag;

TaskHandle_t DataTransferManageTask_Handler;		//任务句柄
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
			err = netconn_write(Session[i].NetConnSend ,pData,strlen(pData),NETCONN_COPY); //发送数据sizeof(tcp_server_sendbuf)
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
		//数据接收进程
		if(ClientNum > 0)	//有client接入
		{
			for(i_cycle = 0; i_cycle<ClientNum;i_cycle++)	
			{	
				if((recv_err = netconn_recv(Session[i_cycle].NetConnRecv, &recvbuf)) == ERR_OK)  	//接收到数据
				{	
					netconn_getaddr(Session[i_cycle].NetConnRecv,&ipaddr,&port,0); 	//获取远端IP地址和端口号					
					remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
					remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
					remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
					remot_addr[0] = (uint8_t)(ipaddr.addr);
					
					taskENTER_CRITICAL();  //关中断
					
					memset(DataTransferManage_recvbuf,0,DATA_RX_BUFSIZE);  //数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表,数据是void *payload，一个空指针
					{
						//判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (DATA_RX_BUFSIZE-data_len)) memcpy(DataTransferManage_recvbuf+data_len,q->payload,(DATA_RX_BUFSIZE-data_len));//拷贝数据
						else memcpy(DataTransferManage_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;
						if(data_len > DATA_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
					}
					taskEXIT_CRITICAL();  //开中断
					
					if(enQueue(Session[i_cycle].QueueRecv, DataTransferManage_recvbuf, data_len))		//接收到的数据放入缓冲区
					{
						printf("Inset Element to ReceiveBuffer[%d] queue successful! \r\n", i_cycle);
					}
					data_len=0;  //复制完成后data_len要清零。
					printf("接到数据来自Client[%d]\n", i_cycle);
					printf("%s\r\n",DataTransferManage_recvbuf);  //通过串口发送接收到的数据
					netbuf_delete(recvbuf);//一定要加上这一句!!!!不然会内存泄漏！！！
				}
			}			
		}
	vTaskDelay(100);//100ms后再启用
	}
}
