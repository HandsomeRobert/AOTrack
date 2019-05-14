/**************************************************************************************************
**************************数据处理线程***********************************************************
***************************************************************************************************/
#ifndef __DATAPROCESS_H
#define __DATAPROCESS_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "DataTransferManage.h"
#include "TcpPacketServer.h"					//获得ClientNum
//#include "TCPProtocol.h"							//获取TCP数据包命令

uint8_t DataProcessTask_init(void);

#endif

