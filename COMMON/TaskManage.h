#ifndef __TASKMANAGE_H
#define __TASKMANAGE_H
/**************************************************************************************************
***********���ļ�����ͳһ������̵߳����ȼ��Ͷ�ջ��С�������ʱ�޷�����*******************************
**************************����Խ�����ȼ�Խ��******************************************************
***************************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "stdbool.h"

typedef unsigned char byte;

//#ifndef TCPIP_THREAD_PRIO
//#define TCPIP_THREAD_PRIO		( configMAX_PRIORITIES - 1 )	 //�����ں���������ȼ�Ϊ���
//ϵͳ��������
#define   START_TASK_PRIO      					configMAX_PRIORITIES - 1	//configMAX_PRIORITIES = 10
#define   START_STK_SIZE       						256
extern TaskHandle_t StartTask_Handler;

//ϵͳ���ز�������
#define   PARAMETERSLOAD_TASK_PRIO      configMAX_PRIORITIES - 2	//configMAX_PRIORITIES = 10
#define   PARAMETERSLOAD_STK_SIZE       	512
extern TaskHandle_t StartTask_Handler;

//������������
#define   ActionExecute_TASK_PRIO      	configMAX_PRIORITIES - 2	//configMAX_PRIORITIES = 10
#define   ActionExecute_STK_SIZE       		256
extern TaskHandle_t ActionExecuteTask_Handler;

//���ݽ����봦�����񣬶�SOCKET
#define      DataTransferManage_TASK_PRIO   7
#define      DataTransferManage_STK_SIZE  512
extern TaskHandle_t DataTransferManageTask_Handler;

//�������ݴ�������
#define      DataProcess_TASK_PRIO      		6
#define      DataProcess_STK_SIZE       	512
extern TaskHandle_t DataProcessTask_Handler;

//Tracking �������ȼ����ջ��С
#define TRACKING_TASK_PRIO									5		
#define TRACKING_STK_SIZE									512
extern TaskHandle_t TrackingThread_Handler;

//TCPServer������������ȼ����ջ��С
#define TCPSERVER_TASK_PRIO									4
#define TCPSERVER_STK_SIZE								256
extern TaskHandle_t TCPSERVERTask_Handler;
//lwip DHCP����
//�����������ȼ�
#define LWIP_DHCP_TASK_PRIO       					2
#define LWIP_DHCP_STK_SIZE  		    			256

//״̬��ʾ����1s LED��һ��
#define    StatusIndicate_TASK_PRIO        	1
#define    StatusIndicate_STK_SIZE         64
extern TaskHandle_t StatusIndicateTask_Handler;

//������ģ������Timer
#define    SimulationTimer_TASK_PRIO        1
#define    SimulationTimer_STK_SIZE        64
extern TaskHandle_t SimulationTimerTask_Handler;

#endif
