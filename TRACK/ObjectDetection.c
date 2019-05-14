/**************************************************************************************************
*************************************�����߳�******************************************************
***************************************************************************************************/
#include "ObjectDetection.h"
#include <math.h>
#include "TCPProtocol.h"

TaskHandle_t ObjectDetectionThread_Handler;
QueueHandle_t	ModuleQueue[maxTrackingModule];				//������Ϣ�������ڽ�����Ϣ
#define moduleQueueDepth	10

static char* strCreateObject = "<=================Detect someting Create Objectr===================>";
 
__IO ObjectInfo ObjectBuffer[maxTrackingObjects];										//�������ֻ�ܱ���maxTrackingObjects����������� 
//ObjectList ObjectInModuleList[maxTrackingModule];								//����һ�����ٶ�ָ���б�����

StctActionListItem ObjectInModuleList[maxTrackingModule][maxTrackingObjects];//��ά�������ڴ洢���ж����б�

__IO	uint32_t GlobalObjectID;																	//����һ��ȫ�ֶ���ID��__IO��ʾֱ�Ӵӵ�ַ��ȡֵ��ȡ������ֵ�������������Ӳ���޸Ĵ�ֵ

static void ObjectDetectionThread(void);


static byte CreateObject(byte objectCNT, int moduleID, int encoder, int flag, int lastTriggerPreviousEncoder,int lastReceivePreviousEncoder)
{
	Packet* pPacket;
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
	ObjectBuffer[objectCNT].pData 	 = strCreateObject;
	ObjectBuffer[objectCNT].ProcessedResult = true;							//������ֵ������������ʱ����Ϊ��
	ObjectBuffer[objectCNT].objectAliveFlag = true;							//������ٹ���
	
	GlobalObjectID++;

	pPacket = CreateStartTrackingPacket(1, GlobalObjectID);
	printf("%s\r\n",(char*)pPacket);
	TCPSendPacket(ClientServer, pPacket);
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


//��������̽������
//����ֵ:0 ̽�����񴴽��ɹ�
//		���� ̽�����񴴽�ʧ��
uint8_t ObjectDetectionTask_init(void)
{
	uint8_t res;
	taskENTER_CRITICAL();              
	res = xTaskCreate((TaskFunction_t)ObjectDetectionThread,
					(const char*  )"ObjectDetectionTread",
					(uint16_t     )OBJECTDETECTION_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )OBJECTDETECTION_TASK_PRIO,
					(TaskHandle_t*)&ObjectDetectionThread_Handler);
	taskEXIT_CRITICAL();

	return res;
}

