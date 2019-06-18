#ifndef __TCPPACKETSERVER_H
#define __TCPPACKETSERVER_H
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include "stdbool.h"

#include "lwip/opt.h"
#include "lwip_comm.h"
#include <string.h>
#include "delay.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h" 
#include "FreeRTOS.h"
#include "TaskManage.h"		//统一管理优先级和堆栈大小
#include "SemaphoreManage.h"
#include "TCPQueue.h"

#define TCP_SERVER_RX_BUFSIZE	2000		//定义tcp server最大接收数据长度
#define TCP_Queue_MAXBUFSIZE  1500
//#define TCP_SERVER_PORT			6666	//定义tcp server的端口
#define LWIP_SEND_DATA			0X80	//定义有数据发送

#define TCP_Server_RX_BUFSIZE 1500

#define MaxClients  		8						//最大允许多少个Client接入
#define MaxBufferLength 10

typedef struct
{
	bool IsBufferAlive;
	byte* pBufferData;
}StcBufferElem;

struct PacketServerSession
{
	byte ClientID;
	struct netconn* NetConnRecv;
	struct netconn* NetConnSend;
//	StcBufferElem BufferRecv[MaxBufferLength];							//Receive buffer area
	byte* BufferRecvArea;																		//Receive  area
	StcBufferElem BufferSend[MaxBufferLength];							//Send buffer area
	//TransferProtocol, TransferMode, TransferPriority
	
};


extern uint16_t PortSend;
extern uint16_t PortReceive;
extern uint8_t ClientNum;
extern int i_MTU;

extern bool IsRunning;
extern bool StopServerListen;

extern struct PacketServerSession Session[MaxClients];		//定义一个用于管理接收socket的session
extern xSemaphoreHandle xSemaphore[MaxClients];
extern TaskHandle_t TCPSERVERTask_Handler;

extern uint32_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP客户端接收数据缓冲区

extern bool SocketLinkFlag;	//是否有连接建立标志

uint8_t TCPServerListenCycleTask_init(void);		//TCP服务器初始化(创建TCP服务器线程)
#endif

