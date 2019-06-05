/**************************************************************************************************
*************************************跟踪线程******************************************************
***************************************************************************************************/
#include "Tracking.h"
#include <math.h>
#include "TCPProtocol.h"
#include "timer.h"
#include "malloc.h"

extern uint32_t Timer;			//使用主函数定义的仿真Timer

__IO	uint32_t GlobalObjectID;											//定义一个全局对象ID，__IO表示直接从地址处取值，取得最新值，允许所有软件硬件修改此值
QueueHandle_t	ModuleQueue[maxTrackingModule];				//创建信息队列用于接收信息
__IO ObjectInfo ObjectBuffer[maxTrackingObjects];	  //创建最大只能保存maxTrackingObjects个对象的数组 
									
StctActionListItem ObjectInModuleList[maxTrackingModule][maxTrackingObjects];//二维数组用于存储所有动作列表

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
	while(ObjectBuffer[objectCNT].objectAliveFlag != false)		//防止覆盖跟踪尚未结束的对象
	{
		objectCNT++;
		if(objectCNT > maxTrackingObjects - 1) 
		{
			objectCNT = 0;
			printf("Buffer is full Please check wether need to enlarge the Object Buffer!. \n");
			break;
		}
	}
	//往对象缓冲数组里填充数据
	ObjectBuffer[objectCNT].ClientID = 0;								//待传入值，
	ObjectBuffer[objectCNT].ObjectID = GlobalObjectID;					//待设置，暂时与循环同步
//	ObjectBuffer[objectCNT].BornEncoderNum = CaptureNumber;		//该对象的起始编码器值(出生值)
	ObjectBuffer[objectCNT].pData 	 = "TestData";
	ObjectBuffer[objectCNT].ProcessedResult = true;							//待传入值，处理结果，暂时设置为好
	ObjectBuffer[objectCNT].objectAliveFlag = true;							//激活跟踪过程
	
	GlobalObjectID++;

	return objectCNT;//返回创建的对象在缓冲数组中的位置
}

