/**************************************************************************************************
*************************本文件用于数据缓冲Queue的实现******************************************************
***************************************************************************************************/

#include <stdio.h>
#include "TCPQueue.h"
#include "malloc.h"

//初始化队列
status initQueue(queue **PQueue, int queueCapacity)
{

	(*PQueue)->front = 0; 
	(*PQueue)->rear 	= 0; 
	(*PQueue)->PerElemMaxSize = queueCapacity;

	return OK;
}

//销毁队列
void destroyQueue(queue *PQueue)
{
		byte i = PQueue->front;           

    while(i != PQueue->rear)     					//释放队列
    {
        myfree(SRAMEX, PQueue->pBase[i]);
				PQueue->pBase[i] = NULL;					//重新指向NULL，避免成为野指针
        i = (i+1) % MaxPointNumInQueue;    
    }
		
		PQueue->front = 0; 
    PQueue->rear = 0;
		
    printf("Queue Is destroyed\n");
}

////清空队列
//void clearQueue(queue *PQueue)
//{
//    PQueue->front = 0; 
//    PQueue->rear = 0;
//}

//判断队列是否为空
status isEmpityQueue(queue *PQueue)
{
    if( PQueue->front == PQueue->rear )  //队头==队尾则为空
        return TRUE;

    return FALSE;
}

/****在循环队列中，“队满”和“队空”的条件可能是相同的，即front==rear；
***********本循环队列使用《少使用一个元素空间》来处理***********
*******队满条件为：(PQueue->rear+1)%MAX_SIZE == PQueue->front
*********/

//判断队列是否为空
status isFullQueue(queue *PQueue)
{
    if( (PQueue->rear+1)%MaxPointNumInQueue == PQueue->front )  //队满
        return TRUE;

    return FALSE;
}

//获得队列长度
int getQueueLen(queue *PQueue)
{
    //正常情况为队列长度rear,front之差，但是当rear/front计数值大于MaxPointNumInQueue时，要取余
    return (PQueue->rear - PQueue->front)%MaxPointNumInQueue;
}

//先进先出在队尾插入元素，Data为要插入的元素，dataLength为要插入的大小
status enQueue(queue* PQueue,ElemType* Data , u32 dataLength)
{
		PQueue->pBase[PQueue->rear] = (int *)mymalloc(SRAMEX, dataLength);
	
		if(!PQueue->pBase[PQueue->rear])
    {
        printf("Array memory allocate failed! \n");
        return ERROR_T;
    }
	
    if(isFullQueue(PQueue)==TRUE)
    {
        printf("Queue is Full, Insert Element Failed! \n");
        return FALSE;
    }

    //
    //PQueue->pBase[PQueue->rear] = Data;
		mymemcpy(PQueue->pBase[PQueue->rear] , Data, dataLength);
    PQueue->rear = (PQueue->rear+1) % MaxPointNumInQueue; //

    return TRUE;
}

//元素出队，先进先出，队头位置删除
status deQueue(queue *PQueue,int **pElement)
{
    //
    if(isEmpityQueue(PQueue)==TRUE)
    {
        printf("Queue is empty, Out Queue Failed! \n");
        return FALSE;
    }

    (*pElement) = PQueue->pBase[PQueue->front];       //先进先出
		myfree(SRAMEX, PQueue->pBase[PQueue->front]);
    PQueue->front = (PQueue->front+1) % MaxPointNumInQueue; //队头移动到后一个位置

    return TRUE;
}

////
//void queueTraverse(queue *PQueue)
//{
//    int i = PQueue->front;           //
//    printf("Travel the queue :\n");
//    while(i != PQueue->rear)     //
//    {
//        printf("%d  ", PQueue->pBase[i]);
//        i = (i+1) % MaxPointNumInQueue;              //
//    }
//    printf("\n");
//}


