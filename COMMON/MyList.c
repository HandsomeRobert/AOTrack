/**************************************************************************************************
*************************���ļ�����List��ʵ��******************************************************
***************************************************************************************************/

#include <stdio.h>
#include "MyList.h"

/*��ʼ���б�*/
void InitList(ObjectList *L)			//��ͷ�����б�
{ 
	(*L) = mymalloc(SRAMEX, (sizeof(struct LNode)));//����ͷ���
	printf("Malloc List MemoryAdress is %p \n", L);
	if (*L == NULL) //�ڴ����ʧ��
	{
		printf("List Malloc Memory Failed. \n");
		exit(0);
		
	}
	
	(*L)->Length = 0;
	(*L)->next = *L;  //ͷ���ָ����
	printf("Malloc List MemoryAdress *L -> next  is %p \n", (*L) -> next );
}

///*����б�*/
//void ClearList(ObjectList *L) 
//{
//	ObjectList p, q;
//	*L = (*L)->next;  //����βָ��ָ��ͷ�ڵ㣬��Ȼ�ͷ����һ��βָ��ʱ���޷�ָ��ͷ�ڵ�
//	p = (*L)->next;  //pָ���һ�����

//	while (p != *L) //pδ����ͷ
//	{
//			 q = p->next;
//			 myfree(SRAMEX, p);
//			 p = q;
//	}
//	(*L) -> next = *L; //ͷ�ڵ�ָ����
//}

///*�����б�*/
//void DestroyList(ObjectList *L)
//{
//	ClearList(L);
//	myfree(SRAMEX, *L); //�ͷ�ͷ�ڵ�
//	(*L) = NULL;
//}

///*���б��*/
//int ListLength(ObjectList L)
//{
//	ObjectList p = L->next->next;  //pָ���һ���ڵ�(��ΪL->nextָ���ͷ����)
//	int j = 0;

//	while (p != L->next) //pδ����ͷ
//	{
//		++j;
//		p = p->next;
//	}
//	return j;
//}

/*�����б�*/
void ListInsert(ObjectList *L, int i, ObjectTrack Object) //�ڱ�β����ı�βָ��
{
	ObjectList p = (*L);  //pָ���ͷ->next
	ObjectList q, s;
	int j = 0;

	if (i<1 || i>((*L)->Length)+1) //����λ�ò�����
		printf("Insert Elem to List Failed==Position Wrong \n");
	 
	while (j < i-1)  //λ�ú�������ҵ���i-1����㣨��һ�������⣩
	{
		++j;
		p = p->next;			//pָ���i-1�����
	}    
	q  = p->next; 	//qָ���i�����
	s = (ObjectList)mymalloc(SRAMEX, sizeof(struct LNode));
	s->Object = Object;				
	
	//�������������������������︳ֵ���ܳ����������еĶ����Objectָ�붼ָ��ͬһ���ط�
	if (q == *L)		//����ڱ�β���룬���轫��βԪ��ָ���ͷ
		s->next = *L;
	else
		s->next = q;
	
	p->next = s;
	(*L)->Length++;
}

/*���б�β�ͼ���Ԫ��*/
void ListAppend(ObjectList *L, ObjectTrack Object)
{
	ListInsert(L, ((*L)->Length)+1, Object);						
	printf("ListAdd Position[%p]==>Length[ %d]\n", *L, (*L)->Length);
}	
/*���ݸ����ĵ�ַ���б���ɾ����Ӧ��Ԫ��*/
void ListDeletePointItem(ObjectList *L, ObjectList L_Delete)
{
	ObjectList p = (*L)->next;					//ָ���б��еĵ�һ��Ԫ��
	ObjectList L_Delete_Before = (*L);	//ָ���ͷ�����ڵ�һ��Ԫ�ص�ɾ��
	bool isFindElem = true;
	
	while(p != L_Delete)				//���б���Ѱ��Ҫɾ����Ԫ��
	{
		L_Delete_Before = p;
		p = p->next;
		
		if(p == *L)//ѭ����ͷ��㣬��δ�ҵ�����ɾ����ָ����ɾ��ʧ��
		{
			printf("Elemet does not exist, Delete failed.	\n");
			isFindElem = false;	
			break;
		}
	}
	
	if(p == (*L))//���ɾ�����Ǳ�ͷ���
	{
		printf("Error: List Head Cannot be Deleted!. \n");
		isFindElem = false;
	}
	
	if(isFindElem)
	{
		L_Delete_Before->next = p->next;//pΪ�ҵ��Ľ�㣬L_Delete_BeforeΪ�ҵ��Ľ���ǰһ���
		
		myfree(SRAMEX, p);
		(*L)->Length--;
		printf("ListDelete Position[%p] Length[ %d]\n", *L, (*L)->Length);
	}
}

///*ɾ���б��е�Ԫ��*/
//void ListDelete(ObjectList *L, int i)
//{
//    ObjectList p = (*L)->next;//ָ���ͷ
//    ObjectList q;
//    int j = 0;
//    if (i<1 || i>(*L)->Length)
//        exit(0);
//    
//    while (j<i-1)  //�ҵ���i-1�����
//    {
//         ++j;
//         p = p->next;
//    }
//    q = p->next;  //qָ���i�����

//    p->next = q->next;
//    if (q == *L)   //ɾ�����Ǳ�βԪ�أ���βָ�뷢���ı�
//        *L = p;
//    myfree(SRAMEX, q);
//		(*L)->Length--;
//}



///*�����б�*/
//void TravelList(ObjectList L)
//{
//   ObjectList p = L->next->next;  //pָ���һ�����(��ΪL->nextָ���ͷ������)
//   int j = 0;

//    while (p != L->next)  //pδ����ͷ
//   {
//        ++j;
//        //printf("The Number[%d] Value is :%d\n", j, p -> Object);
//        p = p->next;
//    }
//}
