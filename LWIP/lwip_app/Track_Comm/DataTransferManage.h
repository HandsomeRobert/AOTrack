/**************************************************************************************************
**************************�����շ������߳�******************************************************
***************************************************************************************************/
#ifndef __DATATRANSFERMANAGE_H
#define __DATATRANSFERMANAGE_H

#include "FreeRTOS.h"
#include "TaskManage.h"
#include "TCPProtocol.h"

#define LWIP_SEND_DATA			0X80    //���������ݷ���
#define DATA_RX_BUFSIZE			2000		//����tcp server���������ݳ���
#define DATA_PORT						8088		//����tcp server�Ķ˿�
#define	ClientServer				10

extern uint8_t DataSendFlag;


//void Start();
//void Stop();
//void SendPacket(int clientID, struct Packet packet);
//void SendCommand1(struct ITCPTransferable command);
//void SendCommand2(int clientID, struct ITCPTransferable command);
//void SendCommand3(int clientID, int packetID, struct ITCPTransferable command);
//bool IsConnected(int clientID);

uint8_t DataTransferManageTask_init(void);
void TCPSendDataChar(byte clientID, char* pData);
void TCPSendDataByte(byte clientID, byte* pData, int dataSize);
void TCPSendPacket	(byte clientID, Packet* packet);

#endif