//����̽���߳�
static void ObjectDetectionThread(void)
{
	long long temp = 0;			
	byte Object_i = 0, Module_i = 0, Action_i = 0;		//�������Ʊ���
	byte GlobalObjectCount = 0;
	uint32_t TimeCountStart = 0;											//���ڼ����߳�����ʱ��
	uint16_t timeCount = 0;
	static bool AddActiveFlag = false;	
	static StctActionListItem objectTrackTemp ;				//���������ݴ�ֵ
	static ModuleQueueItem* moduleQueueTemp;					//����һ���Ӷ�������ȡ���ݵ��ݴ�ֵָ��
	static __IO int64_t encoderNumber = 0;     				// ����������ֵ
	static __IO int64_t encoder1Number = 0;
	static __IO int64_t encoder2Number = 0;
	
	static short triggerInterval = 0;									//���δ��������������
	static __IO int64_t LS1_EncoderNumTem = 0;				//���1��Ӧ����һ�α�����ֵ(�ݴ�����˫��������)
	static __IO int64_t LS2_EncoderNumTem = 0;				//���2��Ӧ����һ�α�����ֵ(�ݴ�����˫��������)
	static __IO int64_t LS3_EncoderNumTem = 0;				//���3��Ӧ����һ�α�����ֵ(�ݴ�����˫��������)
	static __IO int64_t LS4_EncoderNumTem = 0;				//���4��Ӧ����һ�α�����ֵ(�ݴ�����˫��������)
	
	
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
*****************************************************************���ٹ��̼����*********************************************************************************				
*************************************************************************************************************************************************************/				
	while(1)
	{	
		TimeCountStart = xTaskGetTickCount();
		
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
							printf("==>Module[%d] Catch the Light111 Signals\n", Module_i);
							temp=TIM2CH1_CAPTURE_STA&0X3F;							//�������
							temp*=0XFFFFFFFF;		 	    									//���ʱ���ܺ�  ******Ӧ�ÿ���ȥ��
							temp+=TIM2CH1_CAPTURE_VAL;      						//�õ��ܵĸߵ�ƽʱ��	
							printf("HIGH:%lld us\r\n",temp);						//��ӡ�ܵĸߵ�ƽʱ�䡣
							printf("Using Encoder[%d], EncoderNumber is [%lld]\n", ModuleConfig[Module_i].Encoder, encoderNumber);
							
							TIM2CH1_CAPTURE_STA = 0;          								//���1������һ�β���		
							triggerInterval 	= encoderNumber - LS1_EncoderNumTem;
							LS1_EncoderNumTem = encoderNumber;					//��������һ�α������Ĵ���ֵ
							AddActiveFlag = true;
						}						
						/****���2***/					
						else if((LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger) && (TIM5CH1_CAPTURE_STA & 0X80))
						{
							printf("==>Module[%d] Catch the Light222 \n", Module_i);
							temp=TIM5CH1_CAPTURE_STA&0X3F;							//�������
							temp*=0XFFFFFFFF;		 	    									//���ʱ���ܺ�  ******Ӧ�ÿ���ȥ��
							temp+=TIM5CH1_CAPTURE_VAL;      						//�õ��ܵĸߵ�ƽʱ��	
							printf("HIGH:%lld us\r\n",temp);						//��ӡ�ܵĸߵ�ƽʱ�䡣
							printf("Using Encoder[%d], EncoderNumber is [%lld]\n", ModuleConfig[Module_i].Encoder, encoderNumber);
							
							TIM5CH1_CAPTURE_STA = 0;          								//���1������һ�β���		
							triggerInterval 	= encoderNumber - LS2_EncoderNumTem;
							LS2_EncoderNumTem = encoderNumber;					//��������һ�α������Ĵ���ֵ
							AddActiveFlag = true;
						}
						if(AddActiveFlag == true)
						{
							if(triggerInterval >  ModuleConfig[Module_i].Debounce	&&
							   triggerInterval > ModuleConfig[Module_i].DoubleTrigger)	//�źŷ�����˫��������
							{
								if(ModuleConfig[Module_i].CreateObjectFlag == true)		//�˸��ٶ�����������
								{	
									//ϵͳ��������������Ϊ0�������޷��������ֵ�ת��һ���������ŻῪʼ��������...
									if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//Ϊ���ݹ����Ķ��󣬽��������ڴ˶εĶ��������б�
									{
										for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
										{
											/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
											switch (ModuleConfig[Module_i].ActionInstanceConfig[Action_i].ActionType)
											{
												case ActRequestMachineData:
													//AddActionToList(byte i, int objectID, int targetValue, enum enumActionType actionType, byte actionNumber, byte outputType, byte outputChannel)
														 AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue, 
																						 ActRequestMachineData, Action_i, NoOutput, 0);																				
												break;
												
												case ActSetOutput					: 
															AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue, 
																							ActSetOutput, Action_i, TypeOutputDigital, 0);		
												break;
												
												case ActObjectTakeOver		: 
															AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue, 
																							ActObjectTakeOver, Action_i, NoOutput, 0);
												break;
												
												case ActTriggerCamera			: 
															AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue, 
																							ActTriggerCamera, Action_i, TypeOutputDigital, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.DigitalOutput);													
												break;
												
												case ActTriggerSensor			: 
															AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue, 
																							ActTriggerSensor, Action_i, TypeOutputDigital, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.SensorID);													
												break;
												
												case ActPushOut						: 
															AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue, 
																							ActPushOut, Action_i, TypeOutputResult, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.DigitalOutput);													
												break;
												
												default:break;
											}																			
										}
									}
									else		//��Ϊ��һ��ȫ�µĶ����ˣ��������󣬲���Ŀǰ�������ڸ��ٶεĶ��������Ӧ�ĸ��ٶ��б������Ƿ�Ϊ��һ�����ٶζ�����ʹ�ã�
									{
										printf("Create A New Object, Delivered Object not comming in time. \n");
										if(GlobalObjectCount > maxTrackingObjects - 1) GlobalObjectCount = 0;//�����СΪ64��Count����С��64������ֵ��CreateObject�����
										GlobalObjectCount = CreateObject(GlobalObjectCount, Module_i, ModuleConfig[Module_i].Encoder, 1, encoderNumber, encoderNumber);
										GlobalObjectCount++;
																				
										/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
										switch (ModuleConfig[Module_i].ActionInstanceConfig[Action_i].ActionType)
										{
											case ActRequestMachineData:
													 AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue, 
																					 ActRequestMachineData, Action_i, NoOutput, 0);																				
											break;
											
											case ActSetOutput					: 
														AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue, 
																						ActSetOutput, Action_i, TypeOutputDigital, 0);		
											break;
											
											case ActObjectTakeOver		: 
														AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue, 
																						ActObjectTakeOver, Action_i, NoOutput, 0);
											break;
											
											case ActTriggerCamera			: 
														AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue, 
																						ActTriggerCamera, Action_i, TypeOutputDigital, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.DigitalOutput);													
											break;
											
											case ActTriggerSensor			: 
														AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue, 
																						ActTriggerSensor, Action_i, TypeOutputDigital, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.SensorID);													
											break;
											
											case ActPushOut						: 
														AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue, 
																						ActPushOut, Action_i, TypeOutputResult, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.DigitalOutput);													
											break;
											
											default:break;
										}																			
									}												
								}
								else if(ModuleConfig[Module_i].CreateObjectFlag != true)//�˸��ٶβ�����������
								{
									printf("Module[%d] not allowed to Create Object, Just Deliverd. \n", Module_i);
									//ϵͳ��������������Ϊ0�������޷��������ֵ�ת��һ���������ŻῪʼ��������...
									if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//Ϊ���ݹ����Ķ��󣬽��������ڴ˶εĶ��������б�
									{

										for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
										{
											/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
											switch (ModuleConfig[Module_i].ActionInstanceConfig[Action_i].ActionType)
											{
												case ActRequestMachineData:
														 AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue, 
																						 ActRequestMachineData, Action_i, NoOutput, 0);																				
												break;
												
												case ActSetOutput					: 
															AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue, 
																							ActSetOutput, Action_i, TypeOutputDigital, 0);		
												break;
												
												case ActObjectTakeOver		: 
															AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue, 
																							ActObjectTakeOver, Action_i, NoOutput, 0);
												break;
												
												case ActTriggerCamera			: 
															AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue, 
																							ActTriggerCamera, Action_i, TypeOutputDigital, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.DigitalOutput);													
												break;
												
												case ActTriggerSensor			: 
															AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue, 
																							ActTriggerSensor, Action_i, TypeOutputDigital, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.SensorID);													
												break;
												
												case ActPushOut						: 
															AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue, 
																							ActPushOut, Action_i, TypeOutputResult, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.DigitalOutput);													
												break;
												
												default:break;
											}																			
										}
									}
								}
							}							
							AddActiveFlag = false;
						}							
					}
					/****���ٶ��޹��&&ֻ���ж���Ĵ���*****/
					else								
					{	
						//ϵͳ��������������Ϊ0�������޷���������ת��һ���������ŻῪʼ��������...
						if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//Ϊ���ݹ����Ķ��󣬽��������ڴ˶εĶ��������б�
						{
							for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
							{
								/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
								switch (ModuleConfig[Module_i].ActionInstanceConfig[Action_i].ActionType)
								{
									case ActRequestMachineData:
											 AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue, 
																			 ActRequestMachineData, Action_i, NoOutput, 0);																				
									break;
									
									case ActSetOutput					: 
												AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue, 
																				ActSetOutput, Action_i, TypeOutputDigital, 0);		
									break;
									
									case ActObjectTakeOver		: 
												AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue, 
																				ActObjectTakeOver, Action_i, NoOutput, 0);
									break;
									
									case ActTriggerCamera			: 
												AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue, 
																				ActTriggerCamera, Action_i, TypeOutputDigital, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.DigitalOutput);													
									break;
									
									case ActTriggerSensor			: 
												AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue, 
																				ActTriggerSensor, Action_i, TypeOutputDigital, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.SensorID);													
									break;
									
									case ActPushOut						: 
												AddActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue, 
																				ActPushOut, Action_i, TypeOutputResult, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.DigitalOutput);													
									break;
									
									default:break;
								}																			
							}
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
								printf("==>Module[%d] Catch the Light111 Signals Not by Delivered Object Pretty New. \n", Module_i);
								temp=TIM2CH1_CAPTURE_STA&0X3F;							//�������
								temp*=0XFFFFFFFF;		 	    									//���ʱ���ܺ�  ******Ӧ�ÿ���ȥ��
								temp+=TIM2CH1_CAPTURE_VAL;      						//�õ��ܵĸߵ�ƽʱ��	
								printf("HIGH:%lld us\r\n",temp);						//��ӡ�ܵĸߵ�ƽʱ�䡣
								printf("Module[%d]==>Using Encoder[%d], EncoderNumber is [%lld]\n", Module_i, ModuleConfig[Module_i].Encoder, encoderNumber);

								TIM2CH1_CAPTURE_STA = 0;          								//���1������һ�β���			
								triggerInterval = encoderNumber - LS1_EncoderNumTem;
								LS1_EncoderNumTem = encoderNumber;					//��¼�ϴι�类������ֵ
								AddActiveFlag = true;
							}	
						}
						else if((LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger) && (TIM5CH1_CAPTURE_STA & 0X80) )		//������ٶ�ʹ�ù��2
						{						
							if(ModuleConfig[Module_i].CreateObjectFlag == true)	//���ٶ�����������
							{
								printf("==>Module[%d] Catch the Light222 Signals Signals Not by Delivered Object Pretty New. \n", Module_i);
								temp=TIM5CH1_CAPTURE_STA&0X3F;							//�������
								temp*=0XFFFFFFFF;		 	    									//���ʱ���ܺ�  ******Ӧ�ÿ���ȥ��
								temp+=TIM5CH1_CAPTURE_VAL;      						//�õ��ܵĸߵ�ƽʱ��	
								printf("HIGH:%lld us\r\n",temp);						//��ӡ�ܵĸߵ�ƽʱ�䡣
								printf("Module[%d]==>Using Encoder[%d], EncoderNumber is [%lld]\n", Module_i, ModuleConfig[Module_i].Encoder, encoderNumber);

								TIM5CH1_CAPTURE_STA = 0;          								//���2������һ�β���	
								triggerInterval = encoderNumber - LS2_EncoderNumTem;
								LS2_EncoderNumTem = encoderNumber;					//��¼�ϴι�类������ֵ
								AddActiveFlag = true;
							}
						}
							
						if(AddActiveFlag == true)
						{
							if(triggerInterval >  ModuleConfig[Module_i].Debounce	&&
							   triggerInterval > ModuleConfig[Module_i].DoubleTrigger)	//�źŷ�����˫��������
							{
								if(GlobalObjectCount > maxTrackingObjects - 1) GlobalObjectCount = 0;//�����СΪ64��Count����С��64������ֵ��CreateObject�����
								GlobalObjectCount = CreateObject(GlobalObjectCount, Module_i, ModuleConfig[Module_i].Encoder, 1, encoderNumber, encoderNumber);
								GlobalObjectCount++;
																
								/*�����ٶ��б�����Ӧ�Ķ���*/
								for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
								{
									/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
									switch (ModuleConfig[Module_i].ActionInstanceConfig[Action_i].ActionType)
									{
										case ActRequestMachineData:
												 AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue, 
																				 ActRequestMachineData, Action_i, NoOutput, 0);																				
										break;
										
										case ActSetOutput					: 
													AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue, 
																					ActSetOutput, Action_i, TypeOutputDigital, 0);		
										break;
										
										case ActObjectTakeOver		: 
													AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue, 
																					ActObjectTakeOver, Action_i, NoOutput, 0);
										break;
										
										case ActTriggerCamera			: 
													AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue, 
																					ActTriggerCamera, Action_i, TypeOutputDigital, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.DigitalOutput);													
										break;
										
										case ActTriggerSensor			: 
													AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue, 
																					ActTriggerSensor, Action_i, TypeOutputDigital, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.SensorID);													
										break;
										
										case ActPushOut						: 
													AddActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue, 
																					ActPushOut, Action_i, TypeOutputResult, ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.DigitalOutput);													
										break;
										
										default:break;
									}												
								}								
								AddActiveFlag = false;
							}
						}						
					}
				}		
			}		
		}
/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����END*********************************************************************************				
*************************************************************************************************************************************************************/		
			
//		if(Timer > 3020) Timer = 0;		//�ظ���������
		
		timeCount = xTaskGetTickCount()- TimeCountStart;
		if(timeCount > 0)
		{
			printf("ObjectDetection Thread Running Time ==>%d \n", timeCount);
		}

	vTaskDelay(1);																								//������1ms�Ļ�����һֱ���������
	}	
}

