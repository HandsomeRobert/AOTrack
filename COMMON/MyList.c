/**************************************************************************************************
*************************本文件用于List的实现******************************************************
***************************************************************************************************/

#include <stdio.h>
#include "MyList.h"

/*初始化列表*/
void InitList(ObjectList *L)			//带头结点的列表
{ 
	(*L) = mymalloc(SRAMEX, (sizeof(struct LNode)));//分配头结点
	printf("Malloc List MemoryAdress is %p \n", L);
	if (*L == NULL) //内存分配失败
	{
		printf("List Malloc Memory Failed. \n");
		exit(0);
		
	}
	
	(*L)->Length = 0;
	(*L)->next = *L;  //头结点指向本身
	printf("Malloc List MemoryAdress *L -> next  is %p \n", (*L) -> next );
}

///*清空列表*/
//void ClearList(ObjectList *L) 
//{
//	ObjectList p, q;
//	*L = (*L)->next;  //先令尾指针指向头节点，不然释放最后一个尾指针时，无法指向头节点
//	p = (*L)->next;  //p指向第一个结点

//	while (p != *L) //p未到表头
//	{
//			 q = p->next;
//			 myfree(SRAMEX, p);
//			 p = q;
//	}
//	(*L) -> next = *L; //头节点指向本身
//}

///*销毁列表*/
//void DestroyList(ObjectList *L)
//{
//	ClearList(L);
//	myfree(SRAMEX, *L); //释放头节点
//	(*L) = NULL;
//}

///*求列表表长*/
//int ListLength(ObjectList L)
//{
//	ObjectList p = L->next->next;  //p指向第一个节点(因为L->next指向表头本身)
//	int j = 0;

//	while (p != L->next) //p未到表头
//	{
//		++j;
//		p = p->next;
//	}
//	return j;
//}

/*插入列表*/
void ListInsert(ObjectList *L, int i, ObjectTrack Object) //在表尾插入改变尾指针
{
	ObjectList p = (*L);  //p指向表头->next
	ObjectList q, s;
	int j = 0;

	if (i<1 || i>((*L)->Length)+1) //插入位置不合理
		printf("Insert Elem to List Failed==Position Wrong \n");
	 
	while (j < i-1)  //位置合理与否，找到第i-1个结点（第一个结点除外）
	{
		++j;
		p = p->next;			//p指向第i-1个结点
	}    
	q  = p->next; 	//q指向第i个结点
	s = (ObjectList)mymalloc(SRAMEX, sizeof(struct LNode));
	s->Object = Object;				
	
	//！！！！！！！！！！！这里赋值可能出错，导致所有的对象的Object指针都指向同一个地方
	if (q == *L)		//如果在表尾插入，则需将表尾元素指向表头
		s->next = *L;
	else
		s->next = q;
	
	p->next = s;
	(*L)->Length++;
}

/*往列表尾巴加入元素*/
void ListAppend(ObjectList *L, ObjectTrack Object)
{
	ListInsert(L, ((*L)->Length)+1, Object);						
	printf("ListAdd Position[%p]==>Length[ %d]\n", *L, (*L)->Length);
}	
/*根据给出的地址从列表中删除相应的元素*/
void ListDeletePointItem(ObjectList *L, ObjectList L_Delete)
{
	ObjectList p = (*L)->next;					//指向列表中的第一个元素
	ObjectList L_Delete_Before = (*L);	//指向表头，用于第一个元素的删除
	bool isFindElem = true;
	
	while(p != L_Delete)				//从列表中寻找要删除的元素
	{
		L_Delete_Before = p;
		p = p->next;
		
		if(p == *L)//循环到头结点，仍未找到所需删除的指针则删除失败
		{
			printf("Elemet does not exist, Delete failed.	\n");
			isFindElem = false;	
			break;
		}
	}
	
	if(p == (*L))//如果删除的是表头结点
	{
		printf("Error: List Head Cannot be Deleted!. \n");
		isFindElem = false;
	}
	
	if(isFindElem)
	{
		L_Delete_Before->next = p->next;//p为找到的结点，L_Delete_Before为找到的结点的前一结点
		
		myfree(SRAMEX, p);
		(*L)->Length--;
		printf("ListDelete Position[%p] Length[ %d]\n", *L, (*L)->Length);
	}
}

///*删除列表中的元素*/
//void ListDelete(ObjectList *L, int i)
//{
//    ObjectList p = (*L)->next;//指向表头
//    ObjectList q;
//    int j = 0;
//    if (i<1 || i>(*L)->Length)
//        exit(0);
//    
//    while (j<i-1)  //找到第i-1个结点
//    {
//         ++j;
//         p = p->next;
//    }
//    q = p->next;  //q指向第i个结点

//    p->next = q->next;
//    if (q == *L)   //删除的是表尾元素，表尾指针发生改变
//        *L = p;
//    myfree(SRAMEX, q);
//		(*L)->Length--;
//}



///*遍历列表*/
//void TravelList(ObjectList L)
//{
//   ObjectList p = L->next->next;  //p指向第一个结点(因为L->next指向表头本身了)
//   int j = 0;

//    while (p != L->next)  //p未到表头
//   {
//        ++j;
//        //printf("The Number[%d] Value is :%d\n", j, p -> Object);
//        p = p->next;
//    }
//}
