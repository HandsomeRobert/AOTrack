#ifndef __TCPPACKETSERVER_H
#define __TCPPACKETSERVER_H
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

 
//////////////////////
#define TCP_Server_RX_BUFSIZE 1500
extern uint16_t PortSend;
extern uint16_t PortReceive;
extern uint16_t MaxClinets;
extern uint16_t MsIdleTime;
extern uint16_t ClientNum;

extern uint8_t IsRunning;
extern uint8_t StopDaemon;
//////////////////////

#define TCP_SERVER_RX_BUFSIZE	2000		//����tcp server���������ݳ���
#define TCP_SERVER_PORT			5000	//����tcp server�Ķ˿�
#define LWIP_SEND_DATA			0X80	//���������ݷ���

extern u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
extern u8 tcp_server_flag;			//TCP���������ݷ��ͱ�־λ

uint8_t TCPSetverListenCycleInit(void);		//TCP��������ʼ��(����TCP�������߳�)
#endif

