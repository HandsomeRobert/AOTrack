/**************************************************************************************************
*************************************�����߳�******************************************************
***************************************************************************************************/
#include "Tracking.h"
#include <math.h>
#include "TCPProtocol.h"
#include "timer.h"
#include "malloc.h"

extern uint32_t Timer;			//ʹ������������ķ���Timer

__IO	uint32_t GlobalObjectID;											//����һ��ȫ�ֶ���ID��__IO��ʾֱ�Ӵӵ�ַ��ȡֵ��ȡ������ֵ�������������Ӳ���޸Ĵ�ֵ
QueueHandle_t	ModuleQueue[maxTrackingModule];				//������Ϣ�������ڽ�����Ϣ
__IO ObjectInfo ObjectBuffer[maxTrackingObjects];	  //�������ֻ�ܱ���maxTrackingObjects����������� 
									
StctActionListItem ObjectInModuleList[maxTrackingModule][maxTrackingObjects];//��ά�������ڴ洢���ж����б�

static propActionRequestMachineData* 	pTempRequestMachineData = NULL;
static propActionSetOutput* 					pTempSetOutput      = NULL;
static propActionObjectTakeOver* 			pTempObjectTakeOver = NULL;
static propActionTriggerCamera* 			pTempTriggerCamera 	= NULL;
static propActionTriggerSensor* 			pTempTriggerSensor 	= NULL;
static propActionPushOut* 						pTempPushOut 				= NULL;

TaskHandle_t TrackingThread_Handler;
static void TrackingThread(void *arg);

static byte CreateObject(byte objectCNT, int moduleID, int encoder, int flag, int lastTriggerPreviousEncoder,int lastReceivePreviousEncoder)
{
	while(ObjectBuffer[objectCNT].objectAliveFlag != false)		//��ֹ���Ǹ�����δ�����Ķ���
	{
		objectCNT++;
		if(objectCNT > maxTrackingObjects - 1) 
		{
			objectCNT = 0;
			printf("Buffer is full Please check wether need to enlarge the Object Buffer!. \n");
			break;
		}
	}
	//�����󻺳��������������
	ObjectBuffer[objectCNT].ClientID = 0;								//������ֵ��
	ObjectBuffer[objectCNT].ObjectID = GlobalObjectID;					//�����ã���ʱ��ѭ��ͬ��
//	ObjectBuffer[objectCNT].BornEncoderNum = CaptureNumber;		//�ö������ʼ������ֵ(����ֵ)
	ObjectBuffer[objectCNT].pData 	 = "TestData";
	ObjectBuffer[objectCNT].ProcessedResult = true;							//������ֵ������������ʱ����Ϊ��
	ObjectBuffer[objectCNT].objectAliveFlag = true;							//������ٹ���
	
	GlobalObjectID++;

	return objectCNT;//���ش����Ķ����ڻ��������е�λ��
}

static void AddActionToList(byte i, int objectID, int targetValue, enum enumActionType actionType, byte actionNumber, byte outputType, byte outputChannel)
{
	int j =0;

	for(j = 0; j<maxTrackingObjects; j++)
	{
		if(!ObjectInModuleList[i][j].IsActionAlive)		//�жϴ˶����Ƿ�Alive�������ܸ��ǣ�������Ը���
		{
			ObjectInModuleList[i][j].ObjectID				= objectID;
			ObjectInModuleList[i][j].TargetValue		=	targetValue;
			ObjectInModuleList[i][j].ActionType			= actionType;
			ObjectInModuleList[i][j].ActionNumber 	= actionNumber;			//����Module ��Ķ�����ţ���parametersLoad����Ҫ�ġ�
			ObjectInModuleList[i][j].OutputType			=	outputType;
			ObjectInModuleList[i][j].OutputChannel	=	outputChannel;
			ObjectInModuleList[i][j].IsActionAlive	=	true;						//��Ǵ˶����Ѽ����б�ռ����һ��λ��
			break;
		}
		else if(j == maxTrackingObjects - 1) printf("The ActionList[%d] is full Please check it \n", i);
	}
}

