/**************************************************************************************************
*************************************�����߳�******************************************************
***************************************************************************************************/
#include "ObjectDetection.h"
#include <math.h>
#include "MyList.h"									//���ٶζ����б�

TaskHandle_t ObjectDetectionThread_Handler;
QueueHandle_t	ModuleQueue[maxTrackingModule];				//������Ϣ�������ڽ�����Ϣ	

static char* strCreateObject = "<=================Detect someting Create Objectr===================>";
 
__IO ObjectInfo ObjectBuffer[maxTrackingObjects];										//�������ֻ�ܱ���maxTrackingObjects����������� 
ObjectList ObjectInModuleList[maxTrackingModule];								//����һ�����ٶ�ָ���б�����
__IO	uint32_t GlobalObjectID;																	//����һ��ȫ�ֶ���ID��__IO��ʾֱ�Ӵӵ�ַ��ȡֵ��ȡ������ֵ�������������Ӳ���޸Ĵ�ֵ

static void ObjectDetectionThread(void);


static byte CreateObject(byte objectCNT)
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
	ObjectBuffer[objectCNT].ClientID = 0;											//������ֵ��
	ObjectBuffer[objectCNT].ObjectID = GlobalObjectID;							//�����ã���ʱ��ѭ��ͬ��
//	ObjectBuffer[objectCNT].BornEncoderNum = CaptureNumber;		//�ö������ʼ������ֵ(����ֵ)
	ObjectBuffer[objectCNT].pData = strCreateObject;
	ObjectBuffer[objectCNT].ProcessedResult = true;						//������ֵ������������ʱ����Ϊ��
	ObjectBuffer[objectCNT].objectAliveFlag = true;						//������ٹ���
	
	GlobalObjectID++;
	TCPSendDataChar(ObjectBuffer[objectCNT].ClientID, strCreateObject);		
	
	return objectCNT;//���ش����Ķ����ڻ��������е�λ��
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
	static ObjectTrack objectTrackTemp ;				//���������ݴ�ֵ
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
	/***����Module�б����Ϣ���в���ʼ����ͷ***/
	for(Module_i = 0; Module_i <Module_Count;Module_i++)
	{
		InitList(&ObjectInModuleList[Module_i]);																							//�����б�
		ModuleQueue[Module_i] = xQueueCreate(ModuleQueueLength, sizeof(ModuleQueueItem));		//������Ϣ����
		printf("ModuleQueueItem size = %d \n", sizeof(ModuleQueueItem));
		printf("Address of ObjectInModuleList[%d] = %p \n", Module_i, ObjectInModuleList[Module_i]);
		printf("Address of ObjectInModuleList[%d]->next = %p \n", Module_i, ObjectInModuleList[Module_i]->next);
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
											objectTrackTemp.ObjectID 				= moduleQueueTemp->DelieverdObjectID;
											//objectTrackTemp.RefEncoderNum 	= encoderNumber;
											/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
											if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionTriggerCamera;
												objectTrackTemp.OutputType 		= TypeOutputDigital;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.DigitalOutput;									
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���											
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionObjectTakeOver;
												objectTrackTemp.OutputType 		= NoOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionSetOutput;
												objectTrackTemp.OutputType 		= TypeOutputDigital;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.DigitalOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionTriggerSensor;
												objectTrackTemp.OutputType 		= TypeOutputDigital;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.SensorID;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionPushOut;
												objectTrackTemp.OutputType 		= TypeOutputResult;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.DigitalOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionRequestMachineData;
												objectTrackTemp.OutputType 		= NoOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
										}
									}
									else		//��Ϊ��һ��ȫ�µĶ����ˣ��������󣬲���Ŀǰ�������ڸ��ٶεĶ��������Ӧ�ĸ��ٶ��б������Ƿ�Ϊ��һ�����ٶζ�����ʹ�ã�
									{
										printf("Create A New Object, Delivered Object not comming in time. \n");
										if(GlobalObjectCount > maxTrackingObjects - 1) GlobalObjectCount = 0;//�����СΪ64��Count����С��64������ֵ��CreateObject�����
										GlobalObjectCount = CreateObject(GlobalObjectCount);
										GlobalObjectCount++;
										
										for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
										{
											objectTrackTemp.ObjectID 				= ObjectBuffer[GlobalObjectCount - 1].ObjectID;
											//objectTrackTemp.RefEncoderNum 	= encoderNumber;
											/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
											if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionTriggerCamera;
												objectTrackTemp.OutputType 		= TypeOutputDigital;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.DigitalOutput;									
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionObjectTakeOver;
												objectTrackTemp.OutputType 		= NoOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionSetOutput;
												objectTrackTemp.OutputType 		= TypeOutputDigital;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.DigitalOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionTriggerSensor;
												objectTrackTemp.OutputType 		= TypeOutputDigital;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.SensorID;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionPushOut;
												objectTrackTemp.OutputType 		= TypeOutputResult;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.DigitalOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionRequestMachineData;
												objectTrackTemp.OutputType 		= NoOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
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
											objectTrackTemp.ObjectID 				= moduleQueueTemp->DelieverdObjectID;
											//objectTrackTemp.RefEncoderNum 	= encoderNumber;
											/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
											if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionTriggerCamera;
												objectTrackTemp.OutputType 		= TypeOutputDigital;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.DigitalOutput;									
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionObjectTakeOver;
												objectTrackTemp.OutputType 		= NoOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionSetOutput;
												objectTrackTemp.OutputType 		= TypeOutputDigital;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.DigitalOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionTriggerSensor;
												objectTrackTemp.OutputType 		= TypeOutputDigital;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.SensorID;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionPushOut;
												objectTrackTemp.OutputType 		= TypeOutputResult;
												objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.DigitalOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
											}
											else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.ActionTriggerFlag == true)
											{
												objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue;
												objectTrackTemp.ActionType 		= TypeActionRequestMachineData;
												objectTrackTemp.OutputType 		= NoOutput;
												objectTrackTemp.ActionNumber	= Action_i;
												vTaskSuspendAll();		//�����ٽ�����ֹ�����
												ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
												xTaskResumeAll();		//�˳��ٽ���	
												continue;
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
								objectTrackTemp.ObjectID 				= moduleQueueTemp->DelieverdObjectID;
								//objectTrackTemp.RefEncoderNum 	= encoderNumber;
								/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
								if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.ActionTriggerFlag == true)
								{
									objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue;
									objectTrackTemp.ActionType 		= TypeActionTriggerCamera;
									objectTrackTemp.OutputType 		= TypeOutputDigital;
									objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.DigitalOutput;									
									objectTrackTemp.ActionNumber	= Action_i;
									vTaskSuspendAll();		//�����ٽ�����ֹ�����
									ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
									xTaskResumeAll();		//�˳��ٽ���	
									continue;
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.ActionTriggerFlag == true)
								{
									objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;
									objectTrackTemp.ActionType 		= TypeActionObjectTakeOver;
									objectTrackTemp.OutputType 		= NoOutput;
									objectTrackTemp.ActionNumber	= Action_i;
									vTaskSuspendAll();		//�����ٽ�����ֹ�����
									ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
									xTaskResumeAll();		//�˳��ٽ���	
									continue;
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.ActionTriggerFlag == true)
								{
									objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue;
									objectTrackTemp.ActionType 		= TypeActionSetOutput;
									objectTrackTemp.OutputType 		= TypeOutputDigital;
									objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.DigitalOutput;
									objectTrackTemp.ActionNumber	= Action_i;
									vTaskSuspendAll();		//�����ٽ�����ֹ�����
									ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
									xTaskResumeAll();		//�˳��ٽ���	
									continue;
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.ActionTriggerFlag == true)
								{
									objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue;
									objectTrackTemp.ActionType 		= TypeActionTriggerSensor;
									objectTrackTemp.OutputType 		= TypeOutputDigital;
									objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.SensorID;
									objectTrackTemp.ActionNumber	= Action_i;
									vTaskSuspendAll();		//�����ٽ�����ֹ�����
									ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
									xTaskResumeAll();		//�˳��ٽ���	
									continue;
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.ActionTriggerFlag == true)
								{
									objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue;
									objectTrackTemp.ActionType 		= TypeActionPushOut;
									objectTrackTemp.OutputType 		= TypeOutputResult;
									objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.DigitalOutput;
									objectTrackTemp.ActionNumber	= Action_i;
									vTaskSuspendAll();		//�����ٽ�����ֹ�����
									ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
									xTaskResumeAll();		//�˳��ٽ���	
									continue;
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.ActionTriggerFlag == true)
								{
									objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue;
									objectTrackTemp.ActionType 		= TypeActionRequestMachineData;
									objectTrackTemp.OutputType 		= NoOutput;
									objectTrackTemp.ActionNumber	= Action_i;
									vTaskSuspendAll();		//�����ٽ�����ֹ�����
									ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
									xTaskResumeAll();		//�˳��ٽ���	
									continue;
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
								printf("==>Module[%d] Catch the Light222 Signals. \n", Module_i);
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
								GlobalObjectCount = CreateObject(GlobalObjectCount);		//�������󲢷��������Ķ�����buffer���λ�ã���Ϊ���ܳ����봴����λ���ϵĶ�����δ������ɣ����ɺ��������ҵ����е�λ�ô������󲢷���λ��
								GlobalObjectCount++;
								
								
								/*�����ٶ��б�����Ӧ�Ķ���*/
								for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
								{
									objectTrackTemp.ObjectID 				= ObjectBuffer[GlobalObjectCount - 1].ObjectID;
									//objectTrackTemp.RefEncoderNum 	= encoderNumber;
									/*�ж����������͵Ķ���(���ڶ�λ����ĳ�����ٶε�ĳ��������������ȷ����ǰ����������)*/
									if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.ActionTriggerFlag == true)
									{
										objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue;
										objectTrackTemp.ActionType 		= TypeActionTriggerCamera;
										objectTrackTemp.OutputType 		= TypeOutputDigital;
										objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.DigitalOutput;									
										objectTrackTemp.ActionNumber	= Action_i;
										vTaskSuspendAll();		//�����ٽ�����ֹ�����
										ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
										xTaskResumeAll();		//�˳��ٽ���	
										continue;
									}
									else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.ActionTriggerFlag == true)
									{
										objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;
										objectTrackTemp.ActionType 		= TypeActionObjectTakeOver;
										objectTrackTemp.OutputType 		= NoOutput;
										objectTrackTemp.ActionNumber	= Action_i;
										vTaskSuspendAll();		//�����ٽ�����ֹ�����
										ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
										xTaskResumeAll();		//�˳��ٽ���	
										continue;
									}
									else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.ActionTriggerFlag == true)
									{
										objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue;
										objectTrackTemp.ActionType 		= TypeActionSetOutput;
										objectTrackTemp.OutputType 		= TypeOutputDigital;
										objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.DigitalOutput;
										objectTrackTemp.ActionNumber	= Action_i;
										vTaskSuspendAll();		//�����ٽ�����ֹ�����
										ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
										xTaskResumeAll();		//�˳��ٽ���	
										continue;
									}
									else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.ActionTriggerFlag == true)
									{
										objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue;
										objectTrackTemp.ActionType 		= TypeActionTriggerSensor;
										objectTrackTemp.OutputType 		= TypeOutputDigital;
										objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.SensorID;
										objectTrackTemp.ActionNumber	= Action_i;
										vTaskSuspendAll();		//�����ٽ�����ֹ�����
										ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
										xTaskResumeAll();		//�˳��ٽ���	
										continue;
									}
									else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.ActionTriggerFlag == true)
									{
										objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue;
										objectTrackTemp.ActionType 		= TypeActionPushOut;
										objectTrackTemp.OutputType 		= TypeOutputResult;
										objectTrackTemp.OutputChannel = ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.DigitalOutput;
										objectTrackTemp.ActionNumber	= Action_i;//�˴�������Ĵ���ᱻ�������Ż�����֪��Ϊɶ��������
										vTaskSuspendAll();		//�����ٽ�����ֹ�����
										ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
										xTaskResumeAll();		//�˳��ٽ���	
										continue;
									}
									else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.ActionTriggerFlag == true)
									{
										objectTrackTemp.TargetValue 	= encoderNumber + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue;
										objectTrackTemp.ActionType 		= TypeActionRequestMachineData;
										objectTrackTemp.OutputType 		= NoOutput;
										objectTrackTemp.ActionNumber	= Action_i;
										vTaskSuspendAll();		//�����ٽ�����ֹ�����
										ListAppend(&ObjectInModuleList[Module_i], objectTrackTemp);			//���б��м��붯��
										xTaskResumeAll();		//�˳��ٽ���	
										continue;
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