static void AddActionToList(byte i, int objectID, int targetValue, enum enumActionType actionType, byte actionNumber, byte outputType, byte outputChannel)
{
	int j =0;

	for(j = 0; j<maxTrackingObjects; j++)
	{
		if(!ObjectInModuleList[i][j].IsActionAlive)		//判断此动作是否Alive，是则不能覆盖，否则可以覆盖
		{
			ObjectInModuleList[i][j].ObjectID				= objectID;
			ObjectInModuleList[i][j].TargetValue		=	targetValue;
			ObjectInModuleList[i][j].ActionType			= actionType;
			ObjectInModuleList[i][j].ActionNumber 	= actionNumber;			//单个Module 里的动作编号，是parametersLoad内需要的。
			ObjectInModuleList[i][j].OutputType			=	outputType;
			ObjectInModuleList[i][j].OutputChannel	=	outputChannel;
			ObjectInModuleList[i][j].IsActionAlive	=	true;						//标记此动作已加入列表，占据了一个位置
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
		/*判断是哪种类型的动作(由于定位到了某个跟踪段的某个动作，所以是确定当前动作的种类)*/
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


//创建跟踪任务
//返回值:0 跟踪任务创建成功
//		其他 跟踪任务创建失败
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

//Tracking跟踪线程
static void TrackingThread(void *arg)
{	
	long long temp = 0;
	byte GlobalObjectCount 	= 0;
//	uint16_t timeCount 			= 0;
	static bool AddActiveFlag 		= false;	
	static StctActionListItem objectTrackTemp ;				//跟踪数据暂存值
//	static ModuleQueueItem* 	moduleQueueTemp;					//定义一个从队列中暂取数据的暂存值指针
//	Packet* pPacket = mymalloc(SRAMEX, 256);					//分配一个暂存区
	
//	static __IO int64_t encoderNumber 		= 0;     		//编码器计数值
//	static __IO int64_t encoder1Number 		= 0;				//编码器1计数值
//	static __IO int64_t encoder2Number 		= 0;				//编码器2计数值
	
	static short triggerInterval 					= 0;				//两次触发间隔编码器数
	static __IO int64_t LS1_EncoderNumTem = 0;				//光电1对应的上一次编码器值(暂存用于双触发保护)
	static __IO int64_t LS2_EncoderNumTem = 0;				//光电2对应的上一次编码器值(暂存用于双触发保护)
	static __IO int64_t LS3_EncoderNumTem = 0;				//光电3对应的上一次编码器值(暂存用于双触发保护)
	static __IO int64_t LS4_EncoderNumTem = 0;				//光电4对应的上一次编码器值(暂存用于双触发保护)
	
	
	
	byte Module_i = 0, Action_i = 0;												//遍历控制变量
	byte Object_i = 0, i_cycle 	= 0;
	int data_len = 0;
	static __IO int64_t encoderNumber 		= 0;     					// 编码器计数值
	static __IO int64_t encoder1Number 		= 0;
	static __IO int64_t encoder2Number 		= 0;
	static __IO int64_t encoderDelivered 	= 0;	
//	static bool isActionIn = false;
	static ModuleQueueItem* moduleQueueTemp;						//定义一个往队列中填充数据的暂放指针
	static Packet* pPacket;															//数据包首地址
	uint32_t TimeCountStart = 0;												//用于计算线程运行时间
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

	moduleQueueTemp = mymalloc(SRAMEX, sizeof(ModuleQueueItem));//分配内存初始化
	pPacket 				= mymalloc(SRAMEX, 256);										//为数据包分配一个固定的256字节的数据临时存储区
	
	
	for(Object_i = 0; Object_i < maxTrackingObjects; Object_i++)							
	{
		/*对象属性初始化*/
		ObjectBuffer[Object_i].objectAliveFlag 	= false;				//初始化对象标记为未激活跟踪状态
	}
	
	for(Module_i = 0; Module_i < maxTrackingModule; Module_i++)							
	{
		/*对象属性初始化*/
		for(Action_i = 0;Action_i	<	maxTrackingObjects;Action_i++)
			ObjectInModuleList[Object_i][Action_i].IsActionAlive 	= false;				//初始化跟踪段动作列表中的动作为未激活状态
	}
	
	//创建队列：
	for(Module_i = 0;Module_i < maxTrackingModule;Module_i++)
	{
		ModuleQueue[Module_i] = xQueueCreate(moduleQueueDepth, sizeof(ModuleQueueItem));
		
	}
	moduleQueueTemp = mymalloc(SRAMEX, sizeof(ModuleQueueItem));//初始化指针
	
	
/************************************************************************************************************************************************************			
*****************************************************************跟踪过程监控区******************************************************************************				
*************************************************************************************************************************************************************/	
	while(1)
	{
		TimeCountStart 	= OverflowCount_TIM6*0XFFFF +__HAL_TIM_GET_COUNTER(&TIM6_Handler);//0.7us
		TimeCountStartRTOS = xTaskGetTickCount();
		
		if( ClientNum > 0)															//有检测程序Inspection连接
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

				if(xQueueReceive(ModuleQueue[Module_i], moduleQueueTemp, 0) == pdTRUE)	//队列中有数据，有动作传递，值传递过来了
				{
					/*******传递动作的检测实现****/
					if(ModuleConfig[Module_i].ModuleTrigger > 0)//有光电
					{
						/****光电1***/					
						if((LightSignal_1 == ModuleConfig[Module_i].ModuleTrigger) && (TIM2CH1_CAPTURE_STA & 0X80))
						{
////							printf("==>Module[%d] Catch the Light111 Signals\n", Module_i);
////							temp=TIM2CH1_CAPTURE_STA&0X3F;							//溢出次数
////							temp*=0XFFFFFFFF;		 	    									//溢出时间总和  ******应该可以去掉
////							temp+=TIM2CH1_CAPTURE_VAL;      						//得到总的高电平时间	
////							printf("HIGH:%lld us\r\n",temp);						//打印总的高电平时间。
////							printf("Using Encoder[%d], EncoderNumber is [%lld]\n", ModuleConfig[Module_i].Encoder, encoderNumber);
////							
							TIM2CH1_CAPTURE_STA = 0;          								//光电1开启下一次捕获		
							triggerInterval 		= encoderNumber - LS1_EncoderNumTem;
							LS1_EncoderNumTem 	= encoderNumber;					//保存光电上一次编码器的触发值
							AddActiveFlag 			= true;
						}						
						/****光电2***/					
						else if((LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger) && (TIM5CH1_CAPTURE_STA & 0X80))
						{
////							printf("==>Module[%d] Catch the Light222 \n", Module_i);
////							temp=TIM5CH1_CAPTURE_STA&0X3F;							//溢出次数
////							temp*=0XFFFFFFFF;		 	    									//溢出时间总和  ******应该可以去掉
////							temp+=TIM5CH1_CAPTURE_VAL;      						//得到总的高电平时间	
////							printf("HIGH:%lld us\r\n",temp);						//打印总的高电平时间。
////							printf("Using Encoder[%d], EncoderNumber is [%lld]\n", ModuleConfig[Module_i].Encoder, encoderNumber);
////							
							TIM5CH1_CAPTURE_STA = 0;          								//光电1开启下一次捕获		
							triggerInterval 		= encoderNumber - LS2_EncoderNumTem;
							LS2_EncoderNumTem 	= encoderNumber;					//保存光电上一次编码器的触发值
							AddActiveFlag 			= true;
						}
						if(AddActiveFlag == true)
						{
							AddActiveFlag = false;
							if(triggerInterval >  ModuleConfig[Module_i].Debounce	&&
							   triggerInterval > ModuleConfig[Module_i].DoubleTrigger)	//信号防抖和双触发保护
							{
								if(ModuleConfig[Module_i].CreateObjectFlag == true)		//此跟踪段允许创建对象
								{	
									//系统初次启动编码器为0，可能无法创建兑现得转过一定编码器才会开始创建对象...
									if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//为传递过来的对象，仅将对象在此段的动作加入列表
									{
										pPacket = CreateObjectRunInPacket(pPacket, 1, moduleQueueTemp->DelieverdObjectID, Module_i, encoderNumber, 
																											0, encoderNumber, moduleQueueTemp->DelieverdEncoderNum);//告诉PC对象XXX进入了跟踪段XXX
										data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
										i_cycle  = 0;
										for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//往所有连接的客户端都发送对象到来的消息
										{
											WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
										}
									
										AddALLActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber);//往所在跟踪段列表添加动作						
									}
									else		//认为是一个全新的对象了，创建对象，并将目前对象所在跟踪段的动作加入对应的跟踪段列表（无论是否为第一个跟踪段都可以使用）
									{
										printf("Create A New Object, Delivered Object not comming in time. \n");
										if(GlobalObjectCount > maxTrackingObjects - 1) GlobalObjectCount = 0;//缓冲大小为64，Count必须小于64，否则赋值给CreateObject会出错
										GlobalObjectCount = CreateObject(GlobalObjectCount, Module_i, ModuleConfig[Module_i].Encoder, 1, encoderNumber, encoderNumber);
										GlobalObjectCount++;
											
										pPacket = CreateObjectRunInPacket(pPacket, 1, ObjectBuffer[GlobalObjectCount - 1].ObjectID, Module_i, encoderNumber, 
																											1, encoderNumber, moduleQueueTemp->DelieverdEncoderNum);//高速PC对象XXX进入了跟踪段XXX
										data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
										i_cycle  = 0;
										for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//往所有连接的客户端都发送对象到来的消息
										{
											WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
										}	
										
										AddALLActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber);//往所在跟踪段列表添加动作																								
									}												
								}
								else if(ModuleConfig[Module_i].CreateObjectFlag != true)//此跟踪段不允许创建对象
								{
									printf("Module[%d] not allowed to Create Object, Just Deliverd. \n", Module_i);
									//系统初次启动编码器为0，可能无法创建兑现得转过一定编码器才会开始创建对象...
									if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//为传递过来的对象，仅将对象在此段的动作加入列表
									{
										pPacket = CreateObjectRunInPacket(pPacket, 1,  moduleQueueTemp->DelieverdObjectID, Module_i, encoderNumber, 
																											0, encoderNumber, moduleQueueTemp->DelieverdEncoderNum);//高速PC对象XXX进入了跟踪段XXX
										data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
										i_cycle  = 0;
										for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//往所有连接的客户端都发送对象到来的消息
										{
											WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
										}
										
										AddALLActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber);//往所在跟踪段列表添加动作		
									}
								}
							}							
						}							
					}
					/****跟踪段无光电&&只进行对象的传递*****/
					else								
					{
printf("Module[%d] not have lightSensor, Just Deliverd. \n", Module_i);						
						//系统初次启动编码器为0，可能无法创建得先转过一定编码器才会开始创建对象...
						if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//为传递过来的对象，仅将对象在此段的动作加入列表
						{
							pPacket = CreateObjectRunInPacket(pPacket, 1,  moduleQueueTemp->DelieverdObjectID, Module_i, encoderNumber, 
																								0, encoderNumber, moduleQueueTemp->DelieverdEncoderNum);//高速PC对象XXX进入了跟踪段XXX动作为传递的动作
							data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
							i_cycle  = 0;
							for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//往所有连接的客户端都发送对象到来的消息
							{
								WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
							}
							
							AddALLActionToList(Module_i, moduleQueueTemp->DelieverdObjectID, encoderNumber);//往所在跟踪段列表添加动作		
						}
					}
				}
			
				
				else			//非传递而来待测物体对象的创建...消息队列中没有对象传递，但是光电仍感应到了信号
				{
					if(ModuleConfig[Module_i].ModuleTrigger > 0)	//当前跟踪段有光电
					{
						/*********光电1*********/
						if((LightSignal_1 == ModuleConfig[Module_i].ModuleTrigger) && (TIM2CH1_CAPTURE_STA & 0X80) )		//如果跟踪段使用光电1
						{
							if(ModuleConfig[Module_i].CreateObjectFlag == true)	//允许创建对象
							{
//								printf("==>Module[%d] Catch the Light111 Signals Not by Delivered Object Pretty New. \n", Module_i);
//								temp	= TIM2CH1_CAPTURE_STA&0X3F;							//溢出次数
//								temp *= 0XFFFFFFFF;		 	    									//溢出时间总和  ******应该可以去掉
//								temp += TIM2CH1_CAPTURE_VAL;      						//得到总的高电平时间	
//								printf("HIGH:%lld us\r\n",temp);						//打印总的高电平时间。
//								printf("Module[%d]==>Using Encoder[%d], EncoderNumber is [%lld]\n", Module_i, ModuleConfig[Module_i].Encoder, encoderNumber);
								TIM2CH1_CAPTURE_STA = 0;          								//光电1开启下一次捕获			
								triggerInterval 		= encoderNumber - LS1_EncoderNumTem;
								LS1_EncoderNumTem 	= encoderNumber;					//记录上次光电被触发的值
								AddActiveFlag 			= true;
							}	
						}
						else if((LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger) && (TIM5CH1_CAPTURE_STA & 0X80) )		//如果跟踪段使用光电2
						{						
							if(ModuleConfig[Module_i].CreateObjectFlag == true)	//跟踪段允许创建对象
							{
//								printf("==>Module[%d] Catch the Light222 Signals Signals Not by Delivered Object Pretty New. \n", Module_i);
//								temp=TIM5CH1_CAPTURE_STA&0X3F;							//溢出次数
//								temp*=0XFFFFFFFF;		 	    									//溢出时间总和  ******应该可以去掉
//								temp+=TIM5CH1_CAPTURE_VAL;      						//得到总的高电平时间	
//								printf("HIGH:%lld us\r\n",temp);						//打印总的高电平时间。
//								printf("Module[%d]==>Using Encoder[%d], EncoderNumber is [%lld]\n", Module_i, ModuleConfig[Module_i].Encoder, encoderNumber);

								TIM5CH1_CAPTURE_STA = 0;          								//光电2开启下一次捕获	
								triggerInterval 		= encoderNumber - LS2_EncoderNumTem;
								LS2_EncoderNumTem 	= encoderNumber;					//记录上次光电被触发的值
								AddActiveFlag 			= true;
							}
						}
							
						if(AddActiveFlag == true)
						{
							AddActiveFlag = false;
							
printf("Module[%d] not delvered, ModuleQueue empty ,Create Pretty New \n", Module_i);
							if(triggerInterval >  ModuleConfig[Module_i].Debounce	&&
							   triggerInterval > ModuleConfig[Module_i].DoubleTrigger)	//信号防抖和双触发保护
							{
								if(GlobalObjectCount > maxTrackingObjects - 1) GlobalObjectCount = 0;//缓冲大小为64，Count必须小于64，否则赋值给CreateObject会出错
								GlobalObjectCount = CreateObject(GlobalObjectCount, Module_i, ModuleConfig[Module_i].Encoder, 1, encoderNumber, encoderNumber);
								GlobalObjectCount++;
									
								pPacket = CreateObjectRunInPacket(pPacket, 1,  ObjectBuffer[GlobalObjectCount - 1].ObjectID, Module_i, encoderNumber, 
																									1,  encoderNumber, encoderNumber);//高速PC对象XXX进入了跟踪段XXX
								data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
								i_cycle  = 0;
								for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//往所有连接的客户端都发送对象到来的消息
								{
									WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
								}
								
								AddALLActionToList(Module_i, ObjectBuffer[GlobalObjectCount - 1].ObjectID, encoderNumber);//往所在跟踪段列表添加动作										
							}
						}						
					}
				}		
			}		
		
			
			
/************************************************************************************************************************************************************			
*****************************************************************动作执行-->扫描列表*************************************************************************
*************************************************************************************************************************************************************/		
			for(Module_i = 0;Module_i < Module_Count;Module_i++)
			{ 
				switch (ModuleConfig[Module_i].Encoder)
				{
					case Encoder_1: encoderNumber = encoder1Number;break;
					case Encoder_2: encoderNumber = encoder2Number;break;
				}	
				
				for(Action_i = 0; Action_i < maxTrackingObjects/2; Action_i++)//这里比较耗时maxTrackingObjects 64：1420us, 32: 
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
									//将要发送的数据传递到发送buffer
									
									//1. 耗时1022us(动态分配内存的)，2. 86us（传入pPacket的）
/*ConsumeTime:96usALL*/pPacket = CreateObjectRunOutPacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber, pTempObjectTakeOver->DestinationModule);

									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;//耗时24us
									i_cycle = 0;
									for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//往所有连接的客户端都发送对象到来的消息
									{
										WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
									}
								
/*ConsumeTime:550usALL*/ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;
									testFlag = true;
									break;
								
								case ActTriggerCamera			: 
printf("Get into ActTriggerCamera\r\n");										
/*ConsumeTime:32us*/pTempTriggerCamera = (propActionTriggerCamera*)ModuleConfig[Module_i].ActionInstanceConfig[ObjectInModuleList[Module_i][Action_i].ActionNumber].pActionConfig;

/*ConsumeTime:720us*/xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerCamera, eSetValueWithOverwrite, NULL);	//发送通知去触发相机

