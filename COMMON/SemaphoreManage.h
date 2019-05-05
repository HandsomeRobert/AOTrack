#ifndef __SemaphoreManage_H
#define __SemaphoreManage_H
/**************************************************************************************************
***********���ļ�����ͳһ�����ź����Ķ���ʹ���******************************************************
***************************************************************************************************/

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "stdbool.h"
//#include "TcpPacketServer.h"

extern xSemaphoreHandle xSemaphore[8];//�����ź�������....

extern xSemaphoreHandle OnConnected;            //when connected to the server
extern xSemaphoreHandle OnDisconnected;         //wehen disconnected to the server
extern xSemaphoreHandle OnPacketSendFinish;     //after packet is sent 
extern xSemaphoreHandle OnPacketSendError;      //when a packet is not sent correctly
extern xSemaphoreHandle OnPacketArrive;         //when a packet arrives
extern xSemaphoreHandle OnServerStarted;        //when the server is started    
extern xSemaphoreHandle OnServerStopped;				 //when the server is stoped

extern xSemaphoreHandle OnSysRestart;							//����ϵͳ
extern xSemaphoreHandle OnLoadParametersFromPC; //��PC���ؿ��Ʋ���

void CreateSemaphore(void);

#endif

