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
#include "TaskManage.h"		//ͳһ�������ȼ��Ͷ�ջ��С
#include "SemaphoreManage.h"
#include "TCPQueue.h"

struct PacketServerSession
{
	byte ClientID;
	struct netconn* NetConnRecv;
	struct netconn* NetConnSend;
	queue* QueueRecv;							//Receive buffer area
	queue* QueueSend;							//Send buffer area
	//TransferProtocol, TransferMode, TransferPriority
	
};

#define TCP_SERVER_RX_BUFSIZE	2000		//����tcp server���������ݳ���
#define TCP_Queue_MAXBUFSIZE  1500
//#define TCP_SERVER_PORT			6666	//����tcp server�Ķ˿�
#define LWIP_SEND_DATA			0X80	//���������ݷ���

#define TCP_Server_RX_BUFSIZE 1500
#define MaxClinets  8						//����������ٸ�Client����
extern uint16_t PortSend;
extern uint16_t PortReceive;
extern uint8_t ClientNum;
extern int i_MTU;

extern bool IsRunning;
extern bool StopServerListen;

extern struct PacketServerSession Session[MaxClinets];		//����һ�����ڹ�������socket��session
extern xSemaphoreHandle xSemaphore[MaxClinets];
extern TaskHandle_t TCPSERVERTask_Handler;

extern uint32_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����

extern bool SocketLinkFlag;	//�Ƿ������ӽ�����־

uint8_t TCPServerListenCycleTask_init(void);		//TCP��������ʼ��(����TCP�������߳�)
#endif
