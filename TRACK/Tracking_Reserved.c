/**************************************************************************************************
*************************************�����߳�******************************************************
***************************************************************************************************/
#include "Tracking.h"
#include <math.h>

extern uint32_t Timer;			//ʹ������������ķ���Timer

//Timer: �������� 
//EncoderTimer������������
//#define CaptureNumber Timer;

__IO int64_t EncoderNumber = 0;     // ����������ֵ
__IO int64_t CaptureNumber = 0;     // ����������ֵ
__IO int64_t CaptureNumber2 = 0;		//������2����ֵ

TaskHandle_t TrackingThread_Handler;
static void TrackingThread(void *arg);

struct ObjectTracking ObjectBuffer[maxTrackingObjects];							//�������ֻ�ܱ���maxTrackingObjects�����������
byte Object_GlobalID = 0;

#define LightSignal_1 1
#define LightSignal_2 2

__IO int64_t LS1_EncoderNumTem = 0;				//���1��Ӧ����һ�α�����ֵ(�ݴ�����˫��������)
__IO int64_t LS2_EncoderNumTem = 0;				//���1��Ӧ����һ�α�����ֵ(�ݴ�����˫��������)

char* strCreateObject 	 				 	= "<<<<<<<<<<<<Detect someting Create Object!!!>>>>>>>>>>>>>>>>>";
char* strActionTriggerCamera 			= "<<<<<<<<<<<<Successfully Trigger Camera!!!>>>>>>>>>>>>>>>>>";
char* strActionTriggerSensor 			= "<<<<<<<<<<<<Successfully Trigger Sensor!!!>>>>>>>>>>>>>>>>>";
char* strActionPushOut 			 			= "<<<<<<<<<<<<Successfully Trigger Pusher!!!>>>>>>>>>>>>>>>>>";
char* strActionRequestMachineData = "<<<<<<<<<<<<Successfully RequestMachineData!!!>>>>>>>>>>>>>>>>>";
char* strActionSetOutput 					= "<<<<<<<<<<<<Successfully ActionSetOutput!!!>>>>>>>>>>>>>>>>>";
char* strActionObjectTakeOver 		= "<<<<<<<<<<<<Successfully ActionObjectTakeOver!!!>>>>>>>>>>>>>>>>>";

bool sendPacketTracking(struct ObjectTracking Object)
{
//	int* pInt;
//	byte* pByte;
//	short* pShort;
	return true;
}

bool SendPacket(byte clientID)
{
	return true;
}

