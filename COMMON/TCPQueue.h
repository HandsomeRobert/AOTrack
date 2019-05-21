#ifndef __TCPQUEUE_H
#define __TCPQUEUE_H
/**************************************************************************************************
***********本文件用于TCP缓冲队列的构建*****************************************************
***************************************************************************************************/
#include <stdio.h>
#include <malloc.h>

#define MaxPointNumInQueue 10

typedef uint8_t byte;

typedef enum
{
    OK=0, 			//正确
    ERROR_T=1,  //错误
    TRUE=2,  		//真
    FALSE=3   	//假
}status;

typedef uint8_t ElemType;   //byte
//#define MAX_SIZE 40         //10 int = 40byte


typedef struct
{
    int* pBase[MaxPointNumInQueue];     //每个队列最多只能缓存10个指针,每个指针(地址)在计算机中占4个字节
    ElemType front;      								//队头索引
    ElemType rear;       								//队尾索引
//    int PerElemMaxSize;    							//每个队列的每个元素的最大容量
}queue;

//初始化队列
status initQueue(queue **PQueue);
//销毁队列
void destroyQueue(queue *PQueue);
//清空队列
//void clearQueue(queue *PQueue);
//判断队列是否为空
status isEmpityQueue(queue *PQueue);
//判断队列是否满
status isFullQueue(queue *PQueue);
//获得队列长度
int getQueueLen(queue *PQueue);
//新元素入队
status enQueue(queue *PQueue,ElemType* Data, u32 dataLength);
//新元素出队
status deQueue(queue *PQueue,int **pElement);
//
//void queueTraverse(queue *PQueue);

#endif // TCPQUEUEC_H


