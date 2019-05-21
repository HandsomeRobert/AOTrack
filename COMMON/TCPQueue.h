#ifndef __TCPQUEUE_H
#define __TCPQUEUE_H
/**************************************************************************************************
***********���ļ�����TCP������еĹ���*****************************************************
***************************************************************************************************/
#include <stdio.h>
#include <malloc.h>

#define MaxPointNumInQueue 10

typedef uint8_t byte;

typedef enum
{
    OK=0, 			//��ȷ
    ERROR_T=1,  //����
    TRUE=2,  		//��
    FALSE=3   	//��
}status;

typedef uint8_t ElemType;   //byte
//#define MAX_SIZE 40         //10 int = 40byte


typedef struct
{
    int* pBase[MaxPointNumInQueue];     //ÿ���������ֻ�ܻ���10��ָ��,ÿ��ָ��(��ַ)�ڼ������ռ4���ֽ�
    ElemType front;      								//��ͷ����
    ElemType rear;       								//��β����
//    int PerElemMaxSize;    							//ÿ�����е�ÿ��Ԫ�ص��������
}queue;

//��ʼ������
status initQueue(queue **PQueue);
//���ٶ���
void destroyQueue(queue *PQueue);
//��ն���
//void clearQueue(queue *PQueue);
//�ж϶����Ƿ�Ϊ��
status isEmpityQueue(queue *PQueue);
//�ж϶����Ƿ���
status isFullQueue(queue *PQueue);
//��ö��г���
int getQueueLen(queue *PQueue);
//��Ԫ�����
status enQueue(queue *PQueue,ElemType* Data, u32 dataLength);
//��Ԫ�س���
status deQueue(queue *PQueue,int **pElement);
//
//void queueTraverse(queue *PQueue);

#endif // TCPQUEUEC_H