static void AddALLActionToList(byte Module_i, int objectID, int64_t encoderNumber)
{
	byte Action_i = 0;
	for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
	{
		/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
		switch (ModuleConfig[Module_i].ActionInstanceConfig[Action_i].ActionType)
		{
			case ActRequestMachineData:
				//AddActionToList(byte i, int objectID, int targetValue, enum enumActionType actionType, byte actionNumber, byte outputType, byte outputChannel)
					 AddActionToList(Module_i, objectID, encoderNumber + ((propActionRequestMachineData*)ModuleConfig[Module_i].ActionInstanceConfig[Action_i].pActionConfig)->TargetValue, 
													 ActRequestMachineData, Action_i, NoOutput, 0);																				
			break;
			
			case ActSetOutput					: 
						AddActionToList(Module_i, objectID, encoderNumber + ((propActionSetOutput*)ModuleConfig[Module_i].ActionInstanceConfig[Action_i].pActionConfig)->TargetValue, 
														ActSetOutput, Action_i, TypeOutputDigital, 0);		
			break;
			
			case ActObjectTakeOver		: 
						AddActionToList(Module_i, objectID, encoderNumber + ((propActionObjectTakeOver*)ModuleConfig[Module_i].ActionInstanceConfig[Action_i].pActionConfig)->TargetValue, 
														ActObjectTakeOver, Action_i, NoOutput, 0);
			break;
			
			case ActTriggerCamera			:
						pTempTriggerCamera = (propActionTriggerCamera*)ModuleConfig[Module_i].ActionInstanceConfig[Action_i].pActionConfig;
						AddActionToList(Module_i, objectID, encoderNumber + pTempTriggerCamera->TargetValue, 
														ActTriggerCamera, Action_i, TypeOutputDigital, pTempTriggerCamera->DigitalOutput);													
			break;
			
			case ActTriggerSensor			:
						pTempTriggerSensor = (propActionTriggerSensor*)ModuleConfig[Module_i].ActionInstanceConfig[Action_i].pActionConfig;
						AddActionToList(Module_i, objectID, encoderNumber + pTempTriggerSensor->TargetValue, 
														ActTriggerSensor, Action_i, TypeOutputDigital, pTempTriggerSensor->SensorID);													
			break;
			
			case ActPushOut						: 
						pTempPushOut = (propActionPushOut*)ModuleConfig[Module_i].ActionInstanceConfig[Action_i].pActionConfig;
						AddActionToList(Module_i, objectID, encoderNumber + pTempPushOut->TargetValue, 
														ActPushOut, Action_i, TypeOutputResult, pTempPushOut->DigitalOutput);													
			break;
			
			default:break;
		}																			
	}
}


//������������
//����ֵ:0 �������񴴽��ɹ�
//		���� �������񴴽�ʧ��
uint8_t TrackingTask_init(void)
{
	uint8_t res;
	taskENTER_CRITICAL();              
	res = xTaskCreate((TaskFunction_t)TrackingThread,
					(const char*  )"TracingTread",
					(uint16_t     )TRACKING_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )TRACKING_TASK_PRIO,
					(TaskHandle_t*)&TrackingThread_Handler);
	taskEXIT_CRITICAL();

	return res;
}

