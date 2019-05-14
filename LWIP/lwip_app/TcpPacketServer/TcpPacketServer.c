/**************************************************************************************************
*****************************TCP����˼����߳�******************************************************
***************************************************************************************************/
#include "TcpPacketServer.h"
#include "task.h"

/////////////////////////TCPServer//////////////////////////////////////////////////
uint16_t PortReceive = 20200;	//PortReceive
uint16_t PortSend = 20201;

uint16_t MsIdleTime = 100;
uint8_t ClientNum = 0;				//����Client��Ŀ
bool IsRunning = 0;
bool StopServerListen;
int i_MTU = 1500;

struct PacketServerSession Session[MaxClinets];		//����һ�����ڹ������socket��session

uint32_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
char *tcp_server_sendbuf="Apollo STM32F4/F7 NETCONN TCP Server send data\r\n";	
bool SocketLinkFlag = false;//���ӽ�����־

//�����Դ��Session��
static void addSession(int clientID, int sessionID, struct netconn* netConnRecv, struct netconn* netConnSend)
{
	Session[sessionID].ClientID = clientID;
	Session[sessionID].NetConnRecv = netConnRecv;
	Session[sessionID].NetConnSend = netConnSend;
	
	Session[sessionID].QueueRecv = mymalloc(SRAMEX, sizeof(queue));
	Session[sessionID].QueueSend = mymalloc(SRAMEX, sizeof(queue));
	
	initQueue(&Session[sessionID].QueueRecv, TCP_Queue_MAXBUFSIZE);//Initialing the Recvie buffer queue
	initQueue(&Session[sessionID].QueueSend, TCP_Queue_MAXBUFSIZE);//Initialing the send buffer queue
	printf("Session Client[%d] Add Successful\r\n",clientID);
}


static void TCPServerListenThread(void *arg);//����
//TCP�����������
TaskHandle_t TCPSERVERTask_Handler;

//����TCP������߳�
//����ֵ:0 TCP����˴����ɹ�
//		���� TCP����˴���ʧ��
uint8_t TCPServerListenCycleTask_init(void)
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

