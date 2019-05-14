/**************************************************************************************************
*************************************跟踪线程******************************************************
***************************************************************************************************/
#include "ObjectDetection.h"
#include <math.h>
#include "TCPProtocol.h"

TaskHandle_t ObjectDetectionThread_Handler;
QueueHandle_t	ModuleQueue[maxTrackingModule];				//创建信息队列用于接收信息
#define moduleQueueDepth	10

static char* strCreateObject = "<=================Detect someting Create Objectr===================>";
 
__IO ObjectInfo ObjectBuffer[maxTrackingObjects];										//创建最大只能保存maxTrackingObjects个对象的数组 
//ObjectList ObjectInModuleList[maxTrackingModule];								//定义一个跟踪段指针列表数组

StctActionListItem ObjectInModuleList[maxTrackingModule][maxTrackingObjects];//二维数组用于存储所有动作列表

__IO	uint32_t GlobalObjectID;																	//定义一个全局对象ID，__IO表示直接从地址处取值，取得最新值，允许所有软件硬件修改此值

static void ObjectDetectionThread(void);


static byte CreateObject(byte objectCNT, int moduleID, int encoder, int flag, int lastTriggerPreviousEncoder,int lastReceivePreviousEncoder)
{
	Packet* pPacket;
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
	ObjectBuffer[objectCNT].pData 	 = strCreateObject;
	ObjectBuffer[objectCNT].ProcessedResult = true;							//待传入值，处理结果，暂时设置为好
	ObjectBuffer[objectCNT].objectAliveFlag = true;							//激活跟踪过程
	
	GlobalObjectID++;

	pPacket = CreateStartTrackingPacket(1, GlobalObjectID);
	printf("%s\r\n",(char*)pPacket);
	TCPSendPacket(ClientServer, pPacket);
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


//创建对象探测任务
//返回值:0 探测任务创建成功
//		其他 探测任务创建失败
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

//对象探测线程
static void ObjectDetectionThread(void)
{
	long long temp = 0;			
	byte Object_i = 0, Module_i = 0, Action_i = 0;		//遍历控制变量
	byte GlobalObjectCount = 0;
	uint32_t TimeCountStart = 0;											//用于计算线程运行时间
	uint16_t timeCount = 0;
	static bool AddActiveFlag = false;	
	static StctActionListItem objectTrackTemp ;				//跟踪数据暂存值
	static ModuleQueueItem* moduleQueueTemp;					//定义一个从队列中暂取数据的暂存值指针
	static __IO int64_t encoderNumber = 0;     				// 编码器计数值
	static __IO int64_t encoder1Number = 0;
	static __IO int64_t encoder2Number = 0;
	
	static short triggerInterval = 0;									//两次触发间隔编码器数
	static __IO int64_t LS1_EncoderNumTem = 0;				//光电1对应的上一次编码器值(暂存用于双触发保护)
	static __IO int64_t LS2_EncoderNumTem = 0;				//光电2对应的上一次编码器值(暂存用于双触发保护)
	static __IO int64_t LS3_EncoderNumTem = 0;				//光电3对应的上一次编码器值(暂存用于双触发保护)
	static __IO int64_t LS4_EncoderNumTem = 0;				//光电4对应的上一次编码器值(暂存用于双触发保护)
	
	
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
*****************************************************************跟踪过程监控区*********************************************************************************				
*************************************************************************************************************************************************************/				
	while(1)
	{	
		TimeCountStart = xTaskGetTickCount();
		
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
							printf("==>Module[%d] Catch the Light111 Signals\n", Module_i);
							temp=TIM2CH1_CAPTURE_STA&0X3F;							//溢出次数
							temp*=0XFFFFFFFF;		 	    									//溢出时间总和  ******应该可以去掉
							temp+=TIM2CH1_CAPTURE_VAL;      						//得到总的高电平时间	
							printf("HIGH:%lld us\r\n",temp);						//打印总的高电平时间。
							printf("Using Encoder[%d], EncoderNumber is [%lld]\n", ModuleConfig[Module_i].Encoder, encoderNumber);
							
							TIM2CH1_CAPTURE_STA = 0;          								//光电1开启下一次捕获		
							triggerInterval 	= encoderNumber - LS1_EncoderNumTem;
							LS1_EncoderNumTem = encoderNumber;					//保存光电上一次编码器的触发值
							AddActiveFlag = true;
						}						
						/****光电2***/					
						else if((LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger) && (TIM5CH1_CAPTURE_STA & 0X80))
						{
							printf("==>Module[%d] Catch the Light222 \n", Module_i);
							temp=TIM5CH1_CAPTURE_STA&0X3F;							//溢出次数
							temp*=0XFFFFFFFF;		 	    									//溢出时间总和  ******应该可以去掉
							temp+=TIM5CH1_CAPTURE_VAL;      						//得到总的高电平时间	
							printf("HIGH:%lld us\r\n",temp);						//打印总的高电平时间。
							printf("Using Encoder[%d], EncoderNumber is [%lld]\n", ModuleConfig[Module_i].Encoder, encoderNumber);
							
							TIM5CH1_CAPTURE_STA = 0;          								//光电1开启下一次捕获		
							triggerInterval 	= encoderNumber - LS2_EncoderNumTem;
							LS2_EncoderNumTem = encoderNumber;					//保存光电上一次编码器的触发值
							AddActiveFlag = true;
						}
						if(AddActiveFlag == true)
						{
							if(triggerInterval >  ModuleConfig[Module_i].Debounce	&&
							   triggerInterval > ModuleConfig[Module_i].DoubleTrigger)	//信号防抖和双触发保护
							{
								if(ModuleConfig[Module_i].CreateObjectFlag == true)		//此跟踪段允许创建对象
								{	
									//系统初次启动编码器为0，可能无法创建兑现得转过一定编码器才会开始创建对象...
									if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//为传递过来的对象，仅将对象在此段的动作加入列表
									{
										for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
										{
											/*判断是哪种类型的动作(由于定位到了某个跟踪段的某个动作，所以是确定当前动作的种类)*/
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
									else		//认为是一个全新的对象了，创建对象，并将目前对象所在跟踪段的动作加入对应的跟踪段列表（无论是否为第一个跟踪段都可以使用）
									{
										printf("Create A New Object, Delivered Object not comming in time. \n");
										if(GlobalObjectCount > maxTrackingObjects - 1) GlobalObjectCount = 0;//缓冲大小为64，Count必须小于64，否则赋值给CreateObject会出错
										GlobalObjectCount = CreateObject(GlobalObjectCount, Module_i, ModuleConfig[Module_i].Encoder, 1, encoderNumber, encoderNumber);
										GlobalObjectCount++;
																				
										/*判断是哪种类型的动作(由于定位到了某个跟踪段的某个动作，所以是确定当前动作的种类)*/
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
								else if(ModuleConfig[Module_i].CreateObjectFlag != true)//此跟踪段不允许创建对象
								{
									printf("Module[%d] not allowed to Create Object, Just Deliverd. \n", Module_i);
									//系统初次启动编码器为0，可能无法创建兑现得转过一定编码器才会开始创建对象...
									if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//为传递过来的对象，仅将对象在此段的动作加入列表
									{

										for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
										{
											/*判断是哪种类型的动作(由于定位到了某个跟踪段的某个动作，所以是确定当前动作的种类)*/
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
					/****跟踪段无光电&&只进行对象的传递*****/
					else								
					{	
						//系统初次启动编码器为0，可能无法创建得先转过一定编码器才会开始创建对象...
						if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)					//为传递过来的对象，仅将对象在此段的动作加入列表
						{
							for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
							{
								/*判断是哪种类型的动作(由于定位到了某个跟踪段的某个动作，所以是确定当前动作的种类)*/
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
			
				
				else			//非传递而来待测物体对象的创建...消息队列中没有对象传递，但是光电仍感应到了信号
				{
					if(ModuleConfig[Module_i].ModuleTrigger > 0)	//当前跟踪段有光电
					{
						/*********光电1*********/
						if((LightSignal_1 == ModuleConfig[Module_i].ModuleTrigger) && (TIM2CH1_CAPTURE_STA & 0X80) )		//如果跟踪段使用光电1
						{
							if(ModuleConfig[Module_i].CreateObjectFlag == true)	//允许创建对象
							{
								printf("==>Module[%d] Catch the Light111 Signals Not by Delivered Object Pretty New. \n", Module_i);
								temp=TIM2CH1_CAPTURE_STA&0X3F;							//溢出次数
								temp*=0XFFFFFFFF;		 	    									//溢出时间总和  ******应该可以去掉
								temp+=TIM2CH1_CAPTURE_VAL;      						//得到总的高电平时间	
								printf("HIGH:%lld us\r\n",temp);						//打印总的高电平时间。
								printf("Module[%d]==>Using Encoder[%d], EncoderNumber is [%lld]\n", Module_i, ModuleConfig[Module_i].Encoder, encoderNumber);

								TIM2CH1_CAPTURE_STA = 0;          								//光电1开启下一次捕获			
								triggerInterval = encoderNumber - LS1_EncoderNumTem;
								LS1_EncoderNumTem = encoderNumber;					//记录上次光电被触发的值
								AddActiveFlag = true;
							}	
						}
						else if((LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger) && (TIM5CH1_CAPTURE_STA & 0X80) )		//如果跟踪段使用光电2
						{						
							if(ModuleConfig[Module_i].CreateObjectFlag == true)	//跟踪段允许创建对象
							{
								printf("==>Module[%d] Catch the Light222 Signals Signals Not by Delivered Object Pretty New. \n", Module_i);
								temp=TIM5CH1_CAPTURE_STA&0X3F;							//溢出次数
								temp*=0XFFFFFFFF;		 	    									//溢出时间总和  ******应该可以去掉
								temp+=TIM5CH1_CAPTURE_VAL;      						//得到总的高电平时间	
								printf("HIGH:%lld us\r\n",temp);						//打印总的高电平时间。
								printf("Module[%d]==>Using Encoder[%d], EncoderNumber is [%lld]\n", Module_i, ModuleConfig[Module_i].Encoder, encoderNumber);

								TIM5CH1_CAPTURE_STA = 0;          								//光电2开启下一次捕获	
								triggerInterval = encoderNumber - LS2_EncoderNumTem;
								LS2_EncoderNumTem = encoderNumber;					//记录上次光电被触发的值
								AddActiveFlag = true;
							}
						}
							
						if(AddActiveFlag == true)
						{
							if(triggerInterval >  ModuleConfig[Module_i].Debounce	&&
							   triggerInterval > ModuleConfig[Module_i].DoubleTrigger)	//信号防抖和双触发保护
							{
								if(GlobalObjectCount > maxTrackingObjects - 1) GlobalObjectCount = 0;//缓冲大小为64，Count必须小于64，否则赋值给CreateObject会出错
								GlobalObjectCount = CreateObject(GlobalObjectCount, Module_i, ModuleConfig[Module_i].Encoder, 1, encoderNumber, encoderNumber);
								GlobalObjectCount++;
																
								/*往跟踪段列表加入对应的动作*/
								for(Action_i = 0;Action_i < ModuleConfig[Module_i].NumberOfActions;Action_i++)
								{
									/*判断是哪种类型的动作(由于定位到了某个跟踪段的某个动作，所以是确定当前动作的种类)*/
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
*****************************************************************跟踪过程监控区END*********************************************************************************				
*************************************************************************************************************************************************************/		
			
//		if(Timer > 3020) Timer = 0;		//重复触发测试
		
		timeCount = xTaskGetTickCount()- TimeCountStart;
		if(timeCount > 0)
		{
			printf("ObjectDetection Thread Running Time ==>%d \n", timeCount);
		}

	vTaskDelay(1);																								//不阻塞1ms的话，会一直跑这个程序
	}	
}

