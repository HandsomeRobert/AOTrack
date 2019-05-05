#include "TcpPacketServer.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include <string.h>
#include "delay.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h" 
#include "FreeRTOS.h"
#include "task.h"

//////////////////////TCPServer//////////////////////////////////////////////////
uint16_t PortSend = 5001;
uint16_t PortReceive = 5000;	//PortReceive

uint16_t MsIdleTime = 100;
static uint16_t ClientNum = 0 ;				//定义Client数目
uint8_t IsRunning = 0;
uint8_t StopDaemon;

#define MaxClinets  8						//最大允许多少个Client接入

struct netconn* NetConn[MaxClinets];		//定义一个netconn结构体数组,即socket通道数组

///////////////////////////////////////////////////////////////////////

u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
char *tcp_server_sendbuf="Apollo STM32F4/F7 NETCONN TCP Server send data\r\n";	
u8 tcp_server_flag;								//TCP服务器数据发送标志位

//TCP客户端任务
#define TCPSERVER_TASK_PRIO		6
//任务堆栈大小
#define TCPSERVER_STK_SIZE	300
//任务句柄
TaskHandle_t TCPSERVERTask_Handler;

//tcp服务器任务
static void TCPServerListenThread(void *arg)
{
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	u8 remot_addr[4];
	struct netconn *conn, *newconn;	//
	static ip_addr_t ipaddr;
	static u16_t 			port;
	
	LWIP_UNUSED_ARG(arg);

	conn=netconn_new(NETCONN_TCP);  //创建一个TCP链接，NETCONN_UDP为创建UDP连接
	netconn_bind(conn,IP_ADDR_ANY,TCP_SERVER_PORT);  //绑定端口 5000号端口
	netconn_listen(conn);  		//进入监听模式，设为侦听状态
	conn->recv_timeout = 10;  	//禁止阻塞线程 等待10ms
	while (1) 
	{
		err = netconn_accept(conn,&newconn);  //接收连接请求,扫描是否有连接，利用conn监听，有连接请求则创建一个新的连接，即newconn
		if(err==ERR_OK)
			newconn->recv_timeout = 10;
		

		if (err == ERR_OK)    //处理新连接的数据
		{ 
			struct netbuf *recvbuf;

			netconn_getaddr(newconn,&ipaddr,&port,0); //获取远端IP地址和端口号
			
			remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
			remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
			remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
			remot_addr[0] = (uint8_t)(ipaddr.addr);
			printf("主机%d.%d.%d.%d连接上服务器,主机端口号为:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
			
			while(1)
			{
				if((tcp_server_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
				{
					err = netconn_write(newconn ,tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf),NETCONN_COPY); //发送tcp_server_sendbuf中的数据
					if(err != ERR_OK)
					{
						printf("发送失败\r\n");
					}
					tcp_server_flag &= ~LWIP_SEND_DATA;
				}
				
				if((recv_err = netconn_recv(newconn,&recvbuf)) == ERR_OK)  	//接收到数据 并将数据封装在netbuf中。若接收到一条空消息则表示对方已关闭当前连接，server也该关闭此连接
				{		
					taskENTER_CRITICAL();  //关中断
					memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
					{
						//判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//拷贝数据
						else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_SERVER_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
					}
					taskEXIT_CRITICAL();  //开中断
					data_len=0;  //复制完成后data_len要清零。	
					printf("[The Received Buffer]%s\r\n",tcp_server_recvbuf);  //通过串口发送接收到的数据
					netbuf_delete(recvbuf);
					
//					ClientNum++;					//存储下一个Client
//					if(ClientNum > 8)
//						ClientNum = 0;
				}
				
				else if(recv_err == ERR_CLSD)  //关闭连接
				{
					netconn_close(newconn);
					netconn_delete(newconn);
					printf("主机:%d.%d.%d.%d断开与服务器的连接\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					break;
//					if(ClientNum >0)
//						ClientNum--;
				}
			}
		}
	}
}

//创建TCP服务端线程
//返回值:0 TCP服务端创建成功
//		其他 TCP服务端创建失败
uint8_t TCPSetverListenCycleInit(void)
{
	uint8_t res;
	taskENTER_CRITICAL();              
	res = xTaskCreate((TaskFunction_t)TCPServerListenThread,
					(const char*  )"TCPServerListenThread",
					(uint16_t     )TCPSERVER_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )TCPSERVER_TASK_PRIO,
					(TaskHandle_t*)&TCPSERVERTask_Handler);
	taskEXIT_CRITICAL();

	return res;
}


