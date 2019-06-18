/**************************************************************************************************
*****************************TCP服务端监听线程******************************************************
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
uint8_t ClientNum = 0;				//定义Client数目
bool IsRunning = 0;
bool StopServerListen;
int i_MTU = 1500;

struct PacketServerSession Session[MaxClients];		//定义一个用于管理接收socket的session

uint32_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
bool SocketLinkFlag = false;//连接建立标志

//添加资源到Session里
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


static void TCPServerListenThread(void *arg);//声明
//TCP服务端任务句柄
TaskHandle_t TCPSERVERTask_Handler;

//创建TCP服务端线程
//返回值:0 TCP服务端创建成功
//		其他 TCP服务端创建失败
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

//tcp服务器任务
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
	byte clientID = 0;//从PC获取到的ClientID
	
	err_t err,recv_err,err_SendNetConn;
	uint8_t remot_addr[4];
	struct netconn *ServerConn;	//ServerConn为服务端监听socket
	ip_addr_t ipaddr;
	static u16_t 			port;
	Packet* pPacket = mymalloc(SRAMEX, 256);	//数据包暂存
	
//	LWIP_UNUSED_ARG(arg);

	ServerConn = netconn_new(NETCONN_TCP);  //创建一个TCP链接，NETCONN_UDP为创建UDP连接
	netconn_bind(ServerConn, IP_ADDR_ANY, PortReceive);  //绑定端口 20200号端口  
	netconn_listen(ServerConn);  					//进入监听模式，设为侦听状态
	ServerConn->recv_timeout = 10;  			//禁止阻塞线程 等待10ms
	IsRunning = true;
	
	while (!StopServerListen) 				//初始化上面的代码之后，此线程会一直循环执行这部分代码
	{
		if(ClientNum < MaxClients)//最大创建8个
		{
			//接收连接并创建新的Receive Socket
			err = netconn_accept(ServerConn,&netConnRecvTemp);  //接收连接请求,扫描是否有连接，利用ServerConn监听，
																							//有连接请求则创建一个新的连接，存到NetConn数组里，用于建立多个Socket通道通信
			if (err == ERR_OK)    //处理新连接的数据
			{	
				netconn_getaddr(netConnRecvTemp,&ipaddr,&port,0); //获取远端IP地址和端口号
				remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
				remot_addr[2] = (uint8_t)(ipaddr.addr >> 16);
				remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
				remot_addr[0] = (uint8_t)(ipaddr.addr);
				printf("主机%d.%d.%d.%d连接上服务器,远程主机客户端端口号为:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);				
		
				dataRecvBuffer = (byte* )mymalloc(SRAMEX, 256);	
				//xSemaphoreGive(xSemaphore[ClientNum]);	//给TcpPacketClient一个信号量,Inspection的Server的IP和端口号。用于连接Inspection
				netConnRecvTemp->recv_timeout = 10;
				//接收发来的端口信息
				if((netconn_recv(netConnRecvTemp, &recvbuf)) == ERR_OK)  	//接收到数据
				{
					taskENTER_CRITICAL();  //关中断	
					data_len = recvbuf->p->tot_len;		
					dataRecvBufferTemp = dataRecvBuffer;					
					for(q=recvbuf->p;q!=NULL;q=q->next)
					{
						memcpy(dataRecvBufferTemp, q->payload, q->len);
						dataRecvBufferTemp = dataRecvBufferTemp + q->len;
					}	
					q = NULL;
					dataRecvBufferTemp = NULL;
					netbuf_delete(recvbuf);//一定要加上这一句!!!!不然会收不到数据
				  recvbuf = NULL;
					taskEXIT_CRITICAL();  //开中断
				}		
				
				clientID = *dataRecvBuffer;
				for(i_cycle = 0; i_cycle<data_len;i_cycle++)//打印接收到的数据
				{
					printf("%x", *dataRecvBuffer++);
				}
				myfree(SRAMEX, dataRecvBuffer);
				data_len = 0;
				
				//检查是否有重复的client连接
				for(j_cycle =0;j_cycle<ClientNum;j_cycle++)
				{
					if(Session[j_cycle].ClientID == clientID) //检测连接服务器的IP是否为同一个
					{
						//释放最新的netConnRecvTemp
						printf("Create a Repate Session IP= 192.168.66.%d, ClientID= %d , Please check it\r\n", remot_addr[3], remot_addr[3]);
						netconn_close(netConnRecvTemp);
						netconn_delete(netConnRecvTemp);
						clientRepateFlag = true;
						break;//使用break要看跳出哪个循环，卵用跳出while后会杀死进程
					}
				}

				if(clientRepateFlag != true)
				{
					//创建一个新的Send Socket
					//即一个检测程序Inspection使用两条socket（NetConnRecv和NetConnSend）分别用于收发...
					netConnSendTemp	=	netconn_new(NETCONN_TCP);  //创建一个TCP链接
					err_SendNetConn = netconn_connect(netConnSendTemp,&ipaddr,PortSend);//连接服务器20201端口
					
					if (err_SendNetConn == ERR_OK)    //处理新连接的数据
					{
						printf("NetConnSend ClientID[%d]==>Establish Successful\n", clientID);
						//将client，netConnSendTemp, netConnRecvTemp等加入到Session里
						addSession(clientID, ClientNum, netConnRecvTemp, netConnSendTemp);	//clientID取连接服务器的IP地址的最后一段	即192.168.66.11 中的11
						ClientIDArray[ClientNum] = clientID;
						ClientNum++;	//存储下一组Client的Socket	
						printf("The number of ClientNum==>%d\r\n", ClientNum);//由于从ClientNum从0开始，所以ClientNum++后才是所连接的Client数目
					}
					else
					{
						netconn_delete(netConnSendTemp); //返回值不等于ERR_OK,删除tcp_clientconn连接
						printf("TCP_Server Connect Failed!!!==>error code ::[%d]\n", err_SendNetConn);
					}
				
					clientID = 0;
/*标记跟踪控制器启动*/
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
			clientRepateFlag = false;			//清零用于确保下一次能正常建立连接			
//////			pPacket  = CreateClientIDPacket(pPacket, ClientIDArray, ClientNum-1);	//将所有连接了的Client和ID发送给PC,PacketServer
//////			data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;//耗时24us
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
	
		//检测是否有断开连接
		//此处由于未加netbuf_delete(recvnetbuf);发生过内存泄漏！！！
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
					printf("STM32控制器断开Client[XXX]的连接：======>%d", Session[i_cycle].ClientID);
					Session[i_cycle].ClientID = 0;			//客户端口清零
					
					for(j_cycle = i_cycle;j_cycle<ClientNum-1;j_cycle++)//管理Session数组，删除相应的client对应的Sessionp[i_cycle],同时ClientNum减一，数组前移
					{
						Session[j_cycle] 				= Session[j_cycle + 1];
						ClientIDArray[j_cycle]	=	ClientIDArray[j_cycle + 1];
					}
					ClientNum--;//删除一组session
				}
				netbuf_delete(recvnetbuf);//一定要加上这一句!!!!不然会内存泄漏！！！
				recvnetbuf = NULL;
			}
		}	
	vTaskDelay(100);  	//每隔1000ms秒扫描一次是否有端口接入
	}
	netconn_close(ServerConn);
	netconn_delete(ServerConn);//关闭监听端口
	IsRunning = false;
}


