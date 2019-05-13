/********************************************************************
 * COPYRIGHT -- XISPEK
 ********************************************************************
 * File: TCPProtocol.c
 * Author: Chen Qi
 * Created: May 7, 2019
 * Usage:	Connect With XispekVision Protocol
 *******************************************************************/
#include "TCPProtocol.h"
#include "DataProcess.h"
#include "malloc.h"

void SetValueOffset(char* pDestination, int offset, int value)
{
	pDestination = pDestination + offset;
	*(int*)pDestination = value;
}

//STM32 <---> PC, twoway
//StartTracking: [objectID]
//	0:  Start
//	1:  Stop
Packet* CreateStartTrackingPacket(int lineID, int objectID)
{
	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 0;
	pPacket->PacketType	= 1;
	pPacket->DataSize		= COMMAND_HEADER_SIZE;
	
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;

	pCommand = (Command*)pPacket->pData;
	pCommand->LineID 		= lineID;
	pCommand->ActionID 	= PCCmdActionStartTracking;
	pCommand->ObjectID 	= objectID;
	pCommand->DataSize 	= 0;
	
	offset = PACKET_HEADER_SIZE + COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pPacket, offset, IntEndWord);	

	return pPacket;
}

//PLC ---> PC
//flag: Create flag
//	0: object that is taken over
//	1: object that is normal created
//	2: object that is created in diagnostics mode
Packet* CreateObjectRunInPacket(int lineID, int objectID, int moduleID, int encoder, int flag, int lastTriggerPreviousEncoder,int lastReceivePreviousEncoder)
{
	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;

	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 10;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE + 12;
	
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;
	
	pCommand = (Command*)pPacket->pData;
	pCommand->ActionID 	= PCCmdActionObjectRunIn;
	pCommand->LineID 		= lineID;
	pCommand->ObjectID 	= objectID;
	pCommand->ModuleID 	= moduleID;
	pCommand->Encoder 	= encoder;
	pCommand->DataSize 	= 12;	
	
	offset = PACKET_HEADER_SIZE + COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pPacket, offset, flag);offset += 4;
	SetValueOffset((char*)pPacket, offset, lastTriggerPreviousEncoder);offset += 4;
	SetValueOffset((char*)pPacket, offset, lastReceivePreviousEncoder);offset += 4;
	SetValueOffset((char*)pPacket, offset, IntEndWord);	
	
	return pPacket;
}


Packet* CreateObjectRunOutPacket(int lineID, int objectID, int moduleID, int encoder, int destModuleID)
{
	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;

	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 10;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE + 4;
		
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;
		
	pCommand = (Command*)pPacket->pData;
		
	pCommand->ActionID 	= PCCmdActionObjectRunIn;
	pCommand->LineID 		= lineID;
	pCommand->ObjectID 	= objectID;
	pCommand->ModuleID 	= moduleID;
	pCommand->Encoder 	= encoder;			
	pCommand->DataSize 	= 4;
	
	offset = COMMAND_HEADER_SIZE + PACKET_HEADER_SIZE;
	SetValueOffset((char*)pPacket, offset, destModuleID);offset += 4;
	SetValueOffset((char*)pPacket, offset, IntEndWord);

	return pPacket;
}

Packet* CreateObjectDeletePacket(int lineID, int objectID, int moduleID, int encoder)
{
	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;

	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 10;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE;
			
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;

	pCommand = (Command*)pPacket->pData;
			
	pCommand->ActionID 	= PCCmdActionObjectRunIn;
	pCommand->LineID 		= lineID;
	pCommand->ObjectID 	= objectID;
	pCommand->ModuleID 	= moduleID;
	pCommand->Encoder 	= encoder;
	pCommand->DataSize 	= 0;
	
	offset = COMMAND_HEADER_SIZE + PACKET_HEADER_SIZE;
	SetValueOffset((char*)pPacket, offset, IntEndWord);
	
	return pPacket;
}
	

Packet* CreateTriggerCameraPacket(int lineID, int objectID, int moduleID, int encoder, int cameraID, int imageIndex)
{
	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;

	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 20;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE + 8;
			
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;

	pCommand = (Command*)pPacket->pData;
			
	pCommand->ActionID 	= PCCmdActionObjectRunIn;
	pCommand->LineID 		= lineID;
	pCommand->ObjectID 	= objectID;
	pCommand->ModuleID 	= moduleID;
	pCommand->Encoder 	= encoder;
	pCommand->DataSize 	= 8;

	offset = COMMAND_HEADER_SIZE + PACKET_HEADER_SIZE;
	SetValueOffset((char*)pPacket, offset, cameraID);offset += 4;
	SetValueOffset((char*)pPacket, offset, imageIndex);offset += 4;
	SetValueOffset((char*)pPacket, offset, IntEndWord);

	return pPacket;
}
	