//Tracking�����߳�
static void TrackingThread(void *arg)
{	
	long long temp = 0;
	byte GlobalObjectCount 	= 0;
//	uint16_t timeCount 			= 0;
	static bool AddActiveFlag 		= false;	
	static StctActionListItem objectTrackTemp ;				//���������ݴ�ֵ
//	static ModuleQueueItem* 	moduleQueueTemp;					//����һ���Ӷ�������ȡ���ݵ��ݴ�ֵָ��
//	Packet* pPacket = mymalloc(SRAMEX, 256);					//����һ���ݴ���
	
//	static __IO int64_t encoderNumber 		= 0;     		//����������ֵ
//	static __IO int64_t encoder1Number 		= 0;				//������1����ֵ
//	static __IO int64_t encoder2Number 		= 0;				//������2����ֵ
	
	static short triggerInterval 					= 0;				//���δ��������������
	static __IO int64_t LS1_EncoderNumTem = 0;				//���1��Ӧ����һ�α�����ֵ(�ݴ�����˫��������)
	static __IO int64_t LS2_EncoderNumTem = 0;				//���2��Ӧ����һ�α�����ֵ(�ݴ�����˫��������)
	static __IO int64_t LS3_EncoderNumTem = 0;				//���3��Ӧ����һ�α�����ֵ(�ݴ�����˫��������)
	static __IO int64_t LS4_EncoderNumTem = 0;				//���4��Ӧ����һ�α�����ֵ(�ݴ�����˫��������)
	
	
	
	byte Module_i = 0, Action_i = 0;												//�������Ʊ���
	byte Object_i = 0, i_cycle 	= 0;
	int data_len = 0;
	static __IO int64_t encoderNumber 		= 0;     					// ����������ֵ
	static __IO int64_t encoder1Number 		= 0;
	static __IO int64_t encoder2Number 		= 0;
	static __IO int64_t encoderDelivered 	= 0;	
//	static bool isActionIn = false;
	static ModuleQueueItem* moduleQueueTemp;						//����һ����������������ݵ��ݷ�ָ��
	static Packet* pPacket;															//���ݰ��׵�ַ
	uint32_t TimeCountStart = 0;												//���ڼ����߳�����ʱ��
	uint32_t TimeCountStartRTOS=0;
	
	uint32_t TestTime1=0;
	uint32_t TestTime2=0;
	uint32_t TestTime3=0;
	uint32_t TestTime4=0;
	uint32_t TestTime5=0;
	
	bool testFlag = false;
	uint16_t timeCount = 0;
//	uint16_t timeCount_for_cycle 	= 0;
//	uint16_t timeCountTime_Module = 0;
	BaseType_t err;
OverflowCount_TIM6 = 0;

	moduleQueueTemp = mymalloc(SRAMEX, sizeof(ModuleQueueItem));//�����ڴ��ʼ��
	pPacket 				= mymalloc(SRAMEX, 256);										//Ϊ���ݰ�����һ���̶���256�ֽڵ�������ʱ�洢��
	
	
	for(Object_i = 0; Object_i < maxTrackingObjects; Object_i++)							
	{
		/*�������Գ�ʼ��*/
		ObjectBuffer[Object_i].objectAliveFlag 	= false;				//��ʼ��������Ϊδ�������״̬
	}
	
	for(Module_i = 0; Module_i < maxTrackingModule; Module_i++)							
	{
		/*�������Գ�ʼ��*/
		for(Action_i = 0;Action_i	<	maxTrackingObjects;Action_i++)
			ObjectInModuleList[Object_i][Action_i].IsActionAlive 	= false;				//��ʼ�����ٶζ����б��еĶ���Ϊδ����״̬
	}
	
	//�������У�
	for(Module_i = 0;Module_i < maxTrackingModule;Module_i++)
	{
		ModuleQueue[Module_i] = xQueueCreate(moduleQueueDepth, sizeof(ModuleQueueItem));
		
	}
	moduleQueueTemp = mymalloc(SRAMEX, sizeof(ModuleQueueItem));//��ʼ��ָ��
	
	
/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����******************************************************************************				
*************************************************************************************************************************************************************/	
	while(1)
	{
		TimeCountStart 	= OverflowCount_TIM6*0XFFFF +__HAL_TIM_GET_COUNTER(&TIM6_Handler);//0.7us
		TimeCountStartRTOS = xTaskGetTickCount();
		
		if( ClientNum > 0)															//�м�����Inspection����
		{
			encoder1Number = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
			encoder2Number = (OverflowCount_Encoder8*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder8);
			
			for(Module_i = 0;Module_i < Module_Count;Module_i++)											
			{
											
				switch (ModuleConfig[Module_i].Encoder)
				{
					case Encoder_1: encoderNumber = encoder1Number;break;
					case Encoder_2: encoderNumber = encoder2Number;break;
				}

				if(xQueueReceive(ModuleQueue[Module_i], moduleQueueTemp, 0) == pdTRUE)	//�����������ݣ��ж������ݣ�ֵ���ݹ�����
				{
					/*******���ݶ����ļ��ʵ��****/
					if(ModuleConfig[Module_i].ModuleTrigger > 0)//�й��
					{
						/****���1***/					
						if((LightSignal_1 == ModuleConfig[Module_i].ModuleTrigger) && (TIM2CH1_CAPTURE_STA & 0X80))
						{
////							printf("==>Module[%d] Catch the Light111 Signals\n", Module_i);
////							temp=TIM2CH1_CAPTURE_STA&0X3F;							//�������
////							temp*=0XFFFFFFFF;		 	    									//���ʱ���ܺ�  ******Ӧ�ÿ���ȥ��
////							temp+=TIM2CH1_CAPTURE_VAL;      						//�õ��ܵĸߵ�ƽʱ��	
////							printf("HIGH:%lld us\r\n",temp);						//��ӡ�ܵĸߵ�ƽʱ�䡣
////							printf("Using Encoder[%d], EncoderNumber is [%lld]\n", ModuleConfig[Module_i].Encoder, encoderNumber);
////							
							TIM2CH1_CAPTURE_STA = 0;          								//���1������һ�β���		
							triggerInterval 		= encoderNumber - LS1_EncoderNumTem;
							LS1_EncoderNumTem 	= encoderNumber;					//��������һ�α������Ĵ���ֵ
							AddActiveFlag 			= true;
						}						
						/****���2***/					
						else if((LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger) && (TIM5CH1_CAPTURE_STA & 0X80))
						{
////							printf("==>Module[%d] Catch the Light222 \n", Module_i);
////							temp=TIM5CH1_CAPTURE_STA&0X3F;							//�������
////							temp*=0XFFFFFFFF;		 	    									//���ʱ���ܺ�  ******Ӧ�ÿ���ȥ��
////							temp+=TIM5CH1_CAPTURE_VAL;      						//�õ��ܵĸߵ�ƽʱ��	
////							printf("HIGH:%lld us\r\n",temp);						//��ӡ�ܵĸߵ�ƽʱ�䡣
////							printf("Using Encoder[%d], EncoderNumber is [%lld]\n", ModuleConfig[Module_i].Encoder, encoderNumber);
////							
							TIM5CH1_CAPTURE_STA = 0;          								//���1������һ�β���		
							triggerInterval 		= encoderNumber - LS2_EncoderNumTem;
							LS2_EncoderNumTem 	= encoderNumber;					//��������һ�α������Ĵ���ֵ
							AddActiveFlag 			= true;
						}
						if(AddActiveFlag == true)
						{
							AddActiveFlag = false;
							if(triggerInterval >  ModuleConfig[Module_i].Debounce	&&
							   triggerInterval > ModuleConfig[Module_i].DoubleTrigger)	//�źŷ�����˫��������
							{
								if(ModuleConfig[Module_i].CreateObjectFlag == true)		//�˸��ٶ�����������
								{	
									//ϵͳ��������������Ϊ0�������޷��������ֵ�ת��һ���������ŻῪʼ��������...
									if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//Ϊ���ݹ����Ķ��󣬽��������ڴ˶εĶ��������б�
									{
										pPacket = CreateObjectRunInPacket(pPacket, 1, moduleQueueTemp->DelieverdObjectID, Module_i, encoderNumber, 
																											0, encoderNumber, moduleQueueTemp->DelieverdEncoderNum);//����PC����XXX�����˸��ٶ�XXX
										data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
										i_cycle  = 0;
										for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//���������ӵĿͻ��˶����Ͷ���������Ϣ
										{
											WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
										}
									
										AddALLActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber);//�����ڸ��ٶ��б���Ӷ���						
									}
									else		//��Ϊ��һ��ȫ�µĶ����ˣ��������󣬲���Ŀǰ�������ڸ��ٶεĶ��������Ӧ�ĸ��ٶ��б������Ƿ�Ϊ��һ�����ٶζ�����ʹ�ã�
									{
										printf("Create A New Object, Delivered Object not comming in time. \n");
										if(GlobalObjectCount > maxTrackingObjects - 1) GlobalObjectCount = 0;//�����СΪ64��Count����С��64������ֵ��CreateObject�����
										GlobalObjectCount = CreateObject(GlobalObjectCount, Module_i, ModuleConfig[Module_i].Encoder, 1, encoderNumber, encoderNumber);
										GlobalObjectCount++;
											
										pPacket = CreateObjectRunInPacket(pPacket, 1, ObjectBuffer[GlobalObjectCount - 1].ObjectID, Module_i, encoderNumber, 
																											1, encoderNumber, moduleQueueTemp->DelieverdEncoderNum);//����PC����XXX�����˸��ٶ�XXX
										data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
										i_cycle  = 0;
										for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//���������ӵĿͻ��˶����Ͷ���������Ϣ
										{
											WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
										}	
										
										AddALLActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber);//�����ڸ��ٶ��б���Ӷ���																								
									}												
								}
								else if(ModuleConfig[Module_i].CreateObjectFlag != true)//�˸��ٶβ�����������
								{
									printf("Module[%d] not allowed to Create Object, Just Deliverd. \n", Module_i);
									//ϵͳ��������������Ϊ0�������޷��������ֵ�ת��һ���������ŻῪʼ��������...
									if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//Ϊ���ݹ����Ķ��󣬽��������ڴ˶εĶ��������б�
									{
										pPacket = CreateObjectRunInPacket(pPacket, 1,  moduleQueueTemp->DelieverdObjectID, Module_i, encoderNumber, 
																											0, encoderNumber, moduleQueueTemp->DelieverdEncoderNum);//����PC����XXX�����˸��ٶ�XXX
										data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
										i_cycle  = 0;
										for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//���������ӵĿͻ��˶����Ͷ���������Ϣ
										{
											WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
										}
										
										AddALLActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber);//�����ڸ��ٶ��б���Ӷ���		
									}
								}
							}							
						}							
					}
					/****���ٶ��޹��&&ֻ���ж���Ĵ���*****/
					else								
					{
printf("Module[%d] not have lightSensor, Just Deliverd. \n", Module_i);						
						//ϵͳ��������������Ϊ0�������޷���������ת��һ���������ŻῪʼ��������...
						if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//Ϊ���ݹ����Ķ��󣬽��������ڴ˶εĶ��������б�
						{
							pPacket = CreateObjectRunInPacket(pPacket, 1,  moduleQueueTemp->DelieverdObjectID, Module_i, encoderNumber, 
																								0, encoderNumber, moduleQueueTemp->DelieverdEncoderNum);//����PC����XXX�����˸��ٶ�XXX����Ϊ���ݵĶ���
							data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
							i_cycle  = 0;
							for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//���������ӵĿͻ��˶����Ͷ���������Ϣ
							{
								WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
							}
							
							AddALLActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber);//�����ڸ��ٶ��б���Ӷ���		
						}
					}
				}
			
				
				else			//�Ǵ��ݶ��������������Ĵ���...��Ϣ������û�ж��󴫵ݣ����ǹ���Ը�Ӧ�����ź�
				{
					if(ModuleConfig[Module_i].ModuleTrigger > 0)	//��ǰ���ٶ��й��
					{
						/*********���1*********/
						if((LightSignal_1 == ModuleConfig[Module_i].ModuleTrigger) && (TIM2CH1_CAPTURE_STA & 0X80) )		//������ٶ�ʹ�ù��1
						{
							if(ModuleConfig[Module_i].CreateObjectFlag == true)	//����������
							{
//								printf("==>Module[%d] Catch the Light111 Signals Not by Delivered Object Pretty New. \n", Module_i);
//								temp	= TIM2CH1_CAPTURE_STA&0X3F;							//�������
//								temp *= 0XFFFFFFFF;		 	    									//���ʱ���ܺ�  ******Ӧ�ÿ���ȥ��
//								temp += TIM2CH1_CAPTURE_VAL;      						//�õ��ܵĸߵ�ƽʱ��	
//								printf("HIGH:%lld us\r\n",temp);						//��ӡ�ܵĸߵ�ƽʱ�䡣
//								printf("Module[%d]==>Using Encoder[%d], EncoderNumber is [%lld]\n", Module_i, ModuleConfig[Module_i].Encoder, encoderNumber);
								TIM2CH1_CAPTURE_STA = 0;          								//���1������һ�β���			
								triggerInterval 		= encoderNumber - LS1_EncoderNumTem;
								LS1_EncoderNumTem 	= encoderNumber;					//��¼�ϴι�类������ֵ
								AddActiveFlag 			= true;
							}	
						}
						else if((LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger) && (TIM5CH1_CAPTURE_STA & 0X80) )		//������ٶ�ʹ�ù��2
						{						
							if(ModuleConfig[Module_i].CreateObjectFlag == true)	//���ٶ�����������
							{
//								printf("==>Module[%d] Catch the Light222 Signals Signals Not by Delivered Object Pretty New. \n", Module_i);
//								temp=TIM5CH1_CAPTURE_STA&0X3F;							//�������
//								temp*=0XFFFFFFFF;		 	    									//���ʱ���ܺ�  ******Ӧ�ÿ���ȥ��
//								temp+=TIM5CH1_CAPTURE_VAL;      						//�õ��ܵĸߵ�ƽʱ��	
//								printf("HIGH:%lld us\r\n",temp);						//��ӡ�ܵĸߵ�ƽʱ�䡣
//								printf("Module[%d]==>Using Encoder[%d], EncoderNumber is [%lld]\n", Module_i, ModuleConfig[Module_i].Encoder, encoderNumber);

								TIM5CH1_CAPTURE_STA = 0;          								//���2������һ�β���	
								triggerInterval 		= encoderNumber - LS2_EncoderNumTem;
								LS2_EncoderNumTem 	= encoderNumber;					//��¼�ϴι�类������ֵ
								AddActiveFlag 			= true;
							}
						}
							
						if(AddActiveFlag == true)
						{
							AddActiveFlag = false;
							
printf("Module[%d] not delvered, ModuleQueue empty ,Create Pretty New \n", Module_i);
							if(triggerInterval >  ModuleConfig[Module_i].Debounce	&&
							   triggerInterval > ModuleConfig[Module_i].DoubleTrigger)	//�źŷ�����˫��������
							{
								if(GlobalObjectCount > maxTrackingObjects - 1) GlobalObjectCount = 0;//�����СΪ64��Count����С��64������ֵ��CreateObject�����
								GlobalObjectCount = CreateObject(GlobalObjectCount, Module_i, ModuleConfig[Module_i].Encoder, 1, encoderNumber, encoderNumber);
								GlobalObjectCount++;
									
								pPacket = CreateObjectRunInPacket(pPacket, 1,  ObjectBuffer[GlobalObjectCount - 1].ObjectID, Module_i, encoderNumber, 
																									1,  encoderNumber, encoderNumber);//����PC����XXX�����˸��ٶ�XXX
								data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
								i_cycle  = 0;
								for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//���������ӵĿͻ��˶����Ͷ���������Ϣ
								{
									WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
								}
								
								AddALLActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber);//�����ڸ��ٶ��б���Ӷ���										
							}
						}						
					}
				}		
			}		
		
			
			
