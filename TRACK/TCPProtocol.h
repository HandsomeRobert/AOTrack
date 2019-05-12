/********************************************************************
 * COPYRIGHT -- XISPEK
 ********************************************************************
 * File: TCPProtocol.h
 * Author: Chen Qi
 * Created: May 7, 2019
 * Usage:	Connect With XispekVision Protocol
 *******************************************************************/
#ifndef __TCPPROTOCOL_H
#define __TCPPROTOCOL_H

#define COMMAND_HEADER_SIZE 24
#define PACKET_HEADER_SIZE 	16
#define IntBeginWord				0x47424B50
#define IntEndWord					0x44454B50

typedef struct
{
	int LineID;
	int ActionID;
	int ObjectID;
	int ModuleID;
	int Encoder;
	int DataSize;
	
	char* pData;

}Command;

typedef struct
{
	int BeginWord;
	int PacketID;
	int PacketType;
	int DataSize;
	
	void* pData;
//	int EndWord;
//	int TotalDataSize;
	
}Packet;

/*
**创建各种包
*******/
Packet* CreateStartTrackingPacket(int lineID, int objectID);
Packet* CreateObjectRunInPacket(int lineID, int objectID, int moduleID, int encoder, int flag, int lastTriggerPreviousEncoder,int lastReceivePreviousEncoder);
Packet* CreateObjectRunOutPacket(int lineID, int objectID, int moduleID, int encoder, int destModuleID);
Packet* CreateObjectDeletePacket(int lineID, int objectID, int moduleID, int encoder);
Packet* CreateTriggerCameraPacket(int lineID, int objectID, int moduleID, int encoder, int cameraID, int imageIndex);

#endif

