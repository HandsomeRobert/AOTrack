/**************************************************************************************************
*****************************TCP服务端监听线程******************************************************
***************************************************************************************************/
#include "TcpPacketServer.h"
#include "task.h"

/////////////////////////TCPServer//////////////////////////////////////////////////
uint16_t PortReceive = 20200;	//PortReceive
uint16_t PortSend = 20201;

uint16_t MsIdleTime = 100;
uint8_t ClientNum = 0;				//定义Client数目
bool IsRunning = 0;
bool StopServerListen;
int i_MTU = 1500;

struct PacketServerSession Session[MaxClinets];		//定义一个用于管理接收socket的session

uint32_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
char *tcp_server_sendbuf="Apollo STM32F4/F7 NETCONN TCP Server send data\r\n";	
bool SocketLinkFlag = false;//连接建立标志

//添加资源到Session里
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
	struct netconn *conn;	//conn为服务端监听socket
	static ip_addr_t ipaddr;
	static u16_t 			port;
	LWIP_UNUSED_ARG(arg);

	conn=netconn_new(NETCONN_TCP);  //创建一个TCP链接，NETCONN_UDP为创建UDP连接
	netconn_bind(conn,IP_ADDR_ANY,PortReceive);  //绑定端口 5000号端口
	netconn_listen(conn);  		//进入监听模式，设为侦听状态
	conn->recv_timeout = 10;  	//禁止阻塞线程 等待10ms
	IsRunning = true;
	
	while (!StopServerListen) 				//初始化上面的代码之后，此线程会一直循环执行这部分代码
	{
		if(ClientNum<8)//最大创建8个
		{
			//接收连接并创建新的Receive Socket
			err = netconn_accept(conn,&netConnRecv);  //接收连接请求,扫描是否有连接，利用conn监听，
																												//有连接请求则创建一个新的连接，存到NetConn数组里，用于建立多个Socket通道通信
			if (err == ERR_OK)    //处理新连接的数据
			{	
				//xSemaphoreGive(xSemaphore[ClientNum]);	//给TcpPacketClient一个信号量,Inspection的Server的IP和端口号。用于连接Inspection
				
				netConnRecv->recv_timeout = 10;
				netconn_getaddr(netConnRecv,&ipaddr,&port,0); //获取远端IP地址和端口号
				remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
				remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
				remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
				remot_addr[0] = (uint8_t)(ipaddr.addr);
				printf("主机%d.%d.%d.%d连接上服务器,远程主机客户端端口号为:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);				
				

				//检查是否有重复的client连接
				for(j_cycle =0;j_cycle<ClientNum;j_cycle++)
				{
					if(Session[ClientNum].ClientID == remot_addr[3]) //检测连接服务器的IP是否为同一个
					{
						//释放最新的netConnRecv
						printf("Create a Repate Session IP= 192.168.66.%d, ClientID= %d , Please check it\r\n", remot_addr[3], remot_addr[3]);
						netconn_close(netConnRecv);
						netconn_delete(netConnRecv);
						clientRepateFlag = true;
						break;//使用break要看跳出哪个循环，卵用跳出while后会杀死进程
					}
				}

				if(clientRepateFlag != true)
				{
					//创建一个新的Send Socket
					//即一个检测程序Inspection使用两条socket（NetConnRecv和NetConnSend）分别用于收发...
					netConnSend			=	netconn_new(NETCONN_TCP);  //创建一个TCP链接
					err_SendNetConn = netconn_connect(netConnSend,&ipaddr,PortSend);//连接服务器20201端口
					
					if (err_SendNetConn == ERR_OK)    //处理新连接的数据
					{
						printf("NetConnSend[%d]==>Establish Successful\n", ClientNum);
						//将client，netConnSend, netConnRecv等加入到Session里
						addSession(remot_addr[3], ClientNum, netConnRecv, netConnSend);	//clientID取连接服务器的IP地址的最后一段	即192.168.66.11 中的11
						ClientNum++;	//存储下一组Client的Socket	
						printf("The number of ClientNum==>%d\r\n", ClientNum);//由于从ClientNum从0开始，所以ClientNum++后才是所连接的Client数目
						
					}
					else
					{
						netconn_delete(netConnSend); //返回值不等于ERR_OK,删除tcp_clientconn连接
						printf("TCP_Server Connect Failed!!!==>error code ::[%d]\n", err_SendNetConn);
					}
				}					
			clientRepateFlag = false;			//清零用于确保下一次能正常建立连接
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
					destroyQueue 	(Session[i_cycle].QueueRecv);
					destroyQueue 	(Session[i_cycle].QueueSend);
					//printf("主机:%d.%d.%d.%d断开与服务器的连接\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					printf("主机断开XXX的连接：======>%d", Session[i_cycle].ClientID);
					
					for(j_cycle = i_cycle;j_cycle<ClientNum-1;j_cycle++)//管理Session数组，删除相应的client对应的Sessionp[i_cycle],同时ClientNum减一，数组前移
					{
						Session[j_cycle] = Session[j_cycle + 1]; 
					}
					ClientNum--;//删除一组session
				}
				netbuf_delete(recvnetbuf);//一定要加上这一句!!!!不然会内存泄漏！！！
				recvnetbuf = NULL;
			}
		}	
	vTaskDelay(100);  	//每隔1000ms秒扫描一次是否有端口接入
	}
	
	netconn_delete(conn);//关闭监听端口
	IsRunning = false;
}


