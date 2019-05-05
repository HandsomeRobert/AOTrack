#ifndef __MYLIST_H
#define __MYLIST_H
/**************************************************************************************************
***********���ļ�����List��ʵ��*****************************************************
***************************************************************************************************/

#include "stdbool.h"

#include <stdio.h>
#include <stdlib.h>
#include "malloc.h"


#define LIST_INIT_MAX 	64      //��ı�Ϊ64
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
//	__IO 	int64_t RefEncoderNum;			//64Bit���洢������ı�����ֵ
	__IO 	int64_t TargetValue;				//Ŀ��ֵ
	byte	ActionType;									//��������
	byte  ActionNumber;								//�˶����Ǹ��ٶ��ϵĵڼ�������
	byte	OutputType;									//�������
	byte	OutputChannel;							//���ͨ��
}ObjectTrack;

struct LNode
{
	ObjectTrack Object;               	//��̬����ṹ���˴�ǧ������ָ�룬�������ж��󶼻�ʹ��ͬһ��ָ��....ָ��ʹ��������
	struct LNode* next;									//����βָ��
	int Length;
};
typedef struct LNode* ObjectList;

/*���Ա�ӵ�еĺ���*/
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

