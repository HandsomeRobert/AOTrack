/**************************************************************************************************
**************************���ݴ����߳�***********************************************************
***************************************************************************************************/
#ifndef __DATAPROCESS_H
#define __DATAPROCESS_H

#include "TaskManage.h"
#include "SemaphoreManage.h"
#include "FreeRTOS.h"
#include "DataTransferManage.h"
#include "TcpPacketServer.h"					//���ClientNum
//#include "TCPProtocol.h"							//��ȡTCP���ݰ�����

uint8_t DataProcessTask_init(void);

#endif

