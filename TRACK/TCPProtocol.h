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

#include "stdbool.h"

typedef struct
{
	int BeginWord;
	int PacketID;
	int PacketType;
	int DataSize;
	
	char* pData;	
	
}Packet;

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


#define COMMAND_HEADER_SIZE 							24
#define PACKET_HEADER_SIZE 								16
#define IntBeginWord											0x47424B50
#define IntEndWord												0x44454B50

//System
#define PCCmdActionHeartBeat 							1
#define PCCmdActionWarmRestart  					2
#define PCCmdActionColdRestart 						3
#define PCCmdActionReconfiguration  			4

#define PCCmdActionStartIOLive  					10
#define PCCmdActionGetConnectedClientID 	11
#define PCCmdActionErrorMessage  					20
#define PCCmdActionErrorAcknowledge  			21
#define PCCmdActionXRayErrorPacket				22

//Tracking
#define PCCmdActionObjectRunIn  					100
#define PCCmdActionObjectRunOut  					101
#define PCCmdActionObjectDelete  					102
#define PCCmdActionTriggerCamera  				103
#define PCCmdActionTriggerIOSensor  			104
#define PCCmdActionGetMachineData  				105
#define PCCmdActionSetPushResult  				106
#define PCCmdActionSetUserResult  				107
#define PCCmdActionStartTracking  				120
#define PCCmdActionObjectFallDown  				130

//Automation
#define PCCmdActionSetPLCVariable  				200
#define PCCmdActionStartControl  					201

//Diagnostics
#define PCCmdActionSetTrackingMode  			301
#define PCCmdActionObjectPosition  				302
#define PCCmdActionObjectWidth  					303
#define PCCmdActionRequestModuleInfo 			304
#define PCCmdActionRequestPLCInfo  				305
#define PCCmdActionTrackingDummy  				306

/*
**创建各种包的函数
*******/
Packet* CreateHeartBeatPacket				(Packet* pPacket);
Packet* CreateReconfigurationPacket	(Packet* pPacket, int fileID, int result);
Packet* CreateStartTrackingPacket		(Packet* pPacket, int lineID, int objectID);
Packet* CreateStartControlPacket		(Packet* pPacket, int lineID, int objectID);
Packet* CreateObjectFalldownPacket	(Packet* pPacket, int lineID, int moduleID);
Packet* CreateStartIOLivePacket			(Packet* pPacket, bool start);
Packet* CreateClientIDPacket				(Packet* pPacket, int* ConnectClientID,int ConnectIDNum);
Packet* CreateErrorMessagePacket		(Packet* pPacket, int messageID);
Packet* CreateErrorAcknowledgePacket(Packet* pPacket, int messageID);
Packet* CreateObjectRunInPacket			(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int flag, int lastTriggerPreviousEncoder,int lastReceivePreviousEncoder);
Packet* CreateObjectRunOutPacket		(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int destModuleID);
Packet* CreateObjectDeletePacket		(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder);
Packet* CreateTriggerCameraPacket		(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int cameraID, int imageIndex);
Packet* CreateTriggerIOSensorPacket	(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, int sensorID);
Packet* CreateGetMachineDataPacket	(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder, short* pMachineData);
Packet* CreateSetTrackingModePacket	(Packet* pPacket, int lineID, int mode);
Packet* CreateObjectPositionPacket	(Packet* pPacket, int lineID, int* moduleIDList, int* modulePositionList, int num);
Packet* CreateModuleInfoPacket			(Packet* pPacket, int ModuleID, int ModuleSpeed, int numCreatedObjects, int numCreatedNormalObjects, int numCreatedInternalObjects, int numTakeoverObjects, int numLostObjects);
Packet* CreateTrackingDummyPacket		(Packet* pPacket, int lineID, int objectID, int moduleID, int encoder);
Packet* CreateXRayErrorPacket				(Packet* pPacket, int lineID, int ErrorFlag);

/*
**创建其它数据结构的函数
*******/
char* 	CreateHeartBeatTCP					(char* pData, short count);
char* 	CreateSendAngleTCP					(char* pData, short startIndex, short pos, short angle);


#endif

