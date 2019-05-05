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
static uint16_t ClientNum = 0 ;				//����Client��Ŀ
uint8_t IsRunning = 0;
uint8_t StopDaemon;

#define MaxClinets  8						//���������ٸ�Client����

struct netconn* NetConn[MaxClinets];		//����һ��netconn�ṹ������,��socketͨ������

///////////////////////////////////////////////////////////////////////

u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
char *tcp_server_sendbuf="Apollo STM32F4/F7 NETCONN TCP Server send data\r\n";	
u8 tcp_server_flag;								//TCP���������ݷ��ͱ�־λ

//TCP�ͻ�������
#define TCPSERVER_TASK_PRIO		6
//�����ջ��С
#define TCPSERVER_STK_SIZE	300
//������
TaskHandle_t TCPSERVERTask_Handler;

//tcp����������
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

	conn=netconn_new(NETCONN_TCP);  //����һ��TCP���ӣ�NETCONN_UDPΪ����UDP����
	netconn_bind(conn,IP_ADDR_ANY,TCP_SERVER_PORT);  //�󶨶˿� 5000�Ŷ˿�
	netconn_listen(conn);  		//�������ģʽ����Ϊ����״̬
	conn->recv_timeout = 10;  	//��ֹ�����߳� �ȴ�10ms
	while (1) 
	{
		err = netconn_accept(conn,&newconn);  //������������,ɨ���Ƿ������ӣ�����conn�����������������򴴽�һ���µ����ӣ���newconn
		if(err==ERR_OK)
			newconn->recv_timeout = 10;
		

		if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;

			netconn_getaddr(newconn,&ipaddr,&port,0); //��ȡԶ��IP��ַ�Ͷ˿ں�
			
			remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
			remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
			remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
			remot_addr[0] = (uint8_t)(ipaddr.addr);
			printf("����%d.%d.%d.%d�����Ϸ�����,�����˿ں�Ϊ:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
			
			while(1)
			{
				if((tcp_server_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					err = netconn_write(newconn ,tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf),NETCONN_COPY); //����tcp_server_sendbuf�е�����
					if(err != ERR_OK)
					{
						printf("����ʧ��\r\n");
					}
					tcp_server_flag &= ~LWIP_SEND_DATA;
				}
				
				if((recv_err = netconn_recv(newconn,&recvbuf)) == ERR_OK)  	//���յ����� �������ݷ�װ��netbuf�С������յ�һ������Ϣ���ʾ�Է��ѹرյ�ǰ���ӣ�serverҲ�ùرմ�����
				{		
					taskENTER_CRITICAL();  //���ж�
					memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
						else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_SERVER_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					taskEXIT_CRITICAL();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣	
					printf("[The Received Buffer]%s\r\n",tcp_server_recvbuf);  //ͨ�����ڷ��ͽ��յ�������
					netbuf_delete(recvbuf);
					
//					ClientNum++;					//�洢��һ��Client
//					if(ClientNum > 8)
//						ClientNum = 0;
				}
				
				else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(newconn);
					netconn_delete(newconn);
					printf("����:%d.%d.%d.%d�Ͽ��������������\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					break;
//					if(ClientNum >0)
//						ClientNum--;
				}
			}
		}
	}
}

//����TCP������߳�
//����ֵ:0 TCP����˴����ɹ�
//		���� TCP����˴���ʧ��
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


