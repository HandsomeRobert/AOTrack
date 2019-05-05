/**************************************************************************************************
***********本文件用于统一管理信号量的定义和创建******************************************************
***************************************************************************************************/

#include "SemaphoreManage.h"


//定义二值信号量
xSemaphoreHandle OnConnected;            //when connected to the server
xSemaphoreHandle OnDisconnected;         //wehen disconnected to the server
xSemaphoreHandle OnPacketSendFinish;     //after packet is sent 
xSemaphoreHandle OnPacketSendError;      //when a packet is not sent correctly
xSemaphoreHandle OnPacketArrive;         //when a packet arrives
xSemaphoreHandle OnServerStarted;        //when the server is started    
xSemaphoreHandle OnServerStopped;				 //when the server is stoped
xSemaphoreHandle OnSysRestart;					 //Reboot the system

xSemaphoreHandle OnLoadParametersFromPC; //从PC加载控制参数

xSemaphoreHandle xSemaphore[8];//定义信号量数组....	

void CreateSemaphore(void)
{
	int i = 0;
	//创建信号量
	OnConnected 				= xSemaphoreCreateBinary();			//这里不能使用vSemaphoreCreateBinary,否则会导致意外的信号量为1
	OnDisconnected 			= xSemaphoreCreateBinary();
	OnPacketSendFinish 	= xSemaphoreCreateBinary();
	OnPacketSendError 	= xSemaphoreCreateBinary();
	OnPacketArrive 			= xSemaphoreCreateBinary();
	OnServerStarted 		= xSemaphoreCreateBinary();
	OnServerStopped 		= xSemaphoreCreateBinary();
	
	OnSysRestart 				= xSemaphoreCreateBinary();
	OnLoadParametersFromPC = xSemaphoreCreateBinary();
	
	for(i = 0;i<8;i++)
	{
		xSemaphore[i] = xSemaphoreCreateBinary();
	}
}
