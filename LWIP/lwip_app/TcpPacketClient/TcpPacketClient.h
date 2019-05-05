#ifndef TCPPACKETCLIENT_H
#define TCPPACKETCLIENT_H
#include "sys.h"   
#include "TcpPacketServer.h"

#define TCP_CLIENT_RX_BUFSIZE	1500	//���ջ���������
#define REMOTE_PORT				8087	//����Զ�������Ķ˿ں�
#define LWIP_SEND_DATA			0X80    //���������ݷ���

extern u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
extern u8 tcp_client_flag;		    //TCP�ͻ������ݷ��ͱ�־λ
//extern struct netconn* NetConnSend[MaxClinets];

uint8_t tcp_client_init(void);  //tcp�ͻ��˳�ʼ��(����tcp�ͻ����߳�)

#endif