/************************************************************************************************************************************************************			
*****************************************************************����ִ��-->ɨ���б�*************************************************************************
*************************************************************************************************************************************************************/		
			for(Module_i = 0;Module_i < Module_Count;Module_i++)
			{ 
				switch (ModuleConfig[Module_i].Encoder)
				{
					case Encoder_1: encoderNumber = encoder1Number;break;
					case Encoder_2: encoderNumber = encoder2Number;break;
				}	
				
				for(Action_i = 0; Action_i < maxTrackingObjects/2; Action_i++)//����ȽϺ�ʱmaxTrackingObjects 64��1420us, 32: 
				{
					if(ObjectInModuleList[Module_i][Action_i].IsActionAlive)
					{
						if(encoderNumber > ObjectInModuleList[Module_i][Action_i].TargetValue)
						{	
//							isActionIn = true;
							
							switch(ObjectInModuleList[Module_i][Action_i].ActionType)		//13us
							{
								case ActRequestMachineData:
printf("Get into ActRequestMachineData\r\n");
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								testFlag = true;
								break;
								
								case ActSetOutput					: 
printf("Get into ActSetOutput\r\n");									
									xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerOutput, eSetValueWithOverwrite, NULL);
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
								testFlag = true;
								break;
								
								case ActObjectTakeOver		:
printf("Get into ActObjectTakeOver\r\n");										
/*ConsumeTime:20us*/pTempObjectTakeOver = (propActionObjectTakeOver*)ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].pActionConfig;
								  switch (ModuleConfig[pTempObjectTakeOver->DestinationModule].Encoder)
									{
										case Encoder_1: encoderDelivered = encoder1Number;break;
										case Encoder_2: encoderDelivered = encoder2Number;;break;
									}
									moduleQueueTemp->DelieverdEncoderNum 	= encoderDelivered;
									moduleQueueTemp->DelieverdObjectID		=	ObjectInModuleList[Module_i][Action_i].ObjectID;			
									//moduleQueueTemp->ClientID							= 
									
									err = xQueueSend(ModuleQueue[pTempObjectTakeOver->DestinationModule], moduleQueueTemp, 0);		//
/*ConsumeTime:292usALL*/if(err==errQUEUE_FULL) printf("Queue is Full Send Failed!\r\n");
									//��Ҫ���͵����ݴ��ݵ�����buffer
									
									//1. ��ʱ1022us(��̬�����ڴ��)��2. 86us������pPacket�ģ�
/*ConsumeTime:96usALL*/pPacket = CreateObjectRunOutPacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber, pTempObjectTakeOver->DestinationModule);

									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;//��ʱ24us
									i_cycle = 0;
									for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//���������ӵĿͻ��˶����Ͷ���������Ϣ
									{
										WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
									}
								
/*ConsumeTime:550usALL*/ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
									testFlag = true;
									break;
								
								case ActTriggerCamera			: 
printf("Get into ActTriggerCamera\r\n");										
/*ConsumeTime:32us*/pTempTriggerCamera = (propActionTriggerCamera*)ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].pActionConfig;

/*ConsumeTime:720us*/xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerCamera, eSetValueWithOverwrite, NULL);	//����֪ͨȥ�������

