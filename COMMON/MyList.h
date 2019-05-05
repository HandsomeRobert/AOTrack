#ifndef __MYLIST_H
#define __MYLIST_H
/**************************************************************************************************
***********本文件用于List的实现*****************************************************
***************************************************************************************************/

#include "stdbool.h"

#include <stdio.h>
#include <stdlib.h>
#include "malloc.h"


#define LIST_INIT_MAX 	64      //最长的表长为64
#define LIST_INCREMENT 	2     //
typedef uint8_t byte ;
				
		
#define TypeActionTriggerCamera					1
#define TypeActionObjectTakeOver				2
#define TypeActionSetOutput							3
#define TypeActionTriggerSensor					4
#define TypeActionPushOut								5	
#define TypeActionRequestMachineData		6
							

#define	NoOutput												0
#define	TypeOutputDigital								1
#define	TypeOutputResult								2


typedef struct
{
	__IO 	uint32_t ObjectID;
//	__IO 	int64_t RefEncoderNum;			//64Bit来存储可溢出的编码器值
	__IO 	int64_t TargetValue;				//目标值
	byte	ActionType;									//动作类型
	byte  ActionNumber;								//此动作是跟踪段上的第几个动作
	byte	OutputType;									//输出类型
	byte	OutputChannel;							//输出通道
}ObjectTrack;

struct LNode
{
	ObjectTrack Object;               	//动态分配结构，此处千万不能用指针，否则所有对象都会使用同一个指针....指针使用有问题
	struct LNode* next;									//定义尾指针
	int Length;
};
typedef struct LNode* ObjectList;

/*线性表拥有的函数*/
void InitList(ObjectList *L);          
//void DestroyList(ObjectList *L);      
//void ClearList(ObjectList *L);
//void ListEmpty(ObjectList L);
//int ListLength(ObjectList L);
//int GetElem(ObjectList L, int i, int *e); 
//void LocateList(ObjectList L, int e);     
//int PriorElem(ObjectList L, int cur_e, int *pri_e);  
//int NextElem(ObjectList L, int cur_e, int *Nex_e); 
void ListInsert(ObjectList *L, int i, ObjectTrack Object); 
void ListAppend(ObjectList *L, ObjectTrack Object);
void ListDeletePointItem(ObjectList *L, ObjectList L_Delete);
//void ListDelete(ObjectList *L, int i); 
//void TravelList(ObjectList L);               


#endif