static void CreateObject(byte objectCNT, byte moduleCNT)
{
	while(ObjectBuffer[objectCNT].objectAliveFlag != false)		//��ֹ���Ǹ�����δ�����Ķ���
	{
		objectCNT++;
		if(objectCNT > 64) objectCNT = 0;
	}
	//�����󻺳��������������
	ObjectBuffer[objectCNT].ClientID = 0;											//������ֵ��
	ObjectBuffer[objectCNT].ObjectID = objectCNT;							//�����ã���ʱ��ѭ��ͬ��
	ObjectBuffer[objectCNT].CreateObjectFlag = true;					//������ֵ���Ƿ񴴽�������ʱ����Ϊ����
	ObjectBuffer[objectCNT].BornEncoderNum = CaptureNumber;		//�ö������ʼ������ֵ(����ֵ)
	ObjectBuffer[objectCNT].ModuleReferenceEncoderNum[moduleCNT] = CaptureNumber;	//�������ڵĸ��ٶβο�������ֵ
	ObjectBuffer[objectCNT].CameraID = 0;											//0��ʾû�����
	ObjectBuffer[objectCNT].pData = strCreateObject;
	ObjectBuffer[objectCNT].ProcessedResult = true;						//������ֵ������������ʱ����Ϊ��
	ObjectBuffer[objectCNT].objectAliveFlag = true;						//������ٹ���
	
	ObjectBuffer[objectCNT].objectModulePosition = moduleCNT;	//�����Ӧ�ĸ��ٶΣ������ڵ�һ����磬�����һ�θ��ٶ�, ���ں����ж����Ƿ������ٶε��жϣ����Ե�һ���������򿪵�һ�����ٶ�
	
	ObjectBuffer[objectCNT].ModuleReferenceEncoderNum[0] = ObjectBuffer[objectCNT].BornEncoderNum;//��ʼ���ٶεı�����ֵΪ����������ֵ
	
	TCPSendDataBase(ObjectBuffer[objectCNT].ClientID, strCreateObject);		
	
	objectCNT++;
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
	byte Object_i = 0, Module_i = 0, Action_i = 0;			//�������Ʊ���
	bool exit_Module_Flag = false;											//����ģ���˳�ѭ��
	bool isObjectFullNew = true;												//�����жϹ���Ӧ������ȫ�µĶ����Ǵ������Ķ���

	uint32_t TimeCountStart = 0;												//���ڼ����߳�����ʱ��
	uint16_t timeCount = 0;
	 
	for(Object_i = 0; Object_i < maxTrackingObjects; Object_i++)										//��ʼ�����ж�������ж������������
	{
		/*�������Գ�ʼ��*/
		ObjectBuffer[Object_i].objectAliveFlag 			= false;													//��ʼ��������Ϊδ�������״̬
		ObjectBuffer[Object_i].objectDelieverdFlag 	= false;													//��ʼ������Ϊδ������״̬

		ObjectBuffer[Object_i].objectModulePosition			=	255;												//��Ƕ���Ŀǰ����������һ���ٶΣ�����Ϊȫ�µ�
		//ObjectBuffer[Object_i].delieverDestination	= 255;													//��ʼ���������贫�͵��ĸ��ٶ�
		
		for(Module_i = 0;Module_i <maxTrackingModule ;Module_i++)
		{
			for(Action_i = 0;Action_i < maxActionSingleModule; Action_i++)
			{
				ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] = false;
			}				
		}		
	}
	
	
	
	while(1)
	{	
		TimeCountStart = xTaskGetTickCount();
		
		if( ClientNum > 0)															//�м�����Inspection����
		{		
			/***���1***/			
			if(TIM2CH1_CAPTURE_STA & 0X80)        				//�ɹ�������һ�θߵ�ƽ		���1�˴����ɶ�ʱ�ж���ȷ���ɼ������źų��ȵģ��ǳ���׼��
			{
				printf("Catch the Light111 Signals\n");
				temp=TIM2CH1_CAPTURE_STA&0X3F;							//�������
				temp*=0XFFFFFFFF;		 	    									//���ʱ���ܺ�  ******Ӧ�ÿ���ȥ��
				temp+=TIM2CH1_CAPTURE_VAL;      						//�õ��ܵĸߵ�ƽʱ��	
				printf("HIGH:%lld us\r\n",temp);						//��ӡ�ܵĸߵ�ƽʱ�䡣
				
				CaptureNumber 	= (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);			//��¼�����Ķ���ĳ������������ֵ
				//CaptureNumber2 	= (OverflowCount_Encoder8*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder8);		//��¼�����Ķ���ĳ������������ֵ
				
				printf("Light111 BornSite Encoder Count==>%lld\n", CaptureNumber);
				
				isObjectFullNew = true;//��ʼ��Ĭ��Ϊ��һ��ȫ�µĶ���Ȼ������������ٴ��ж��ǲ���ȫ�µĶ���
				
				for(Module_i = 0;Module_i < Module_Count;Module_i++)								//�ҹ�������ĸ��ٶ�
				{
					if(LightSignal_1 == ModuleConfig[Module_i].ModuleTrigger && 			//Case1:�ҵ��˹�����ڵĸ��ٶ�λ�ã��й�磩�����Ҵ˸��ٶ�����������
						 ModuleConfig[Module_i].CreateObjectFlag == true)
					{
						if(CaptureNumber > ModuleConfig[Module_i].DoubleTrigger)				//��������ʱ����CaptureNumber = LS1_EncoderNumTem=0������޷��������������
						{
							if(temp > ModuleConfig[Module_i].Debounce &&
								(CaptureNumber - LS1_EncoderNumTem) >  ModuleConfig[Module_i].DoubleTrigger)						//�źŷ�����˫��������
							{
								for(Object_i = 0; Object_i< maxTrackingObjects;Object_i++)															//�жϻ����������еĶ������ĸ����
								{
									if(ObjectBuffer[Object_i].objectDelieverdFlag  == true &&
										 ObjectBuffer[Object_i].objectModulePosition == Module_i)	//���󱻼���&&���󱻴�����&&�����˴˸��ٶΣ��Ż���и��ٴ����жϣ�ȷ����һ�����ݹ����Ķ���
									{	////////////////////���CaptureNumber�����ʱ�����ǰ��ľͻ����һ������ĺܴ��ֵ!!!!!�ô���������������������������					
										if(CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ObjectBuffer[Object_i].objectModulePosition] < 
											 ModuleConfig[Module_i].TrackingWindow )	//��ָ���ĸ��ٴ��ڵ����Ϊ��ͬһ��ƿ�ӣ�����������
										{
											
											CaptureNumber2 = CaptureNumber;
										
											ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] = CaptureNumber2 ;//�趨��ǰ�εı�������ʹ�ú�α��������ο�ֵ
											
											//ObjectBuffer[Object_i].objectModulePosition = Module_i;//���¶��������ĸ��ٶ�λ��
										}
										else	//���δ��ָ���ĸ��ٴ����ڵ�������Ϊ��һ����;�ӽ�ȥ��ƿ�ӣ�Ҳ�贴��һ������
										{
											CreateObject(Object_GlobalID, Module_i);											//����һ���µĶ���(����Ϊȫ�µģ�Ҳ�����ǰ�;����һ��ƿ�ӽ�ȥ)
										
											LS1_EncoderNumTem = CaptureNumber;														//���汾�δ���ʱ�̵ı�����ֵ����˫��������
											
											Object_GlobalID++;
											if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;
													
										}
										
										isObjectFullNew = false;																					//��ǹ��������ⲻ��һ����ȫȫ�µĶ���
										ObjectBuffer[Object_i].objectDelieverdFlag = false;								//��ǰ����Ĵ��ݶ���������ϣ����Ϊδ������״̬
										break;																														//������ϣ������ڼ�������Ķ���
									}								
								}														
								if(isObjectFullNew == true)			//�жϴ˴ι��������ǲ�����ȫȫ�µ��źţ���ȫȫ�µ�δ�������Ķ��󣺼���һ�����ٶβ����Ķ���
								{
									CreateObject(Object_GlobalID, Module_i);																								//������Ӧ���ٶεĶ��󣨶���ID, ���ٶ�ID��
									
									
									LS1_EncoderNumTem = CaptureNumber;														//���汾�δ���ʱ�̵ı�����ֵ����˫��������
									Object_GlobalID++;
									if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;						
								}							
							}
						}
						else if(isObjectFullNew == true)			//(����ϵͳ������һ�δ�����)�жϴ˴ι��������ǲ�����ȫȫ�µ��źţ���ȫȫ�µ�δ�������Ķ��󣺼���һ�����ٶβ����Ķ���
						{
							CreateObject(Object_GlobalID, Module_i);																								//������Ӧ���ٶεĶ��󣨶���ID, ���ٶ�ID��
							
							
							LS1_EncoderNumTem = CaptureNumber;														//���汾�δ���ʱ�̵ı�����ֵ����˫��������
							Object_GlobalID++;
							if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;						
						}
						
						break;	//�ڸ��ٶ��ҵ��˹�粢ִ������Ӧ�Ķ������������λ������ѭ��������ѯ��һ����
					}
					
					else if(LightSignal_1 == ModuleConfig[Module_i].ModuleTrigger && 			//Case2:�ҵ��˹�����ڵĸ��ٶ�λ�ã��й�磩�����Ҵ˸��ٶβ�����������
									ModuleConfig[Module_i].CreateObjectFlag != true)
					{
						if(temp > ModuleConfig[Module_i].Debounce &&
							(CaptureNumber - LS1_EncoderNumTem) >  ModuleConfig[Module_i].DoubleTrigger)						//�źŷ�����˫��������
						{
							for(Object_i = 0; Object_i< maxTrackingObjects;Object_i++)															//�жϻ����������еĶ������ĸ����
							{
								if(ObjectBuffer[Object_i].objectDelieverdFlag 	== true &&
									 ObjectBuffer[Object_i].objectModulePosition	== Module_i)	//���󱻼���&&���󱻴�����&&�����˴˸��ٶΣ��Ż���и��ٴ����жϣ�ȷ����һ�����ݹ����Ķ���
								{								
									if(CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ObjectBuffer[Object_i].objectModulePosition] < 
										 ModuleConfig[Module_i].TrackingWindow )	//��ָ���ĸ��ٴ��ڵ����Ϊ��ͬһ��ƿ�ӣ�����������
									{
										
										CaptureNumber2 = CaptureNumber;
									
										ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] = CaptureNumber2 ;//�趨��ǰ�εı�������ʹ�ú�α��������ο�ֵ
										
										//ObjectBuffer[Object_i].objectModulePosition = Module_i;//���¶��������ĸ��ٶ�λ��
									}								
									
									isObjectFullNew = false;																					//��ǹ��������ⲻ��һ����ȫȫ�µĶ���
									ObjectBuffer[Object_i].objectDelieverdFlag = false;								//��ǰ����Ĵ��ݶ���������ϣ����Ϊδ������״̬
									break;			
								}								
							}														
						}	
						break;	//�ڸ��ٶ��ҵ��˹�粢ִ������Ӧ�Ķ������������λ������ѭ��������ѯ��һ����
					}
				//Case3:�޹�粻�������������ﴦ��������ĸ��ٹ��̼����
				}
				
				TIM2CH1_CAPTURE_STA=0;          								//���1������һ�β���			
			}			
			
			
			
			/***���2***/	
			if(TIM5CH1_CAPTURE_STA & 0X80)        					//�ɹ�������һ�θߵ�ƽ		���2�˴����ɶ�ʱ�ж���ȷ���ɼ������źų��ȵģ��ǳ���׼��
			{
				printf("Catch the Light222 Signals\n");
				temp = TIM5CH1_CAPTURE_STA & 0X3F;							//�������
				temp*= 0XFFFFFFFF;		 	    										//���ʱ���ܺ�  ******Ӧ�ÿ���ȥ��
				temp+= TIM5CH1_CAPTURE_VAL;      								//�õ��ܵĸߵ�ƽʱ��	
				printf("HIGH:%lld us\r\n",temp);								//��ӡ�ܵĸߵ�ƽʱ��
				
				
				CaptureNumber 	= (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);			//��¼�����Ķ���ĳ������������ֵ
				//CaptureNumber2 	= (OverflowCount_Encoder8*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder8);		//��¼�����Ķ���ĳ������������ֵ
				
				printf("Light222 BornSite Encoder Count==>%lld\n", CaptureNumber);
				
				isObjectFullNew = true;//��ʼ��Ĭ��Ϊ��һ��ȫ�µĶ���Ȼ������������ٴ��ж��ǲ���ȫ�µĶ���
				
				for(Module_i = 0;Module_i < Module_Count;Module_i++)								//�ҹ�������ĸ��ٶ�
				{
					if(LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger && 			//Case1:�ҵ��˹�����ڵĸ��ٶ�λ�ã��й�磩�����Ҵ˸��ٶ�����������
						 ModuleConfig[Module_i].CreateObjectFlag == true)
					{
						if(CaptureNumber > ModuleConfig[Module_i].DoubleTrigger)
						{
							if(temp > ModuleConfig[Module_i].Debounce &&
								(CaptureNumber - LS2_EncoderNumTem) >  ModuleConfig[Module_i].DoubleTrigger)						//�źŷ�����˫��������
							{
								for(Object_i = 0; Object_i< maxTrackingObjects;Object_i++)															//�жϻ����������еĶ������ĸ����
								{
									if(ObjectBuffer[Object_i].objectDelieverdFlag  == true &&
										 ObjectBuffer[Object_i].objectModulePosition == Module_i)	//���󱻼���&&���󱻴�����&&�����˴˸��ٶΣ��Ż���и��ٴ����жϣ�ȷ����һ�����ݹ����Ķ���
									{	////////////////////���CaptureNumber�����ʱ�����ǰ��ľͻ����һ������ĺܴ��ֵ!!!!!�ô���������������������������					
										if(CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ObjectBuffer[Object_i].objectModulePosition] < 
											 ModuleConfig[Module_i].TrackingWindow )	//��ָ���ĸ��ٴ��ڵ����Ϊ��ͬһ��ƿ�ӣ�����������
										{
											
											CaptureNumber2 = CaptureNumber;
										
											ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] = CaptureNumber2 ;//�趨��ǰ�εı�������ʹ�ú�α��������ο�ֵ
											
											//ObjectBuffer[Object_i].objectModulePosition = Module_i;//���¶��������ĸ��ٶ�λ��
										}
										else	//���δ��ָ���ĸ��ٴ����ڵ�������Ϊ��һ����;�ӽ�ȥ��ƿ�ӣ�Ҳ�贴��һ������
										{
											CreateObject(Object_GlobalID, Module_i);											//����һ���µĶ���(����Ϊȫ�µģ�Ҳ�����ǰ�;����һ��ƿ�ӽ�ȥ)
										
											LS2_EncoderNumTem = CaptureNumber;														//���汾�δ���ʱ�̵ı�����ֵ����˫��������
											
											Object_GlobalID++;
											if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;
													
										}
										
										isObjectFullNew = false;																					//��ǹ��������ⲻ��һ����ȫȫ�µĶ���
										ObjectBuffer[Object_i].objectDelieverdFlag = false;								//��ǰ����Ĵ��ݶ���������ϣ����Ϊδ������״̬
										break;																														//������ϣ������ڼ�������Ķ���
									}								
								}														
								if(isObjectFullNew == true)			//�жϴ˴ι��������ǲ�����ȫȫ�µ��źţ���ȫȫ�µ�δ�������Ķ��󣺼���һ�����ٶβ����Ķ���
								{
									CreateObject(Object_GlobalID, Module_i);																								//������Ӧ���ٶεĶ��󣨶���ID, ���ٶ�ID��
									
									
									LS2_EncoderNumTem = CaptureNumber;														//���汾�δ���ʱ�̵ı�����ֵ����˫��������
									Object_GlobalID++;
									if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;						
								}							
							}
						}
						else if(isObjectFullNew == true)			//(����ϵͳ������һ�δ�����)�жϴ˴ι��������ǲ�����ȫȫ�µ��źţ���ȫȫ�µ�δ�������Ķ��󣺼���һ�����ٶβ����Ķ���
						{
							CreateObject(Object_GlobalID, Module_i);																								//������Ӧ���ٶεĶ��󣨶���ID, ���ٶ�ID��
							
							
							LS2_EncoderNumTem = CaptureNumber;														//���汾�δ���ʱ�̵ı�����ֵ����˫��������
							Object_GlobalID++;
							if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;						
						}
						
						break;	//�ڸ��ٶ��ҵ��˹�粢ִ������Ӧ�Ķ������������λ������ѭ��������ѯ��һ����
					}
					
					else if(LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger && 			//Case2:�ҵ��˹�����ڵĸ��ٶ�λ�ã��й�磩�����Ҵ˸��ٶβ�����������
									ModuleConfig[Module_i].CreateObjectFlag != true)
					{
						if(temp > ModuleConfig[Module_i].Debounce &&
							(CaptureNumber - LS2_EncoderNumTem) >  ModuleConfig[Module_i].DoubleTrigger)						//�źŷ�����˫��������
						{
							for(Object_i = 0; Object_i< maxTrackingObjects;Object_i++)															//�жϻ����������еĶ������ĸ����
							{
								if(ObjectBuffer[Object_i].objectDelieverdFlag 	== true &&
									 ObjectBuffer[Object_i].objectModulePosition	== Module_i)	//���󱻼���&&���󱻴�����&&�����˴˸��ٶΣ��Ż���и��ٴ����жϣ�ȷ����һ�����ݹ����Ķ���
								{								
									if(CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ObjectBuffer[Object_i].objectModulePosition] < 
										 ModuleConfig[Module_i].TrackingWindow )	//��ָ���ĸ��ٴ��ڵ����Ϊ��ͬһ��ƿ�ӣ�����������
									{
										
										CaptureNumber2 = CaptureNumber;
									
										ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] = CaptureNumber2 ;//�趨��ǰ�εı�������ʹ�ú�α��������ο�ֵ
										
										//ObjectBuffer[Object_i].objectModulePosition = Module_i;//���¶��������ĸ��ٶ�λ��
									}								
									
									isObjectFullNew = false;																					//��ǹ��������ⲻ��һ����ȫȫ�µĶ���
									ObjectBuffer[Object_i].objectDelieverdFlag = false;								//��ǰ����Ĵ��ݶ���������ϣ����Ϊδ������״̬
									break;			
								}								
							}														
						}	
						break;	//�ڸ��ٶ��ҵ��˹�粢ִ������Ӧ�Ķ������������λ������ѭ��������ѯ��һ����
					}
				//Case3:�޹�粻�������������ﴦ��������ĸ��ٹ��̼����
				}				
			TIM5CH1_CAPTURE_STA=0;          									//���2������һ�β���			
		 }								


			