/*ConsumeTime:100us*/pPacket = CreateTriggerCameraPacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber,
																											pTempTriggerCamera->CameraID, 1);

									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4; /*ConsumeTime:160us*///��������WriteDataToBufferSend��ʱ160us
									i_cycle  = 0;
									while(Session[i_cycle].ClientID != pTempTriggerCamera->ClientID)//pTempTriggerCamera->ClientID
									{
										i_cycle++;
										if(i_cycle > ClientNum)
										{
											printf("Cannot find the session pTempTriggerCamera->ClientID[%d]\r\n", pTempTriggerCamera->ClientID);
											i_cycle = 0;
											break;
										}
									}			
/*ConsumeTime:160us*/WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);
								
/*ALL ConsumeTime:1002us*/ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
									testFlag = true;
								break;
								
								case ActTriggerSensor			: /*ConsumeTime:938us*/
printf("Get into ActTriggerSensor\r\n");	
									pTempTriggerSensor = (propActionTriggerSensor*)ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].pActionConfig;
									xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerSensor, eSetValueWithOverwrite, NULL);	
								
									pPacket = CreateTriggerIOSensorPacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber, pTempTriggerSensor->SensorID);
								
									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
									i_cycle  = 0;
									while(Session[i_cycle].ClientID != pTempTriggerSensor->ClientID)//pTempTriggerCamera->ClientID
									{
										i_cycle++;
										if(i_cycle > ClientNum)
										{
											printf("Cannot find the session pTempTriggerSensor->ClientID[%d]\r\n", pTempTriggerCamera->ClientID);
											i_cycle = 0;
											break;
										}
									}			
									WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);
				
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
									testFlag = true;
									break;
								
								case ActPushOut						: /*ConsumeTime:256us*/		
