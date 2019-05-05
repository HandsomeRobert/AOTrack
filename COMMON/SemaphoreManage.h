#ifndef __SemaphoreManage_H
#define __SemaphoreManage_H
/**************************************************************************************************
***********本文件用于统一管理信号量的定义和创建******************************************************
***************************************************************************************************/

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "stdbool.h"
//#include "TcpPacketServer.h"

extern xSemaphoreHandle xSemaphore[8];//定义信号量数组....

extern xSemaphoreHandle OnConnected;            //when connected to the server
extern xSemaphoreHandle OnDisconnected;         //wehen disconnected to the server
extern xSemaphoreHandle OnPacketSendFinish;     //after packet is sent 
extern xSemaphoreHandle OnPacketSendError;      //when a packet is not sent correctly
extern xSemaphoreHandle OnPacketArrive;         //when a packet arrives
extern xSemaphoreHandle OnServerStarted;        //when the server is started    
extern xSemaphoreHandle OnServerStopped;				 //when the server is stoped

extern xSemaphoreHandle OnSysRestart;							//重启系统
extern xSemaphoreHandle OnLoadParametersFromPC; //从PC加载控制参数

void CreateSemaphore(void);

#endif