/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����*********************************************************************************				
*************************************************************************************************************************************************************/				
			for(Object_i = 0; Object_i< maxTrackingObjects;Object_i++)										//ά�����������������ж���
			{	
				if(ObjectBuffer[Object_i].objectAliveFlag == true)													//���󱻼����˲Ż������ٹ���
				{								
					for(Module_i = 0;Module_i < Module_Count;Module_i++)
					{	
						if(ObjectBuffer[Object_i].objectModulePosition == Module_i)							//����ֻ�ܽ������������ڵĸ��ٶ�
						{							
							for(Action_i = 0;Action_i < maxActionSingleModule; Action_i++)				//����Module���Ķ���������һ��һ��ȥɨ��(����δ֪�����������������ô������������ÿ��Ե��ö�������NumberOfActions)
							{
								if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.ActionTriggerFlag == true)			//�ĸ�����,�Լ��ڸ��ٶ��ϵ�TargetValue
								{
									CaptureNumber = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
									
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue) >0 )//˵������ObjectBuffer[Object_i]����ModuleConfig[Module_i].ActionInstanceConfig[Action_i]���������λ��
									{
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)					//�ж��Ƿ��ظ���������
										{
											printf("Item_ActionObjectTakeOver ObjectBuffer==%d , Module==%d, ActionNum==%d, CaptureNumber==>%llu, \n", Object_i, Module_i, Action_i, CaptureNumber);
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionObjectTakeOver);
											
											//��Ǹ��ٶ���Object_i������һ�ζ�������......................,Ҳ������������ǰ���ٶεĶ���X���ݳ�ȥ�ˡ�����
											ObjectBuffer[Object_i].objectDelieverdFlag 		= true;
											ObjectBuffer[Object_i].objectModulePosition 	= ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule;//�޸Ķ������ڵĸ��ٶ�λ��
											
											if((ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].ModuleTrigger == 0) && 
												(ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].CreateObjectFlag == false))		//���贫�ݵ��ĸ��ٶ���û�й��Ͳ���������,������������ɹ��ȥ̽��
											{
												//���ݶ�������贫�͵ĸ��ٶ�,�޸Ĵ���Ŀ�ĸ��ٶεĲο�������ֵ = ���ͶεĲο�������ֵ + ���ݶ�����Ŀ��ֵ 
												ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule] = 
																		ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;		
											
											}
