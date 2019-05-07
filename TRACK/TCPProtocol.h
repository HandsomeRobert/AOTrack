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

typedef struct
{
	int LineID;
	int ModuleID;
	int ActionID;
	int ObjectID;
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
	
	char* pData;
	int EndWord;
	
}Packet;

#endif