//tcp����������
static void TCPServerListenThread(void *arg)
{
	////////////////
	int clientID = 0;	
	uint8_t i_cycle = 0,j_cycle = 0;
	struct netconn* netConnRecv;
	struct netconn* netConnSend;
	struct netbuf*	recvnetbuf;
	//uint8_t *dataReceived;//unsigned char==uint8_t
//	int *pInt;
	static bool clientRepateFlag = false;
	////////////////	
	//static uint32_t data_len = 0;
	//struct pbuf *q;
	err_t err,recv_err,err_SendNetConn;
	uint8_t remot_addr[4];
	struct netconn *conn;	//connΪ����˼���socket
	static ip_addr_t ipaddr;
	static u16_t 			port;
	LWIP_UNUSED_ARG(arg);

	conn=netconn_new(NETCONN_TCP);  //����һ��TCP���ӣ�NETCONN_UDPΪ����UDP����
	netconn_bind(conn,IP_ADDR_ANY,PortReceive);  //�󶨶˿� 5000�Ŷ˿�
	netconn_listen(conn);  		//�������ģʽ����Ϊ����״̬
	conn->recv_timeout = 10;  	//��ֹ�����߳� �ȴ�10ms
	IsRunning = true;
	
	while (!StopServerListen) 				//��ʼ������Ĵ���֮�󣬴��̻߳�һֱѭ��ִ���ⲿ�ִ���
	{
		if(ClientNum<8)//��󴴽�8��
		{
			//�������Ӳ������µ�Receive Socket
			err = netconn_accept(conn,&netConnRecv);  //������������,ɨ���Ƿ������ӣ�����conn������
																												//�����������򴴽�һ���µ����ӣ��浽NetConn��������ڽ������Socketͨ��ͨ��
			if (err == ERR_OK)    //���������ӵ�����
			{	
				//xSemaphoreGive(xSemaphore[ClientNum]);	//��TcpPacketClientһ���ź���,Inspection��Server��IP�Ͷ˿ںš���������Inspection
				
				netConnRecv->recv_timeout = 10;
				netconn_getaddr(netConnRecv,&ipaddr,&port,0); //��ȡԶ��IP��ַ�Ͷ˿ں�
				remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
				remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
				remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
				remot_addr[0] = (uint8_t)(ipaddr.addr);
				printf("����%d.%d.%d.%d�����Ϸ�����,Զ�������ͻ��˶˿ں�Ϊ:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);				
				

				//����Ƿ����ظ���client����
				for(j_cycle =0;j_cycle<ClientNum;j_cycle++)
				{
					if(Session[ClientNum].ClientID == remot_addr[3]) //������ӷ�������IP�Ƿ�Ϊͬһ��
					{
						//�ͷ����µ�netConnRecv
						printf("Create a Repate Session IP= 192.168.66.%d, ClientID= %d , Please check it\r\n", remot_addr[3], remot_addr[3]);
						netconn_close(netConnRecv);
						netconn_delete(netConnRecv);
						clientRepateFlag = true;
						break;//ʹ��breakҪ�������ĸ�ѭ������������while���ɱ������
					}
				}

				if(clientRepateFlag != true)
				{
					//����һ���µ�Send Socket
					//��һ��������Inspectionʹ������socket��NetConnRecv��NetConnSend���ֱ������շ�...
					netConnSend			=	netconn_new(NETCONN_TCP);  //����һ��TCP����
					err_SendNetConn = netconn_connect(netConnSend,&ipaddr,PortSend);//���ӷ�����20201�˿�
					
					if (err_SendNetConn == ERR_OK)    //���������ӵ�����
					{
						printf("NetConnSend[%d]==>Establish Successful\n", ClientNum);
						//��client��netConnSend, netConnRecv�ȼ��뵽Session��
						addSession(remot_addr[3], ClientNum, netConnRecv, netConnSend);	//clientIDȡ���ӷ�������IP��ַ�����һ��	��192.168.66.11 �е�11
						ClientNum++;	//�洢��һ��Client��Socket	
						printf("The number of ClientNum==>%d\r\n", ClientNum);//���ڴ�ClientNum��0��ʼ������ClientNum++����������ӵ�Client��Ŀ
						
					}
					else
					{
						netconn_delete(netConnSend); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
						printf("TCP_Server Connect Failed!!!==>error code ::[%d]\n", err_SendNetConn);
					}
				}					
			clientRepateFlag = false;			//��������ȷ����һ����������������
			}			
		}			
	
		//����Ƿ��жϿ�����
		//�˴�����δ��netbuf_delete(recvnetbuf);�������ڴ�й©������
		for(i_cycle = 0;i_cycle<ClientNum;i_cycle++)
		{
			if(Session[i_cycle].NetConnRecv != NULL)
			{
				recv_err = netconn_recv(Session[i_cycle].NetConnRecv,&recvnetbuf);
				if(recv_err == ERR_CLSD)
				{
					netconn_close	(Session[i_cycle].NetConnRecv);
					netconn_close	(Session[i_cycle].NetConnSend);
					netconn_delete(Session[i_cycle].NetConnRecv);
					netconn_delete(Session[i_cycle].NetConnSend);
					destroyQueue 	(Session[i_cycle].QueueRecv);
					destroyQueue 	(Session[i_cycle].QueueSend);
					//printf("����:%d.%d.%d.%d�Ͽ��������������\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					printf("�����Ͽ�XXX�����ӣ�======>%d", Session[i_cycle].ClientID);
					
					for(j_cycle = i_cycle;j_cycle<ClientNum-1;j_cycle++)//����Session���飬ɾ����Ӧ��client��Ӧ��Sessionp[i_cycle],ͬʱClientNum��һ������ǰ��
					{
						Session[j_cycle] = Session[j_cycle + 1]; 
					}
					ClientNum--;//ɾ��һ��session
				}
				netbuf_delete(recvnetbuf);//һ��Ҫ������һ��!!!!��Ȼ���ڴ�й©������
				recvnetbuf = NULL;
			}
		}	
	vTaskDelay(100);  	//ÿ��1000ms��ɨ��һ���Ƿ��ж˿ڽ���
	}
	
	netconn_delete(conn);//�رռ����˿�
	IsRunning = false;
}