/***������������ɹ��ȥ����										
											else if((ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].ModuleTrigger > 0) && 
												(ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].CreateObjectFlag == false))		//���贫�ݵ��ĸ��ٶ��й�絫���������󣬲��򿪶�Ӧ���ٶΣ��ɹ��ȥ�򿪶�Ӧ���ٶ�
											{																								
												//���ݶ�������贫�͵ĸ��ٶ�,�޸Ĵ���Ŀ�ĸ��ٶεĲο�������ֵ = ���ͶεĲο�������ֵ + ���ݶ�����Ŀ��ֵ 
												ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule] = 
															ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;	
												
												
												
											}
											else if((ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].ModuleTrigger > 0) && 
												(ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].CreateObjectFlag == true))		//���贫�ݵ��ĸ��ٶ��й����Ҫ�������󣬲��򿪶�Ӧ���ٶΣ��ɹ��ȥ�򿪶�Ӧ���ٶ�
											{																								
												//���ݶ�������贫�͵ĸ��ٶ�,�޸Ĵ���Ŀ�ĸ��ٶεĲο�������ֵ = ���ͶεĲο�������ֵ + ���ݶ�����Ŀ��ֵ 
												ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule] = 
															ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;		
												
											}
***/											
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;
											
											exit_Module_Flag = true;				//�������ٶ�ѭ��==>�ѵ�����Ӧ�Ķ���						
											break;													//����������ѯѭ��.
										}
									}
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.ActionTriggerFlag == true)
								{
									CaptureNumber = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
									//˵������ObjectBuffer[Object_i]����ModuleConfig[Module_i].ActionInstanceConfig[Action_i]���������λ��
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue) > 0)		
									{
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)						//�˶���δ��������
										{
											printf("Item_ActionPushOut Module==%d, ActionNum==%d, ObjectBuffer==%d , CaptureNumber==>%llu, \n",Module_i, Action_i, Object_i,CaptureNumber);											
											xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerPush, eSetValueWithOverwrite, NULL);	//����֪ͨȥ�����޳�
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionPushOut);
											
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;														
											
											/*************
											******************************���ٶ����ʱ��һ��Ҫ����������ȫ���ָ���ʼ״̬��������������������
											*****************************************************************************************************/
											ObjectBuffer[Object_i].objectAliveFlag = false;				//Ĭ���޳���Ϊ�����������ڵĽ����㣡������������������������������������������
											ObjectBuffer[Object_i].objectModulePosition  = 255;		//��Ƕ��������κ�һ�����ٶ�
											ObjectBuffer[Object_i].objectDelieverdFlag 	= false;
											
											for(Module_i = 0;Module_i <Module_Count ;Module_i++)	//���ö�������ж���ִ��״̬Ϊfalse������Դ������������
											{											
												for(Action_i = 0;Action_i < maxActionSingleModule; Action_i++)
												{
													ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] = false;
												}				
											}	
											
											exit_Module_Flag = true;
											break;
										}
									}									
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.ActionTriggerFlag == true)
								{
									CaptureNumber = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
									
									//˵������ObjectBuffer[Object_i]����ModuleConfig[Module_i].ActionInstanceConfig[Action_i]���������λ��
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionRequestMachineData.TargetValue) > 0)
									{
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)
										{
											printf("Item_ActionRequestMachineData Module==%d, ActionNum==%d, ObjectBuffer==%d , CaptureNumber==>%llu, \n",Module_i, Action_i, Object_i,CaptureNumber);
											
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionRequestMachineData);
											
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;										
											
											exit_Module_Flag = true;
											break;	
										}										
									}
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.ActionTriggerFlag == true)
								{
									CaptureNumber = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
									
									//˵������ObjectBuffer[Object_i]����ModuleConfig[Module_i].ActionInstanceConfig[Action_i]���������λ��
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue) > 0)		
									{	
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)
										{						
											printf("Item_ActionSetOutput Module==%d, ActionNum==%d, ObjectBuffer==%d , CaptureNumber==>%llu, \n",Module_i, Action_i, Object_i,CaptureNumber);
											xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerOutput, eSetValueWithOverwrite, NULL);	//����֪ͨȥ�������
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionSetOutput);
											
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;
											
											exit_Module_Flag = true;
											break;		//����������ѯѭ��	
										}										
									}
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.ActionTriggerFlag == true)
								{			
									CaptureNumber = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
									
									//˵������ObjectBuffer[Object_i]����ModuleConfig[Module_i].ActionInstanceConfig[Action_i]���������λ��
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue) > 0)		
									{	
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)
										{						
											printf("Item_ActionTriggerCamera Module==%d, ActionNum==%d, ObjectBuffer==%d , CaptureNumber==>%llu, \n",Module_i, Action_i, Object_i,CaptureNumber);
											xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerCamera, eSetValueWithOverwrite, NULL);	//����֪ͨȥ�������
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionTriggerCamera);
																					
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;
		
											exit_Module_Flag = true;
											break;		//����������ѯѭ��
										}
									}
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.ActionTriggerFlag == true)
								{					
									CaptureNumber = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
									
									//˵������ObjectBuffer[Object_i]����ModuleConfig[Module_i].ActionInstanceConfig[Action_i]���������λ��
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue) > 0)		
									{
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)
										{
											printf("Item_ActionTriggerSensor Module==%d, ActionNum==%d, ObjectBuffer==%d , CaptureNumber==>%llu, \n",Module_i, Action_i, Object_i,CaptureNumber);
											xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerSensor, eSetValueWithOverwrite, NULL);	//����֪ͨȥ�����������ɼ�
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionTriggerSensor);
																					
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;
		
											exit_Module_Flag = true;
											break;		//����������ѯѭ��
										}
									}
								}
							}							
						}

						
					if(exit_Module_Flag == true)									//�жϵ�ǰ���ٶ��Ƿ��ҵ��˶������ҵ��˾��˳���û�ҵ��ͼ�����
						{
							exit_Module_Flag = false;
							break;
						}						
					}
				}
			}
/************************************************************************************************************************************************************			
*****************************************************************���ٹ��̼����END*********************************************************************************				
*************************************************************************************************************************************************************/		
			
//		if(Timer > 3020) Timer = 0;		//�ظ���������
		}
		timeCount = xTaskGetTickCount()- TimeCountStart;
		if(timeCount > 0)
		{
			printf("Tracking Thread Running Time ==>%d \n", timeCount);//�״�28ms��һ�Σ�����18ms��һ��
		}

	vTaskDelay(1);																								//������1ms�Ļ�����һֱ���������
	}	
}

