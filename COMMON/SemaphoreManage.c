/**************************************************************************************************
***********���ļ�����ͳһ�����ź����Ķ���ʹ���******************************************************
***************************************************************************************************/

#include "SemaphoreManage.h"


//�����ֵ�ź���
xSemaphoreHandle OnConnected;            //when connected to the server
xSemaphoreHandle OnDisconnected;         //wehen disconnected to the server
xSemaphoreHandle OnPacketSendFinish;     //after packet is sent 
xSemaphoreHandle OnPacketSendError;      //when a packet is not sent correctly
xSemaphoreHandle OnPacketArrive;         //when a packet arrives
xSemaphoreHandle OnServerStarted;        //when the server is started    
xSemaphoreHandle OnServerStopped;				 //when the server is stoped
xSemaphoreHandle OnSysRestart;					 //Reboot the system

xSemaphoreHandle OnLoadParametersFromPC; //��PC���ؿ��Ʋ���

xSemaphoreHandle xSemaphore[8];//�����ź�������....	

void CreateSemaphore(void)
{
	int i = 0;
	//�����ź���
	OnConnected 				= xSemaphoreCreateBinary();			//���ﲻ��ʹ��vSemaphoreCreateBinary,����ᵼ��������ź���Ϊ1
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
