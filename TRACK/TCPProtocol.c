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

#define COMMAND_HEADER_SIZE 20
#define PACKET_HEADER_SIZE 	20

void CreateStartTrackingPacket(Packet* pPacket, int lineID, int objectID)
{
	Command* pCommand;
	
	pPacket->BeginWord 	= 0x47424B50;
	pPacket->PacketID 	= 0;
	pPacket->PacketType	= 1;
	pPacket->DataSize	= COMMAND_HEADER_SIZE;
	
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;

	pCommand = (Command*)pPacket->pData;
	pCommand->LineID = lineID;
	pCommand->ActionID = PCCmdActionStartTracking;
	pCommand->ObjectID = objectID;
	pCommand->DataSize = 0;
	
	pPacket->EndWord = 0x44454B50;
}


void CreateObjectRunInPacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int flag, int lastTriggerPreviousEncoder,int lastReceivePreviousEncoder)
{
		Command* pCommand; 
		int offset = COMMAND_HEADER_SIZE;
	
		pPacket->BeginWord = 0x47424B50;
		pPacket->PacketID = 10;
		pPacket->PacketType = 1;
		pPacket->DataSize = COMMAND_HEADER_SIZE + 12;
		
		pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;
 
		pCommand = (Command*)pPacket->pData;
		
		pCommand->ActionID = PCCmdActionObjectRunIn;
		pCommand->LineID = lineID;
		pCommand->ObjectID = objectID;
		pCommand->ModuleID = moduleID;
		pCommand->Encoder = encoder;
	


		pCommand->DataSize = 12;	
//		pCommand->SetValue(offset, flag); offset += 4;
//		pCommand->SetValue(offset, lastTriggerPreviousEncoder); offset += 4;
//		pCommand->SetValue(offset, lastReceivePreviousEncoder); offset += 4;
		
		pPacket->EndWord = 0x44454B50;
		
}

void CreateObjectRunOutPacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int destModuleID)
{
		Command* pCommand;
		int offset = COMMAND_HEADER_SIZE;
	
		pPacket->BeginWord = 0x47424B50;
		pPacket->PacketID = 10;
		pPacket->PacketType = 1;
		pPacket->DataSize = COMMAND_HEADER_SIZE + 4;
			
		pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;
		  
		pCommand = (Command*)pPacket->pData;
			
		pCommand->ActionID = PCCmdActionObjectRunIn;
		pCommand->LineID = lineID;
		pCommand->ObjectID = objectID;
		pCommand->ModuleID = moduleID;
		pCommand->Encoder = encoder;
			
		
		pCommand->DataSize = 4;
//		pCommand->SetValue(offset, destModuleID); offset += 4;
			
		pPacket->EndWord = 0x44454B50;
	}

void CreateObjectDeletePacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder)
{
		Command* pCommand; 
	
		pPacket->BeginWord = 0x47424B50;
		pPacket->PacketID = 10;
		pPacket->PacketType = 1;
		pPacket->DataSize = COMMAND_HEADER_SIZE;
				
		pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;

		pCommand = (Command*)pPacket->pData;
				
		pCommand->ActionID = PCCmdActionObjectRunIn;
		pCommand->LineID = lineID;
		pCommand->ObjectID = objectID;
		pCommand->ModuleID = moduleID;
		pCommand->Encoder = encoder;
		pCommand->DataSize = 0;
		
//		pCommand->SetValue(Command::HEADER_SIZE, (int)0x44454B50);
		pPacket->EndWord = 0x44454B50;		

	}
	

void CreateTriggerCameraPacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int cameraID, int imageIndex)
{
		Command* pCommand;  
		int offset = COMMAND_HEADER_SIZE;
	
		pPacket->BeginWord = 0x47424B50;
		pPacket->PacketID = 20;
		pPacket->PacketType = 1;
		pPacket->DataSize = COMMAND_HEADER_SIZE + 8;
				
		pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;

		pCommand = (Command*)pPacket->pData;
				
		pCommand->ActionID = PCCmdActionObjectRunIn;
		pCommand->LineID = lineID;
		pCommand->ObjectID = objectID;
		pCommand->ModuleID = moduleID;
		pCommand->Encoder = encoder;
				
		
		pCommand->DataSize = 8;
//		pCommand->SetValue(offset, cameraID); offset += 4;
//		pCommand->SetValue(offset, imageIndex); offset += 4;
		pPacket->EndWord = 0x44454B50;
				
	}
	
