/********************************************************************
 * COPYRIGHT -- XISPEK
 ********************************************************************
 * File: TCPProtocol.c
 * Author: Chen Qi
 * Created: May 7, 2019
 * Usage:	Connect With XispekVision Protocol
 *******************************************************************/
#include "TCPProtocol.h"
#include "malloc.h"

void SetValueOffset(char* pDestination, int offset, int value)
{
	pDestination = pDestination + offset;
	*(int*)pDestination = value;
}

//心跳数据包
Packet* CreateHeartBeatPacket(Packet* pPacket)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 0;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE;
	
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;  
	
	pCommand = (Command*)pPacket->pData;
	pCommand->ActionID = PCCmdActionHeartBeat;
	pCommand->DataSize 	= 0;
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);	
	
	return pPacket;
}

Packet* CreateReconfigurationPacket(Packet* pPacket, int fileID, int result)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord = IntBeginWord;
	pPacket->PacketID = 0;
	pPacket->PacketType = 1;
	pPacket->DataSize = COMMAND_HEADER_SIZE;
	
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;
	pCommand = (Command*)pPacket->pData;	

	pCommand->ActionID 	= PCCmdActionReconfiguration;
	pCommand->ObjectID 	= fileID;
	pCommand->Encoder		= result;
	pCommand->DataSize 	= 0;
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);	
	
	return pPacket;
}


//STM32 <---> PC, twoway
//StartTracking: [objectID]
//	0:  Start
//	1:  Stop
Packet* CreateStartTrackingPacket(Packet* pPacket, int lineID, int objectID)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
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
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);	

	return pPacket;
}

//启动Control
Packet* CreateStartControlPacket(Packet* pPacket, int lineID, int objectID)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 0;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE;
	
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE; 

	pCommand 						= (Command*)pPacket->pData;
	pCommand->LineID 		= lineID;	
	pCommand->ActionID 	= PCCmdActionStartControl;
	pCommand->ObjectID 	= objectID;
	pCommand->DataSize 	= 0;

	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);		
	
	return pPacket;
}

Packet* CreateObjectFalldownPacket(Packet* pPacket, int lineID, int moduleID)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 0;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE;
	
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE;

	pCommand 						= (Command*)pPacket->pData;
	pCommand->LineID 		= lineID;	
	pCommand->ActionID 	= PCCmdActionObjectFallDown;
	pCommand->ModuleID 	= moduleID;
	pCommand->DataSize 	= 0;
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);		
	
	return pPacket;
}

//PC ---> PLC start/stop
//start: [ObjectID]
//	0:	stop
//	1:	start 
Packet* CreateStartIOLivePacket(Packet* pPacket, bool start)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 0;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE;
	
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE;
	
	pCommand 						= (Command*)pPacket->pData;
	pCommand->ActionID 	= PCCmdActionStartIOLive;
	pCommand->ObjectID 	= (int)start;
	pCommand->DataSize 	= 0;	
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);
	
	return pPacket;
}

//PLC ---> PC
Packet* CreateClientIDPacket(Packet* pPacket, int * ConnectClientID,int ConnectIDNum)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	char * pOffset;
//	int offset = 0;
	
	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 0;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE + ConnectIDNum*4;
	
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE;
	
	pCommand 						= (Command*)pPacket->pData;
	pCommand->ActionID 	= PCCmdActionGetConnectedClientID;
	pCommand->DataSize 	= ConnectIDNum*4;
	
//		int offset = Command::HEADER_SIZE;
	pOffset = (char*)pPacket->pData+COMMAND_HEADER_SIZE;
	
	if(ConnectIDNum != 0)	
		mymemcpy(pOffset, (char*)ConnectClientID, ConnectIDNum*4);
	pOffset +=ConnectIDNum*4;
	*((int*)pOffset) =(int)0x44454B50;
	return pPacket;
}

//PLC ---> PC
//messageID [ObjectID]
Packet* CreateErrorMessagePacket(Packet* pPacket, int messageID)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 1;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE;
	
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE;
	
	pCommand = (Command*)pPacket->pData;
	pCommand->ActionID 	= PCCmdActionErrorMessage;
	pCommand->ObjectID 	= messageID;
	pCommand->DataSize 	= 0;
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);		
	
	return pPacket;
}

