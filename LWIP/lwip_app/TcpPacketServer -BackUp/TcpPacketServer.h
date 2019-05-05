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

#define TCP_SERVER_RX_BUFSIZE	2000		//定义tcp server最大接收数据长度
#define TCP_SERVER_PORT			5000	//定义tcp server的端口
#define LWIP_SEND_DATA			0X80	//定义有数据发送

extern u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
extern u8 tcp_server_flag;			//TCP服务器数据发送标志位

uint8_t TCPSetverListenCycleInit(void);		//TCP服务器初始化(创建TCP服务器线程)
#endif

