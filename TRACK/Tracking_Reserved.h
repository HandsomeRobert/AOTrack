/**************************************************************************************************
**************************¸ú×ÙÏß³Ì************************************************************
***************************************************************************************************/
#ifndef __TRACKING_H
#define __TRACKING_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "bsp_encoder.h"						//	±àÂëÆ÷	
#include "LightSignal.h"						//¹âµç
#include "led.h"										//ÓÃLEDÀ´Ä£Äâ´¥·¢
#include "DataTransferManage.h"
#include "TcpPacketServer.h"
#include "ParametersLoad.h"					//¼ÓÔØÅäÖÃ²ÎÊı

struct ObjectTracking 
{
	byte 	ClientID;
	short ObjectID;
	bool 	CreateObjectFlag;						//default = false
	__IO 	int64_t BornEncoderNum;					//64BitÀ´´æ´¢¿ÉÒç³öµÄ±àÂëÆ÷Öµ
	byte 	CameraID;
	char 	*pData;
	bool  ProcessedResult;

	/***´«µİ¶¯×÷ËùÊ¹ÓÃµÄ±êÖ¾Î»***/
	bool	objectDelieverdFlag;				//ÓÃÓÚ±ê¼Ç´Ë¶ÔÏóÊÇ·ñ±»´«µİ³öÈ¥ÁË
	byte	objectModulePosition;							//¼ÇÂ¼¶ÔÏóËùÔÚµÄ¸ú×Ù¶Î
	//byte	delieverDestination;				//¼ÇÂ¼¶ÔÏóËùÒª´«µİµ½µÄ¸ú×Ù¶
	__IO 	int64_t deliverEncoderNum;	//´«µİ¶¯×÷µÄ±àÂëÆ÷Öµ
	
	bool 	objectAliveFlag;																							//ÓÃÓÚÅĞ¶Ï¶ÔÏóÊÇ·ñÍê³ÉÁË¸ú×Ù¹ı³Ì£¬ÊÇ·ñÈÔÉú´æ
	//bool	modulePosition[maxTrackingModule];														//¶¨ÒåÒ»¸ö¸ú×Ù¶ÔÏó´¦ÓÚ¸ú×Ù¶ÎµÄÎ»ÖÃ£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!modulePosition ºÍobjectPostitionÓĞÖØ¸´¶¨Òå£¬¼ÇµÃĞŞ¸Ä
	bool 	triggerPosition[maxTrackingModule][maxActionSingleModule];		//¶¨ÒåÒ»¸öÓÃÓÚ¼ÇÂ¼¸ú×Ù¶ÔÏóÔÚ¶ÔÓ¦µÄÎ»ÖÃÊÇ·ñ±»´¥·¢µÄ¶şÎ¬Êı¾İ
	
	__IO 	int64_t ModuleReferenceEncoderNum[maxTrackingModule];					//¸ø¶ÔÏóµÄÃ¿¸ö¸ú×Ù¶Î¶¨ÒåÒ»¸ö±àÂëÆ÷²Î¿¼Öµ

};

extern __IO int64_t EncoderNumber;

#define maxTrackingObjects  64			//×î´óÎ¬»¤¶àÉÙ¸ö¸ú×Ù¶ÔÏó
//#define defaultPositionRange 3		//Ä¬ÈÏÔÊĞíµ½´ï¶¯×÷Ö´ĞĞ´¦µÄ±àÂëÆ÷ÊıÖµÆ«²î
#define Message_TrrigerCamera 1			//¶¯×÷¶ÔÓ¦µÄÏûÏ¢Öµ
#define Message_TrrigerSensor 2
#define Message_TrrigerOutput 3
#define Message_TrrigerPush 	4


uint8_t TrackingTask_init(void);

#endif