//PLC <---> PC
//messageID [ObjectID]
//	0:	acknowledge all errors
Packet* CreateErrorAcknowledgePacket(Packet* pPacket, int messageID)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord = IntBeginWord;
	pPacket->PacketID = 1;
	pPacket->PacketType = 1;
	pPacket->DataSize = COMMAND_HEADER_SIZE;
	
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;  
	
	pCommand = (Command*)pPacket->pData;
	
	pCommand->ActionID 	= PCCmdActionErrorAcknowledge;
	pCommand->ObjectID 	= messageID;
	pCommand->DataSize 		= 0;
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);		
	
	return pPacket;
}


//PLC ---> PC
//flag: Create flag
//	0: object that is taken over
//	1: object that is normal created
//	2: object that is created in diagnostics mode
Packet* CreateObjectRunInPacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int flag, int lastTriggerPreviousEncoder,int lastReceivePreviousEncoder)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
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
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, flag);offset += 4;
	SetValueOffset((char*)pCommand, offset, lastTriggerPreviousEncoder);offset += 4;
	SetValueOffset((char*)pCommand, offset, lastReceivePreviousEncoder);offset += 4;
	SetValueOffset((char*)pCommand, offset, IntEndWord);	
	
	return pPacket;
}


Packet* CreateObjectRunOutPacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int destModuleID)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;

	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 10;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE + 4;
		
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;
		
	pCommand = (Command*)pPacket->pData;
		
	pCommand->ActionID 	= PCCmdActionObjectRunOut;
	pCommand->LineID 		= lineID;
	pCommand->ObjectID 	= objectID;
	pCommand->ModuleID 	= moduleID;
	pCommand->Encoder 	= encoder;			
	pCommand->DataSize 	= 4;
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, destModuleID);offset += 4;
	SetValueOffset((char*)pCommand, offset, IntEndWord);

	return pPacket;
}

Packet* CreateObjectDeletePacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;

	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 10;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE;
			
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;

	pCommand = (Command*)pPacket->pData;
			
	pCommand->ActionID 	= PCCmdActionObjectDelete;
	pCommand->LineID 		= lineID;
	pCommand->ObjectID 	= objectID;
	pCommand->ModuleID 	= moduleID;
	pCommand->Encoder 	= encoder;
	pCommand->DataSize 	= 0;
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);
	
	return pPacket;
}
	

Packet* CreateTriggerCameraPacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int cameraID, int imageIndex)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;

	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 20;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE + 8;
			
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;

	pCommand = (Command*)pPacket->pData;
			
	pCommand->ActionID 	= PCCmdActionTriggerCamera;
	pCommand->LineID 		= lineID;
	pCommand->ObjectID 	= objectID;
	pCommand->ModuleID 	= moduleID;
	pCommand->Encoder 	= encoder;
	pCommand->DataSize 	= 8;

	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, cameraID);offset += 4;
	SetValueOffset((char*)pCommand, offset, imageIndex);offset += 4;
	SetValueOffset((char*)pCommand, offset, IntEndWord);

	return pPacket;
}
	
Packet* CreateTriggerIOSensorPacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int sensorID)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= 0x47424B50;
	pPacket->PacketID 	= 20;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE + 4;
				
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE;  
	
	pCommand = (Command*)pPacket->pData;			
	pCommand->ActionID 	= PCCmdActionTriggerIOSensor;
	pCommand->LineID		=	lineID;
	pCommand->ObjectID	=	objectID;
	pCommand->ModuleID	=	moduleID;
	pCommand->Encoder		=	encoder;
	pCommand->DataSize	=	4;
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, sensorID);offset += 4;
	SetValueOffset((char*)pCommand, offset, IntEndWord);

	return pPacket;
}

Packet* CreateGetMachineDataPacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, short* pMachineData)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
//	int offset = 0;
	char* pDst;
	
	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 10;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE + 32;					
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE;

	pCommand 						= (Command*)pPacket->pData;				
	pCommand->ActionID	=	PCCmdActionGetMachineData;
	pCommand->LineID		=	lineID;
	pCommand->ObjectID	=	objectID;
	pCommand->ModuleID	=	moduleID;
	pCommand->Encoder		=	encoder;
	pCommand->DataSize	=	32;
	
	pDst = (char*)pCommand + COMMAND_HEADER_SIZE;
	mymemcpy(pDst, pMachineData, 32);
	pDst += 32;
	
	SetValueOffset((char*)pDst, 0, IntEndWord);
					
	return pPacket;
}

//PC ---> PLC start/stop
//mode: [ObjectID]
//	0: 	TrackingNone 			stop diagnostics mode
//	1: 	TrackingProduction 		normal mode
//	2:	TrackingOneObject		set only one object as diagnostics object
//	3:	TrackingMultiObject		set another object as diagnostics object after the last one finishes
//	4:	TrackingLearning		learning mode, only one object should be in the inspection line
Packet* CreateSetTrackingModePacket(Packet* pPacket, int lineID, int mode)
{
//	Packet* pPacket 		= (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= IntBeginWord;
	pPacket->PacketID 	= 0;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE;
		
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE;
  
	pCommand = (Command*)pPacket->pData;
	pCommand->LineID 		= lineID;
	pCommand->ActionID	= PCCmdActionSetTrackingMode;
	pCommand->ObjectID	=	mode;
	pCommand->DataSize	=	0;
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);
	
	return pPacket;
}

Packet* CreateObjectPositionPacket(Packet* pPacket, int lineID, int* moduleIDList, int* modulePositionList, int num)
{
//	Packet* pPacket 		= (Packet*)mymalloc(SRAMEX, 256);
	Command* pCommand;
	//int offset = 0;		
	int offset, id, pos,i;
	
	pPacket->BeginWord 	= 0x47424B50;
	pPacket->PacketID 	= 0;
	pPacket->PacketType = 1;
					
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE; 
	
	pCommand = (Command*)pPacket->pData;
	pCommand->ActionID	= PCCmdActionObjectPosition;
	pCommand->LineID		=	lineID;
	
	offset = COMMAND_HEADER_SIZE;
	for(i = 0;i < num; i++)
	{
		id = moduleIDList[i];
		moduleIDList[i] = 0;
		pos = modulePositionList[i];
		modulePositionList[i] = 0;
		
		SetValueOffset((char*)pCommand, offset, id);offset += 4;
		SetValueOffset((char*)pCommand, offset, pos);offset += 4;
	}
	
	pCommand->DataSize 	= offset - COMMAND_HEADER_SIZE;
	pPacket->DataSize 	= offset ;	
	SetValueOffset((char*)pCommand, offset, IntEndWord);
	
	return pPacket;
}

Packet* CreateModuleInfoPacket(Packet* pPacket, int ModuleID, int ModuleSpeed, int numCreatedObjects, int numCreatedNormalObjects, int numCreatedInternalObjects, int numTakeoverObjects, int numLostObjects)
{
//	Packet* pPacket = (Packet*)mymalloc(SRAMEX, 128);
	Command* pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= 0x47424B50;
	pPacket->PacketID 	= 0;
	pPacket->PacketType = 1;
					
	
	pPacket->pData = (char*)pPacket + PACKET_HEADER_SIZE;
 
	pCommand = (Command*)pPacket->pData;
	
	pCommand->ActionID	=	PCCmdActionRequestModuleInfo;
	pCommand->ModuleID	=	ModuleID;
	pCommand->DataSize	=	24;	
	
	offset = COMMAND_HEADER_SIZE;

	SetValueOffset((char*)pCommand, offset, ModuleSpeed);offset += 4;
	SetValueOffset((char*)pCommand, offset, numCreatedObjects);offset += 4;
	SetValueOffset((char*)pCommand, offset, numCreatedNormalObjects);offset += 4;
	SetValueOffset((char*)pCommand, offset, numCreatedInternalObjects);offset += 4;
	SetValueOffset((char*)pCommand, offset, numTakeoverObjects);offset += 4;
	SetValueOffset((char*)pCommand, offset, numLostObjects);offset += 4;
	SetValueOffset((char*)pCommand, offset, IntEndWord);
	
	pPacket->DataSize = offset;			//如果时Packet得全部大小应该时offset + 4(报尾大小)

	return pPacket;
}

