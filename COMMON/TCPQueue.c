/**************************************************************************************************
*************************���ļ��������ݻ���Queue��ʵ��******************************************************
***************************************************************************************************/

#include <stdio.h>
#include "TCPQueue.h"
#include "malloc.h"

//��ʼ������
status initQueue(queue **PQueue, int queueCapacity)
{

	(*PQueue)->front = 0; 
	(*PQueue)->rear 	= 0; 
	(*PQueue)->PerElemMaxSize = queueCapacity;

	return OK;
}

//���ٶ���
void destroyQueue(queue *PQueue)
{
		byte i = PQueue->front;           

    while(i != PQueue->rear)     					//�ͷŶ���
    {
        myfree(SRAMEX, PQueue->pBase[i]);
				PQueue->pBase[i] = NULL;					//����ָ��NULL�������ΪҰָ��
        i = (i+1) % MaxPointNumInQueue;    
    }
		
		PQueue->front = 0; 
    PQueue->rear = 0;
		
    printf("Queue Is destroyed\n");
}

////��ն���
//void clearQueue(queue *PQueue)
//{
//    PQueue->front = 0; 
//    PQueue->rear = 0;
//}

//�ж϶����Ƿ�Ϊ��
status isEmpityQueue(queue *PQueue)
{
    if( PQueue->front == PQueue->rear )  //��ͷ==��β��Ϊ��
        return TRUE;

    return FALSE;
}

/****��ѭ�������У����������͡��ӿա���������������ͬ�ģ���front==rear��
***********��ѭ������ʹ�á���ʹ��һ��Ԫ�ؿռ䡷������***********
*******��������Ϊ��(PQueue->rear+1)%MAX_SIZE == PQueue->front
*********/

//�ж϶����Ƿ�Ϊ��
status isFullQueue(queue *PQueue)
{
    if( (PQueue->rear+1)%MaxPointNumInQueue == PQueue->front )  //����
        return TRUE;

    return FALSE;
}

//��ö��г���
int getQueueLen(queue *PQueue)
{
    //�������Ϊ���г���rear,front֮����ǵ�rear/front����ֵ����MaxPointNumInQueueʱ��Ҫȡ��
    return (PQueue->rear - PQueue->front)%MaxPointNumInQueue;
}

//�Ƚ��ȳ��ڶ�β����Ԫ�أ�DataΪҪ�����Ԫ�أ�dataLengthΪҪ����Ĵ�С
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

//Ԫ�س��ӣ��Ƚ��ȳ�����ͷλ��ɾ��
status deQueue(queue *PQueue,int **pElement)
{
    //
    if(isEmpityQueue(PQueue)==TRUE)
    {
        printf("Queue is empty, Out Queue Failed! \n");
        return FALSE;
    }

    (*pElement) = PQueue->pBase[PQueue->front];       //�Ƚ��ȳ�
		myfree(SRAMEX, PQueue->pBase[PQueue->front]);
    PQueue->front = (PQueue->front+1) % MaxPointNumInQueue; //��ͷ�ƶ�����һ��λ��

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


