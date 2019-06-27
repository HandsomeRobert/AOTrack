#ifndef __TASKMANAGE_H
#define __TASKMANAGE_H
/**************************************************************************************************
***********本文件用于统一管理各线程的优先级和堆栈大小，句柄暂时无法管理*******************************
**************************数字越大，优先级越高******************************************************
***************************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "stdbool.h"

typedef unsigned char byte;

//#ifndef TCPIP_THREAD_PRIO
//#define TCPIP_THREAD_PRIO		( configMAX_PRIORITIES - 1 )	 //定义内核任务的优先级为最高
//系统启动任务
#define   START_TASK_PRIO      					configMAX_PRIORITIES - 1	//configMAX_PRIORITIES = 10
#define   START_STK_SIZE       						256
extern TaskHandle_t StartTask_Handler;

//系统加载参数任务
#define   PARAMETERSLOAD_TASK_PRIO      configMAX_PRIORITIES - 2	//configMAX_PRIORITIES = 10
#define   PARAMETERSLOAD_STK_SIZE       	512
extern TaskHandle_t StartTask_Handler;

//动作触发任务
#define   ActionExecute_TASK_PRIO      	configMAX_PRIORITIES - 2	//configMAX_PRIORITIES = 10
#define   ActionExecute_STK_SIZE       		256
extern TaskHandle_t ActionExecuteTask_Handler;

//数据接收与处理任务，多SOCKET
#define      DataTransferManage_TASK_PRIO   7
#define      DataTransferManage_STK_SIZE  512
extern TaskHandle_t DataTransferManageTask_Handler;

//接收数据处理命令
#define      DataProcess_TASK_PRIO      		6
#define      DataProcess_STK_SIZE       	512
extern TaskHandle_t DataProcessTask_Handler;

//Tracking 任务优先级与堆栈大小
#define TRACKING_TASK_PRIO									5		
#define TRACKING_STK_SIZE									512
extern TaskHandle_t TrackingThread_Handler;

//TCPServer服务端任务优先级与堆栈大小
#define TCPSERVER_TASK_PRIO									4
#define TCPSERVER_STK_SIZE								256
extern TaskHandle_t TCPSERVERTask_Handler;
//lwip DHCP任务
//设置任务优先级
#define LWIP_DHCP_TASK_PRIO       					2
#define LWIP_DHCP_STK_SIZE  		    			256

//状态显示任务，1s LED闪一次
#define    StatusIndicate_TASK_PRIO        	1
#define    StatusIndicate_STK_SIZE         64
extern TaskHandle_t StatusIndicateTask_Handler;

//编码器模拟任务Timer
#define    SimulationTimer_TASK_PRIO        1
#define    SimulationTimer_STK_SIZE        64
extern TaskHandle_t SimulationTimerTask_Handler;

#endif