printf("Get into ActPushOut\r\n");		
									//��Packet�����⣬��֪�к�����
									pPacket = CreateObjectDeletePacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber);				
								
									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
									i_cycle  = 0;
									for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//���������ӵĿͻ��˶����Ͷ���������Ϣ
									{
										if(Session[i_cycle].ClientID != ClientServer)
										WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
									}						
////								
									/******����Ѱ�Ҵ˶����ڻ����������λ�ã� �˴������ͷŶ�Ӧ��ObjectBuffer���󣬷����޷��ٴ��������޷���ObjectBuffer�����ֵ*****/
									Object_i = 0 ;
									while(ObjectBuffer[Object_i].ObjectID != ObjectInModuleList[Module_i][Action_i].ObjectID)
									{
										Object_i++;
										if(Object_i > (maxTrackingObjects - 1)) 
										{
											printf("Cannot Find The consistent ObjectID in ObjectBuffer failed In Tracking. \n");
											break;
										}	
									}
									if(ObjectBuffer[Object_i].ProcessedResult == false)	
									{
										xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerPush, 	 eSetValueWithOverwrite, NULL);//����֪ͨȥ�����޳�																							
									}
									ObjectBuffer[Object_i].objectAliveFlag = false;									//�ͷű�ռ�еĶ���
																		
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;		//�ͷ�ռ�еĶ���
									testFlag = true;
									break;
								
								default:break;
							}
