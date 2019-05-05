/**************************************************************************************************
**************************数据收发管理线程******************************************************
***************************************************************************************************/
#ifndef __DATATRANSFERMANAGE_H
#define __DATATRANSFERMANAGE_H

#include "FreeRTOS.h"
#include "TaskManage.h"

#define LWIP_SEND_DATA			0X80    //定义有数据发送
#define DATA_RX_BUFSIZE	2000		//定义tcp server最大接收数据长度
#define DATA_PORT			8088	//定义tcp server的端口

extern uint8_t DataSendFlag;

struct Packet
{
	const int HEADER_SIZE ;
	const int TAILER_SIZE ;

	int BeginWord;
	int PacketID;			//used as priority
	int PacketType;
	int DataBytes;
	char* pData;
	int EndWord;

//	int GetPacketSize();
//	//return the packet size, 0 if no complete packet, -1 if format is wrong
//	int GetPacketFromBuffer(char* pBuffer, int len);
};
//struct Command
//{

//};
//struct ITCPTransferable
//{
//	
//};


//void Start();
//void Stop();
//void SendPacket(int clientID, struct Packet packet);
//void SendCommand1(struct ITCPTransferable command);
//void SendCommand2(int clientID, struct ITCPTransferable command);
//void SendCommand3(int clientID, int packetID, struct ITCPTransferable command);
//bool IsConnected(int clientID);

uint8_t DataTransferManageTask_init(void);
void TCPSendDataBase(byte clientID, char *pData);

#endif
