#ifndef __UDPPACKET_H
#define __UDPPACKET_H
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
 
#define UDP_DEMO_RX_BUFSIZE		2000	//����udp���������ݳ���
#define UDP_DEMO_PORT			8089	//����udp���ӵı��ض˿ں�
#define LWIP_SEND_DATA			0X80    //���������ݷ���

extern u8 udp_flag;		//UDP���ݷ��ͱ�־λ

uint8_t udp_demo_init(void);		//udp demo��ʼ��������UDP�߳�
#endif