//						break;
						}
					}
				}			
			
//printf("Get for whole List Cycle Time ==>%d \n", __HAL_TIM_GET_COUNTER(&TIM6_Handler) - timeCount_for_cycle);//����440usһ��ѭ����3��Ϊ1320us
//				
//				if(isActionIn == true)	//�����������Moduleѭ��
//				{
//					isActionIn = false;
//					break;
//				}
//			
			}
//printf("Sending Module whole cycle Spending time  ==>%d \n", __HAL_TIM_GET_COUNTER(&TIM6_Handler) - timeCountTime_Module);//1378us
			

/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����END******************************************************************************				
*************************************************************************************************************************************************************/				

/***����ʱ��ͳ��***/
//////			
//////			TimeCountStart 	=  OverflowCount_TIM6*0XFFFF + __HAL_TIM_GET_COUNTER(&TIM6_Handler) - TimeCountStart;		//11us
//////			TimeCountStartRTOS = xTaskGetTickCount() - TimeCountStartRTOS;
//////			
//////			if(TimeCountStart > 800 && TimeCountStart<65536 && testFlag == true)//��֪Ϊ�λ���ִ���65536+TimeCountStart(����)�ļ���ֵ�������...
//////			{	
//////				printf("TrackingTime ==>%d \n", TimeCountStart);//No-Load:1430us   Load:1600
//////				//printf("TrackingTimeRTOSSSS ==>%d \n", TimeCountStartRTOS);
//////				testFlag = false;
//////			}	

		}
	vTaskDelay(1);																								//������1ms�Ļ�����һֱ���������	
	}	
}

