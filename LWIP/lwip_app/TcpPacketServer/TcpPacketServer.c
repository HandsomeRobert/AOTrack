/**************************************************************************************************
*****************************TCP����˼����߳�******************************************************
***************************************************************************************************/
#include "TcpPacketServer.h"
#include "task.h"
#include "TCPProtocol.h"
#include "DataTransferManage.h"
#include "timer.h"

/////////////////////////TCPServer//////////////////////////////////////////////////
uint16_t PortReceive = 20200;	//PortReceive
uint16_t PortSend = 20201;

uint16_t MsIdleTime = 100;
uint8_t ClientNum = 0;				//����Client��Ŀ
bool IsRunning = 0;
bool StopServerListen;
int i_MTU = 1500;

struct PacketServerSession Session[MaxClients];		//����һ�����ڹ������socket��session

uint32_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
bool SocketLinkFlag = false;//���ӽ�����־

//�����Դ��Session��
static void addSession(int clientID, int sessionID, struct netconn* netConnRecv, struct netconn* netConnSend)
{
	byte i = 0;
	Session[sessionID].ClientID = clientID;
	Session[sessionID].NetConnRecv = netConnRecv;
	Session[sessionID].NetConnSend = netConnSend;
	Session[sessionID].NetConnRecv->recv_timeout = 2;//recv_timeout Cannot be set to 0

	Session[sessionID].BufferRecvArea = mymalloc(SRAMEX, 1280);//10*128, 10 commands

	for(i = 0; i < MaxBufferLength; i++)
	{
		Session[sessionID].BufferSend[i].pBufferData = mymalloc(SRAMEX, 128);
		Session[sessionID].BufferSend[i].IsBufferAlive = false;
	}
	
//	initQueue(&Session[sessionID].QueueRecv);//Initialing the Recvie buffer queue
//	initQueue(&Session[sessionID].QueueSend);//Initialing the send buffer queue
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
	uint8_t i_cycle = 0,j_cycle = 0;
	int data_len = 0;
	static int ClientIDArray[MaxClients] = {0};
	struct netconn* netConnRecvTemp;
	struct netconn* netConnSendTemp;
	struct netbuf*	recvnetbuf;
	static bool clientRepateFlag = false;

	struct netbuf *recvbuf;
	byte* 	dataRecvBuffer;
	byte* 	dataRecvBufferTemp;
	struct pbuf *q;
	int* pInt;
	byte clientID = 0;//��PC��ȡ����ClientID
	
	err_t err,recv_err,err_SendNetConn;
	uint8_t remot_addr[4];
	struct netconn *ServerConn;	//ServerConnΪ����˼���socket
	ip_addr_t ipaddr;
	static u16_t 			port;
	Packet* pPacket = mymalloc(SRAMEX, 256);	//���ݰ��ݴ�
	
//	LWIP_UNUSED_ARG(arg);

	ServerConn = netconn_new(NETCONN_TCP);  //����һ��TCP���ӣ�NETCONN_UDPΪ����UDP����
	netconn_bind(ServerConn, IP_ADDR_ANY, PortReceive);  //�󶨶˿� 20200�Ŷ˿�  
	netconn_listen(ServerConn);  					//�������ģʽ����Ϊ����״̬
	ServerConn->recv_timeout = 10;  			//��ֹ�����߳� �ȴ�10ms
	IsRunning = true;
	
	while (!StopServerListen) 				//��ʼ������Ĵ���֮�󣬴��̻߳�һֱѭ��ִ���ⲿ�ִ���
	{
		if(ClientNum < MaxClients)//��󴴽�8��
		{
			//�������Ӳ������µ�Receive Socket
			err = netconn_accept(ServerConn,&netConnRecvTemp);  //������������,ɨ���Ƿ������ӣ�����ServerConn������
																							//�����������򴴽�һ���µ����ӣ��浽NetConn��������ڽ������Socketͨ��ͨ��
			if (err == ERR_OK)    //���������ӵ�����
			{	
				netconn_getaddr(netConnRecvTemp,&ipaddr,&port,0); //��ȡԶ��IP��ַ�Ͷ˿ں�
				remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
				remot_addr[2] = (uint8_t)(ipaddr.addr >> 16);
				remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
				remot_addr[0] = (uint8_t)(ipaddr.addr);
				printf("����%d.%d.%d.%d�����Ϸ�����,Զ�������ͻ��˶˿ں�Ϊ:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);				
		
				dataRecvBuffer = (byte* )mymalloc(SRAMEX, 256);	
				//xSemaphoreGive(xSemaphore[ClientNum]);	//��TcpPacketClientһ���ź���,Inspection��Server��IP�Ͷ˿ںš���������Inspection
				netConnRecvTemp->recv_timeout = 10;
				//���շ����Ķ˿���Ϣ
				if((netconn_recv(netConnRecvTemp, &recvbuf)) == ERR_OK)  	//���յ�����
				{
					taskENTER_CRITICAL();  //���ж�	
					data_len = recvbuf->p->tot_len;		
					dataRecvBufferTemp = dataRecvBuffer;					
					for(q=recvbuf->p;q!=NULL;q=q->next)
					{
						memcpy(dataRecvBufferTemp, q->payload, q->len);
						dataRecvBufferTemp = dataRecvBufferTemp + q->len;
					}	
					q = NULL;
					dataRecvBufferTemp = NULL;
					netbuf_delete(recvbuf);//һ��Ҫ������һ��!!!!��Ȼ���ղ�������
				  recvbuf = NULL;
					taskEXIT_CRITICAL();  //���ж�
				}		
				
				clientID = *dataRecvBuffer;
				for(i_cycle = 0; i_cycle<data_len;i_cycle++)//��ӡ���յ�������
				{
					printf("%x", *dataRecvBuffer++);
				}
				myfree(SRAMEX, dataRecvBuffer);
				data_len = 0;
				
				//����Ƿ����ظ���client����
				for(j_cycle =0;j_cycle<ClientNum;j_cycle++)
				{
					if(Session[j_cycle].ClientID == clientID) //������ӷ�������IP�Ƿ�Ϊͬһ��
					{
						//�ͷ����µ�netConnRecvTemp
						printf("Create a Repate Session IP= 192.168.66.%d, ClientID= %d , Please check it\r\n", remot_addr[3], remot_addr[3]);
						netconn_close(netConnRecvTemp);
						netconn_delete(netConnRecvTemp);
						clientRepateFlag = true;
						break;//ʹ��breakҪ�������ĸ�ѭ������������while���ɱ������
					}
				}

				if(clientRepateFlag != true)
				{
					//����һ���µ�Send Socket
					//��һ��������Inspectionʹ������socket��NetConnRecv��NetConnSend���ֱ������շ�...
					netConnSendTemp	=	netconn_new(NETCONN_TCP);  //����һ��TCP����
					err_SendNetConn = netconn_connect(netConnSendTemp,&ipaddr,PortSend);//���ӷ�����20201�˿�
					
					if (err_SendNetConn == ERR_OK)    //���������ӵ�����
					{
						printf("NetConnSend ClientID[%d]==>Establish Successful\n", clientID);
						//��client��netConnSendTemp, netConnRecvTemp�ȼ��뵽Session��
						addSession(clientID, ClientNum, netConnRecvTemp, netConnSendTemp);	//clientIDȡ���ӷ�������IP��ַ�����һ��	��192.168.66.11 �е�11
						ClientIDArray[ClientNum] = clientID;
						ClientNum++;	//�洢��һ��Client��Socket	
						printf("The number of ClientNum==>%d\r\n", ClientNum);//���ڴ�ClientNum��0��ʼ������ClientNum++����������ӵ�Client��Ŀ
					}
					else
					{
						netconn_delete(netConnSendTemp); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
						printf("TCP_Server Connect Failed!!!==>error code ::[%d]\n", err_SendNetConn);
					}
				
					clientID = 0;
/*��Ǹ��ٿ���������*/
					pPacket  = CreateStartTrackingPacket(pPacket, 1, 0);
					data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
					i_cycle = 0;
					while(Session[i_cycle].ClientID != ClientServer)
					{
						i_cycle++;
						if(i_cycle > ClientNum)
						{
							printf("Cannot find the session ClientServer[%d]\r\n", ClientServer);
							i_cycle = 0;
							break;
						}
					}			
					WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);

				}					
			clientRepateFlag = false;			//��������ȷ����һ����������������			
//////			pPacket  = CreateClientIDPacket(pPacket, ClientIDArray, ClientNum-1);	//�����������˵�Client��ID���͸�PC,PacketServer
//////			data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;//��ʱ24us
//////			i_cycle = 0;
//////			while(Session[i_cycle].ClientID != ClientServer)
//////			{
//////				i_cycle++;
//////				if(i_cycle > ClientNum)
//////				{
//////					printf("In TcpPacketServer.c Cannot find the session ClientServer[%d]\r\n", ClientServer);
//////					i_cycle = 0;
//////					break;
//////				}
//////			}			
//////			WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);
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
					for(j_cycle = 0; j_cycle<MaxBufferLength;j_cycle++)
					{
						myfree(SRAMEX, Session[i_cycle].BufferRecvArea);
						myfree(SRAMEX, Session[i_cycle].BufferSend[j_cycle].pBufferData);
						Session[i_cycle].BufferSend[j_cycle].IsBufferAlive = false;
					}
					printf("STM32�������Ͽ�Client[XXX]�����ӣ�======>%d", Session[i_cycle].ClientID);
					Session[i_cycle].ClientID = 0;			//�ͻ��˿�����
					
					for(j_cycle = i_cycle;j_cycle<ClientNum-1;j_cycle++)//����Session���飬ɾ����Ӧ��client��Ӧ��Sessionp[i_cycle],ͬʱClientNum��һ������ǰ��
					{
						Session[j_cycle] 				= Session[j_cycle + 1];
						ClientIDArray[j_cycle]	=	ClientIDArray[j_cycle + 1];
					}
					ClientNum--;//ɾ��һ��session
				}
				netbuf_delete(recvnetbuf);//һ��Ҫ������һ��!!!!��Ȼ���ڴ�й©������
				recvnetbuf = NULL;
			}
		}	
	vTaskDelay(100);  	//ÿ��1000ms��ɨ��һ���Ƿ��ж˿ڽ���
	}
	netconn_close(ServerConn);
	netconn_delete(ServerConn);//�رռ����˿�
	IsRunning = false;
}