/*ConsumeTime:100us*/pPacket = CreateTriggerCameraPacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber,
																											pTempTriggerCamera->CameraID, 1);

									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4; /*ConsumeTime:160us*///整个包含WriteDataToBufferSend耗时160us
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
									//此Packet有问题，不知有何问题
									pPacket = CreateObjectDeletePacket(pPacket, 1, ObjectInModuleList[Module_i][Action_i].ObjectID, Module_i, encoderNumber);				
								
									data_len = PACKET_HEADER_SIZE + pPacket->DataSize + 4;
									i_cycle  = 0;
									for(i_cycle = 0;i_cycle <ClientNum; i_cycle++)//往所有连接的客户端都发送对象到来的消息
									{
										if(Session[i_cycle].ClientID != ClientServer)
										WriteDataToBufferSend(i_cycle, (byte*)pPacket, data_len);	
									}						
////								
									/******遍历寻找此对象在缓冲区数组的位置， 此处亦须释放对应的ObjectBuffer对象，否则无法再创建对象，无法往ObjectBuffer内填充值*****/
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
										xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerPush, 	 eSetValueWithOverwrite, NULL);//发送通知去触发剔除																							
									}
									ObjectBuffer[Object_i].objectAliveFlag = false;									//释放被占有的对象
																		
									ObjectInModuleList[Module_i][Action_i].IsActionAlive = false;		//释放占有的动作
									testFlag = true;
									break;
								
								default:break;
							}
