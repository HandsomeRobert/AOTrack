#ifndef __UDPPACKET_H
#define __UDPPACKET_H
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
 
#define UDP_DEMO_RX_BUFSIZE		2000	//定义udp最大接收数据长度
#define UDP_DEMO_PORT			8089	//定义udp连接的本地端口号
#define LWIP_SEND_DATA			0X80    //定义有数据发送

extern u8 udp_flag;		//UDP数据发送标志位

uint8_t udp_demo_init(void);		//udp demo初始化，创建UDP线程
#endif