/************************************************************************
Packet* CreateSendPLCInfoPacket()
{
	int offset;
	TrackingManager * pTrackingManager = (TrackingManager*)TrackingManager::GetInstance();
	MemoryManager* pMemoryManager = (MemoryManager*)MemoryManager::GetInstance();
	CommManager * pCommManager = (CommManager*)CommManager::GetInstance();
	
	Packet* pPacket = (Packet*)pMemoryManager->Alloc(256);
	pPacket->BeginWord = 0x47424B50;
	pPacket->PacketID = 0;
	pPacket->PacketType = 1;
					
	pPacket->pData = (char*)pPacket + Packet::HEADER_SIZE;
	Command* pCommand;  
	pCommand = (Command*)pPacket->pData;
	pCommand->SetActionID(ActionRequestPLCInfo);

	offset = Command::HEADER_SIZE;
	offset = pTrackingManager->FillDiagnosticsData(offset, (char*)pCommand);
	offset = pMemoryManager->FillDiagnosticsData(offset, (char*)pCommand);
	offset = pCommManager->FillDiagnosticsData(offset, (char*)pCommand);
	
	pCommand->SetValue(offset, (int)0x44454B50);
	pCommand->SetDataSize(offset - Command::HEADER_SIZE);
	pPacket->DataBytes = offset;	

	return (char*)pPacket;
}
***************************************************************************/


Packet* CreateTrackingDummyPacket(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder)
{
//	Packet* 	pPacket 	= (Packet*)mymalloc(SRAMEX, 128);
	Command* 	pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= 0x47424B50;
	pPacket->PacketID 	= 0;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE;
			
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE;

	pCommand 						= (Command*)pPacket->pData;
	pCommand->LineID 		= lineID;
	pCommand->ActionID	=	PCCmdActionTrackingDummy;
	pCommand->ObjectID	=	objectID;
	pCommand->Encoder		=	encoder;
	pCommand->DataSize	=	0;
	
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);

	return pPacket;
}

char* CreateHeartBeatTCP(char* pData, short count)
{
//	char* pData = (char*)mymalloc(SRAMEX, 16);
	
	pData[0] = 0;
	pData[1] = 0;
	pData[2] = 0;
	pData[3] = 0;
	pData[4] = 0;
	pData[5] = 0;
	pData[6] = 0;
	pData[7] = 0;
	
	pData[8] = count;
	pData[9] = count>>8;
	
	return pData;
}

char* CreateSendAngleTCP(char* pData, short startIndex, short pos, short angle)
{
//	char* pData = (char*)mymalloc(SRAMEX, 16);
	
	pData[0] = 0;
	pData[1] = 0;
	pData[2] = 0;
	pData[3] = 0;
	pData[4] = 0;
	pData[5] = 0;
	pData[6] = 0;
	pData[7] = 0;
	
	pData[startIndex++] = pos;
	pData[startIndex++] = pos>>8;
	pData[startIndex++] = angle;
	pData[startIndex] 	= angle>>8;
	return pData;
}

Packet* CreateXRayErrorPacket(Packet* pPacket, int lineID, int ErrorFlag)
{
//	Packet* 	pPacket 	= (Packet*)mymalloc(SRAMEX, 128);
	Command* 	pCommand;
	int offset = 0;
	
	pPacket->BeginWord 	= 0x47424B50;
	pPacket->PacketID 	= 0;
	pPacket->PacketType = 1;
	pPacket->DataSize 	= COMMAND_HEADER_SIZE;
			
	pPacket->pData 			= (char*)pPacket + PACKET_HEADER_SIZE;
 
	pCommand 						= (Command*)pPacket->pData;
	pCommand->LineID		=	lineID;
	pCommand->ActionID	=	PCCmdActionXRayErrorPacket;
	pCommand->ObjectID	=	ErrorFlag;
	pCommand->DataSize	= 0;
	
	offset = COMMAND_HEADER_SIZE;
	SetValueOffset((char*)pCommand, offset, IntEndWord);

	return pPacket;
}