//						break;
						}
					}
				}			
			
//printf("Get for whole List Cycle Time ==>%d \n", __HAL_TIM_GET_COUNTER(&TIM6_Handler) - timeCount_for_cycle);//空载440us一个循环，3次为1320us
//				
//				if(isActionIn == true)	//用于跳出大的Module循环
//				{
//					isActionIn = false;
//					break;
//				}
//			
			}
//printf("Sending Module whole cycle Spending time  ==>%d \n", __HAL_TIM_GET_COUNTER(&TIM6_Handler) - timeCountTime_Module);//1378us
			

/************************************************************************************************************************************************************			
*****************************************************************跟踪过程监控区END******************************************************************************				
*************************************************************************************************************************************************************/				

/***运行时间统计***/
//////			
//////			TimeCountStart 	=  OverflowCount_TIM6*0XFFFF + __HAL_TIM_GET_COUNTER(&TIM6_Handler) - TimeCountStart;		//11us
//////			TimeCountStartRTOS = xTaskGetTickCount() - TimeCountStartRTOS;
//////			
//////			if(TimeCountStart > 800 && TimeCountStart<65536 && testFlag == true)//不知为何会出现大于65536+TimeCountStart(正常)的计数值情况出现...
//////			{	
//////				printf("TrackingTime ==>%d \n", TimeCountStart);//No-Load:1430us   Load:1600
//////				//printf("TrackingTimeRTOSSSS ==>%d \n", TimeCountStartRTOS);
//////				testFlag = false;
//////			}	

		}
	vTaskDelay(1);																								//不阻塞1ms的话，会一直跑这个程序	
	}	
}

