//nclude "TcpPacketClient.h"
//#include "lwip/opt.h"
//#include "lwip_comm.h"
//#include <string.h>
//#include "delay.h"
//#include "lwip/lwip_sys.h"
//#include "lwip/api.h" 
//#include "FreeRTOS.h"
//#include "task.h"
//#include "string.h"
//#include "TaskManage.h"		//统一管理优先级和堆栈大小
//#include "TcpPacketServer.h"

////struct netconn* NetConnSend[MaxClinets];		//定义一个netconn结构体数组,即socket通道数组

//char *host = NULL;
//char *bindIPAddress = NULL;
//int clientID;
//int portSend;
//int portReceive;

//int msTimeOut= 1000;
//int msIdleTime = 100;

//uint8_t stopConnection;
//uint8_t autoConnect = 0;
//uint16_t autoConnectInterval = 2000;

//uint8_t NETBUS_txBuffer[1000];
//uint32_t NETBUS_txLength=100;
//struct netconn *tcp_clientconn=NULL;					//TCP CLIENT网络连接结构体
//u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
//u8 tcp_client_flag;		//TCP客户端数据发送标志位

////TCP Client任务
//TaskHandle_t TCPCLIENTTask_Handler;

//struct Packet
//{
//	const int HEADER_SIZE;//16
//	const int TAILER_SIZE;//4
//	
//	int BeginWord;
//  int PacketID;			//used as priority
//  int PacketType;
//  int DataBytes;
//  char* pData;
//  int EndWord;
//	int PacketSize;
////  int GetPacketFromBuffer(char* pBuffer, int len);
//};


////tcp客户端任务函数
//void tcp_client_thread(void *arg)
//{
//	u32 data_len = 0;
//	struct pbuf *q;
//	err_t err,recv_err;
//	static ip_addr_t server_ipaddr,loca_ipaddr;
//	static u16_t 		 server_port,loca_port;

//	LWIP_UNUSED_ARG(arg);
//	server_port = REMOTE_PORT;
//	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
//	
//	while (1) 
//	{
//		tcp_clientconn=netconn_new(NETCONN_TCP);  //创建一个TCP链接
//    err = netconn_connect(tcp_clientconn,&server_ipaddr,server_port);//连接服务器
//		if(err != ERR_OK)
//		{
//			netconn_delete(tcp_clientconn); //返回值不等于ERR_OK,删除tcp_clientconn连接
//			printf("TCP_Server Connect Failed!!!");
//		}
//		else if (err == ERR_OK)    //处理新连接的数据
//		{ 
//			struct netbuf *recvbuf;
//			tcp_clientconn->recv_timeout = 10;
//			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //获取本地IP主机IP地址和端口号
//			printf("连接上服务器%d.%d.%d.%d,本机端口号为:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
//			while(1)
//			{
//				if((tcp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
//				{
//					err = netconn_write(tcp_clientconn ,NETBUS_txBuffer,NETBUS_txLength,NETCONN_COPY); //发送数据
//					if(err != ERR_OK)
//					{
//						printf("发送失败\r\n");
//					}
//					tcp_client_flag &= ~LWIP_SEND_DATA;
//				}
//					
//				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //接收到数据
//				{	
//					taskENTER_CRITICAL();
//					memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //数据接收缓冲区清零
//					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
//					{
//						//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
//						//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
//						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//拷贝数据
//						else memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
//						data_len += q->len;  	
//						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
//					}
//					taskEXIT_CRITICAL();
//					data_len=0;  //复制完成后data_len要清零。					
//					printf("Received Buffer==>%s\r\n",tcp_client_recvbuf);
//					//NETBUS_Respond(tcp_client_recvbuf);
//					netbuf_delete(recvbuf);
//				}else if(recv_err == ERR_CLSD)  //关闭连接
//				{
//					netconn_delete(tcp_clientconn);
//					netconn_delete(tcp_clientconn);
//					printf("服务器%d.%d.%d.%d删除连接\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
//					break;
//				}
//			}
//		}
//	}
//}

////创建TCP客户端线程
////返回值:0 TCP客户端创建成功
////		其他 TCP客户端创建失败
//uint8_t tcp_client_init(void)
//{
//	uint8_t res;
//	
//	taskENTER_CRITICAL();              
//	res = xTaskCreate((TaskFunction_t)tcp_client_thread,
//					(const char*  )"tcp_client_task",
//					(uint16_t     )TCPCLIENT_STK_SIZE,
//					(void*        )NULL,
//					(UBaseType_t  )TCPCLIENT_TASK_PRIO,
//					(TaskHandle_t*)&TCPCLIENTTask_Handler);
//	taskEXIT_CRITICAL();
//	
//	return res;
//}
