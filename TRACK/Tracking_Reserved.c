/**************************************************************************************************
*************************************跟踪线程******************************************************
***************************************************************************************************/
#include "Tracking.h"
#include <math.h>

extern uint32_t Timer;			//使用主函数定义的仿真Timer

//Timer: 仿真坐标 
//EncoderTimer：编码器坐标
//#define CaptureNumber Timer;

__IO int64_t EncoderNumber = 0;     // 编码器计数值
__IO int64_t CaptureNumber = 0;     // 编码器计数值
__IO int64_t CaptureNumber2 = 0;		//编码器2计数值

TaskHandle_t TrackingThread_Handler;
static void TrackingThread(void *arg);

struct ObjectTracking ObjectBuffer[maxTrackingObjects];							//创建最大只能保存maxTrackingObjects个对象的数组
byte Object_GlobalID = 0;

#define LightSignal_1 1
#define LightSignal_2 2

__IO int64_t LS1_EncoderNumTem = 0;				//光电1对应的上一次编码器值(暂存用于双触发保护)
__IO int64_t LS2_EncoderNumTem = 0;				//光电1对应的上一次编码器值(暂存用于双触发保护)

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
	while(ObjectBuffer[objectCNT].objectAliveFlag != false)		//防止覆盖跟踪尚未结束的对象
	{
		objectCNT++;
		if(objectCNT > 64) objectCNT = 0;
	}
	//往对象缓冲数组里填充数据
	ObjectBuffer[objectCNT].ClientID = 0;											//待传入值，
	ObjectBuffer[objectCNT].ObjectID = objectCNT;							//待设置，暂时与循环同步
	ObjectBuffer[objectCNT].CreateObjectFlag = true;					//待传入值，是否创建对象，暂时设置为创建
	ObjectBuffer[objectCNT].BornEncoderNum = CaptureNumber;		//该对象的起始编码器值(出生值)
	ObjectBuffer[objectCNT].ModuleReferenceEncoderNum[moduleCNT] = CaptureNumber;	//对象所在的跟踪段参考编码器值
	ObjectBuffer[objectCNT].CameraID = 0;											//0表示没有相机
	ObjectBuffer[objectCNT].pData = strCreateObject;
	ObjectBuffer[objectCNT].ProcessedResult = true;						//待传入值，处理结果，暂时设置为好
	ObjectBuffer[objectCNT].objectAliveFlag = true;						//激活跟踪过程
	
	ObjectBuffer[objectCNT].objectModulePosition = moduleCNT;	//激活对应的跟踪段，对象处于第一个光电，激活第一段跟踪段, 由于后面有对象是否进入跟踪段的判断，所以第一个对象必须打开第一个跟踪段
	
	ObjectBuffer[objectCNT].ModuleReferenceEncoderNum[0] = ObjectBuffer[objectCNT].BornEncoderNum;//初始跟踪段的编码器值为出生编码器值
	
	TCPSendDataBase(ObjectBuffer[objectCNT].ClientID, strCreateObject);		
	
	objectCNT++;
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
	byte Object_i = 0, Module_i = 0, Action_i = 0;			//遍历控制变量
	bool exit_Module_Flag = false;											//用于模块退出循环
	bool isObjectFullNew = true;												//用于判断光电感应到的是全新的对象还是传递来的对象

	uint32_t TimeCountStart = 0;												//用于计算线程运行时间
	uint16_t timeCount = 0;
	 
	for(Object_i = 0; Object_i < maxTrackingObjects; Object_i++)										//初始化所有对象的所有动作，不激活动作
	{
		/*对象属性初始化*/
		ObjectBuffer[Object_i].objectAliveFlag 			= false;													//初始化对象标记为未激活跟踪状态
		ObjectBuffer[Object_i].objectDelieverdFlag 	= false;													//初始化对象为未被传递状态

		ObjectBuffer[Object_i].objectModulePosition			=	255;												//标记对象目前还不处于任一跟踪段，对象为全新的
		//ObjectBuffer[Object_i].delieverDestination	= 255;													//初始化对象所需传送到的跟踪段
		
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
		
		if( ClientNum > 0)															//有检测程序Inspection连接
		{		
			/***光电1***/			
			if(TIM2CH1_CAPTURE_STA & 0X80)        				//成功捕获到了一次高电平		光电1此处是由定时中断来确定采集到的信号长度的（非常精准）
			{
				printf("Catch the Light111 Signals\n");
				temp=TIM2CH1_CAPTURE_STA&0X3F;							//溢出次数
				temp*=0XFFFFFFFF;		 	    									//溢出时间总和  ******应该可以去掉
				temp+=TIM2CH1_CAPTURE_VAL;      						//得到总的高电平时间	
				printf("HIGH:%lld us\r\n",temp);						//打印总的高电平时间。
				
				CaptureNumber 	= (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);			//记录创建的对象的出生点编码器数值
				//CaptureNumber2 	= (OverflowCount_Encoder8*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder8);		//记录创建的对象的出生点编码器数值
				
				printf("Light111 BornSite Encoder Count==>%lld\n", CaptureNumber);
				
				isObjectFullNew = true;//初始化默认为是一个全新的对象，然后交由下面的来再次判断是不是全新的对象
				
				for(Module_i = 0;Module_i < Module_Count;Module_i++)								//找光电所属的跟踪段
				{
					if(LightSignal_1 == ModuleConfig[Module_i].ModuleTrigger && 			//Case1:找到了光电所在的跟踪段位置（有光电），并且此跟踪段允许创建对象
						 ModuleConfig[Module_i].CreateObjectFlag == true)
					{
						if(CaptureNumber > ModuleConfig[Module_i].DoubleTrigger)				//初次启动时由于CaptureNumber = LS1_EncoderNumTem=0会造成无法创建对象的问题
						{
							if(temp > ModuleConfig[Module_i].Debounce &&
								(CaptureNumber - LS1_EncoderNumTem) >  ModuleConfig[Module_i].DoubleTrigger)						//信号防抖和双触发保护
							{
								for(Object_i = 0; Object_i< maxTrackingObjects;Object_i++)															//判断缓冲区内所有的对象到了哪个光电
								{
									if(ObjectBuffer[Object_i].objectDelieverdFlag  == true &&
										 ObjectBuffer[Object_i].objectModulePosition == Module_i)	//对象被激活&&对象被传递了&&对象到了此跟踪段，才会进行跟踪窗口判断，确认是一个传递过来的动作
									{	////////////////////如果CaptureNumber溢出的时候，相减前面的就会出现一个错误的很大的值!!!!!得处理！！！！！！！！！！！！！					
										if(CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ObjectBuffer[Object_i].objectModulePosition] < 
											 ModuleConfig[Module_i].TrackingWindow )	//在指定的跟踪窗口到达，认为是同一个瓶子，不创建对象
										{
											
											CaptureNumber2 = CaptureNumber;
										
											ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] = CaptureNumber2 ;//设定当前段的编码器（使用后段编码器）参考值
											
											//ObjectBuffer[Object_i].objectModulePosition = Module_i;//更新对象所处的跟踪段位置
										}
										else	//如果未在指定的跟踪窗口内到达则认为是一个半途加进去的瓶子，也需创建一个对象
										{
											CreateObject(Object_GlobalID, Module_i);											//创建一个新的对象(可能为全新的，也可能是半途加了一个瓶子进去)
										
											LS1_EncoderNumTem = CaptureNumber;														//保存本次触发时刻的编码器值用于双触发保护
											
											Object_GlobalID++;
											if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;
													
										}
										
										isObjectFullNew = false;																					//标记光电产生的这不是一个完全全新的对象
										ObjectBuffer[Object_i].objectDelieverdFlag = false;								//当前对象的传递动作处理完毕，标记为未被传递状态
										break;																														//处理完毕，无需在检索后面的对象
									}								
								}														
								if(isObjectFullNew == true)			//判断此次光电产生的是不是完全全新的信号，完全全新的未被创建的对象：即第一个跟踪段产生的对象
								{
									CreateObject(Object_GlobalID, Module_i);																								//创建对应跟踪段的对象（对象ID, 跟踪段ID）
									
									
									LS1_EncoderNumTem = CaptureNumber;														//保存本次触发时刻的编码器值用于双触发保护
									Object_GlobalID++;
									if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;						
								}							
							}
						}
						else if(isObjectFullNew == true)			//(用于系统开机第一次触发。)判断此次光电产生的是不是完全全新的信号，完全全新的未被创建的对象：即第一个跟踪段产生的对象
						{
							CreateObject(Object_GlobalID, Module_i);																								//创建对应跟踪段的对象（对象ID, 跟踪段ID）
							
							
							LS1_EncoderNumTem = CaptureNumber;														//保存本次触发时刻的编码器值用于双触发保护
							Object_GlobalID++;
							if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;						
						}
						
						break;	//在跟踪段找到了光电并执行了相应的动作就跳出光电位置搜索循环，不查询下一个了
					}
					
					else if(LightSignal_1 == ModuleConfig[Module_i].ModuleTrigger && 			//Case2:找到了光电所在的跟踪段位置（有光电），并且此跟踪段不允许创建对象
									ModuleConfig[Module_i].CreateObjectFlag != true)
					{
						if(temp > ModuleConfig[Module_i].Debounce &&
							(CaptureNumber - LS1_EncoderNumTem) >  ModuleConfig[Module_i].DoubleTrigger)						//信号防抖和双触发保护
						{
							for(Object_i = 0; Object_i< maxTrackingObjects;Object_i++)															//判断缓冲区内所有的对象到了哪个光电
							{
								if(ObjectBuffer[Object_i].objectDelieverdFlag 	== true &&
									 ObjectBuffer[Object_i].objectModulePosition	== Module_i)	//对象被激活&&对象被传递了&&对象到了此跟踪段，才会进行跟踪窗口判断，确认是一个传递过来的动作
								{								
									if(CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ObjectBuffer[Object_i].objectModulePosition] < 
										 ModuleConfig[Module_i].TrackingWindow )	//在指定的跟踪窗口到达，认为是同一个瓶子，不创建对象
									{
										
										CaptureNumber2 = CaptureNumber;
									
										ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] = CaptureNumber2 ;//设定当前段的编码器（使用后段编码器）参考值
										
										//ObjectBuffer[Object_i].objectModulePosition = Module_i;//更新对象所处的跟踪段位置
									}								
									
									isObjectFullNew = false;																					//标记光电产生的这不是一个完全全新的对象
									ObjectBuffer[Object_i].objectDelieverdFlag = false;								//当前对象的传递动作处理完毕，标记为未被传递状态
									break;			
								}								
							}														
						}	
						break;	//在跟踪段找到了光电并执行了相应的动作就跳出光电位置搜索循环，不查询下一个了
					}
				//Case3:无光电不创建对象不在这里处理，在下面的跟踪过程监控区
				}
				
				TIM2CH1_CAPTURE_STA=0;          								//光电1开启下一次捕获			
			}			
			
			
			
			/***光电2***/	
			if(TIM5CH1_CAPTURE_STA & 0X80)        					//成功捕获到了一次高电平		光电2此处是由定时中断来确定采集到的信号长度的（非常精准）
			{
				printf("Catch the Light222 Signals\n");
				temp = TIM5CH1_CAPTURE_STA & 0X3F;							//溢出次数
				temp*= 0XFFFFFFFF;		 	    										//溢出时间总和  ******应该可以去掉
				temp+= TIM5CH1_CAPTURE_VAL;      								//得到总的高电平时间	
				printf("HIGH:%lld us\r\n",temp);								//打印总的高电平时间
				
				
				CaptureNumber 	= (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);			//记录创建的对象的出生点编码器数值
				//CaptureNumber2 	= (OverflowCount_Encoder8*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder8);		//记录创建的对象的出生点编码器数值
				
				printf("Light222 BornSite Encoder Count==>%lld\n", CaptureNumber);
				
				isObjectFullNew = true;//初始化默认为是一个全新的对象，然后交由下面的来再次判断是不是全新的对象
				
				for(Module_i = 0;Module_i < Module_Count;Module_i++)								//找光电所属的跟踪段
				{
					if(LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger && 			//Case1:找到了光电所在的跟踪段位置（有光电），并且此跟踪段允许创建对象
						 ModuleConfig[Module_i].CreateObjectFlag == true)
					{
						if(CaptureNumber > ModuleConfig[Module_i].DoubleTrigger)
						{
							if(temp > ModuleConfig[Module_i].Debounce &&
								(CaptureNumber - LS2_EncoderNumTem) >  ModuleConfig[Module_i].DoubleTrigger)						//信号防抖和双触发保护
							{
								for(Object_i = 0; Object_i< maxTrackingObjects;Object_i++)															//判断缓冲区内所有的对象到了哪个光电
								{
									if(ObjectBuffer[Object_i].objectDelieverdFlag  == true &&
										 ObjectBuffer[Object_i].objectModulePosition == Module_i)	//对象被激活&&对象被传递了&&对象到了此跟踪段，才会进行跟踪窗口判断，确认是一个传递过来的动作
									{	////////////////////如果CaptureNumber溢出的时候，相减前面的就会出现一个错误的很大的值!!!!!得处理！！！！！！！！！！！！！					
										if(CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ObjectBuffer[Object_i].objectModulePosition] < 
											 ModuleConfig[Module_i].TrackingWindow )	//在指定的跟踪窗口到达，认为是同一个瓶子，不创建对象
										{
											
											CaptureNumber2 = CaptureNumber;
										
											ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] = CaptureNumber2 ;//设定当前段的编码器（使用后段编码器）参考值
											
											//ObjectBuffer[Object_i].objectModulePosition = Module_i;//更新对象所处的跟踪段位置
										}
										else	//如果未在指定的跟踪窗口内到达则认为是一个半途加进去的瓶子，也需创建一个对象
										{
											CreateObject(Object_GlobalID, Module_i);											//创建一个新的对象(可能为全新的，也可能是半途加了一个瓶子进去)
										
											LS2_EncoderNumTem = CaptureNumber;														//保存本次触发时刻的编码器值用于双触发保护
											
											Object_GlobalID++;
											if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;
													
										}
										
										isObjectFullNew = false;																					//标记光电产生的这不是一个完全全新的对象
										ObjectBuffer[Object_i].objectDelieverdFlag = false;								//当前对象的传递动作处理完毕，标记为未被传递状态
										break;																														//处理完毕，无需在检索后面的对象
									}								
								}														
								if(isObjectFullNew == true)			//判断此次光电产生的是不是完全全新的信号，完全全新的未被创建的对象：即第一个跟踪段产生的对象
								{
									CreateObject(Object_GlobalID, Module_i);																								//创建对应跟踪段的对象（对象ID, 跟踪段ID）
									
									
									LS2_EncoderNumTem = CaptureNumber;														//保存本次触发时刻的编码器值用于双触发保护
									Object_GlobalID++;
									if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;						
								}							
							}
						}
						else if(isObjectFullNew == true)			//(用于系统开机第一次触发。)判断此次光电产生的是不是完全全新的信号，完全全新的未被创建的对象：即第一个跟踪段产生的对象
						{
							CreateObject(Object_GlobalID, Module_i);																								//创建对应跟踪段的对象（对象ID, 跟踪段ID）
							
							
							LS2_EncoderNumTem = CaptureNumber;														//保存本次触发时刻的编码器值用于双触发保护
							Object_GlobalID++;
							if(Object_GlobalID > maxTrackingObjects) Object_GlobalID = 0;						
						}
						
						break;	//在跟踪段找到了光电并执行了相应的动作就跳出光电位置搜索循环，不查询下一个了
					}
					
					else if(LightSignal_2 == ModuleConfig[Module_i].ModuleTrigger && 			//Case2:找到了光电所在的跟踪段位置（有光电），并且此跟踪段不允许创建对象
									ModuleConfig[Module_i].CreateObjectFlag != true)
					{
						if(temp > ModuleConfig[Module_i].Debounce &&
							(CaptureNumber - LS2_EncoderNumTem) >  ModuleConfig[Module_i].DoubleTrigger)						//信号防抖和双触发保护
						{
							for(Object_i = 0; Object_i< maxTrackingObjects;Object_i++)															//判断缓冲区内所有的对象到了哪个光电
							{
								if(ObjectBuffer[Object_i].objectDelieverdFlag 	== true &&
									 ObjectBuffer[Object_i].objectModulePosition	== Module_i)	//对象被激活&&对象被传递了&&对象到了此跟踪段，才会进行跟踪窗口判断，确认是一个传递过来的动作
								{								
									if(CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ObjectBuffer[Object_i].objectModulePosition] < 
										 ModuleConfig[Module_i].TrackingWindow )	//在指定的跟踪窗口到达，认为是同一个瓶子，不创建对象
									{
										
										CaptureNumber2 = CaptureNumber;
									
										ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] = CaptureNumber2 ;//设定当前段的编码器（使用后段编码器）参考值
										
										//ObjectBuffer[Object_i].objectModulePosition = Module_i;//更新对象所处的跟踪段位置
									}								
									
									isObjectFullNew = false;																					//标记光电产生的这不是一个完全全新的对象
									ObjectBuffer[Object_i].objectDelieverdFlag = false;								//当前对象的传递动作处理完毕，标记为未被传递状态
									break;			
								}								
							}														
						}	
						break;	//在跟踪段找到了光电并执行了相应的动作就跳出光电位置搜索循环，不查询下一个了
					}
				//Case3:无光电不创建对象不在这里处理，在下面的跟踪过程监控区
				}				
			TIM5CH1_CAPTURE_STA=0;          									//光电2开启下一次捕获			
		 }								


			
/************************************************************************************************************************************************************			
*****************************************************************跟踪过程监控区*********************************************************************************				
*************************************************************************************************************************************************************/				
			for(Object_i = 0; Object_i< maxTrackingObjects;Object_i++)										//维护并管理缓冲区的所有对象
			{	
				if(ObjectBuffer[Object_i].objectAliveFlag == true)													//对象被激活了才会进入跟踪过程
				{								
					for(Module_i = 0;Module_i < Module_Count;Module_i++)
					{	
						if(ObjectBuffer[Object_i].objectModulePosition == Module_i)							//对象只能进入允许其所在的跟踪段
						{							
							for(Action_i = 0;Action_i < maxActionSingleModule; Action_i++)				//单个Module最大的动作个数，一个一个去扫描(由于未知动作数量，必须得这么操作！！！最好可以调用动作数量NumberOfActions)
							{
								if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.ActionTriggerFlag == true)			//哪个动作,以及在跟踪段上的TargetValue
								{
									CaptureNumber = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
									
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue) >0 )//说明对象ObjectBuffer[Object_i]到了ModuleConfig[Module_i].ActionInstanceConfig[Action_i]这个动作的位置
									{
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)					//判断是否重复触发动作
										{
											printf("Item_ActionObjectTakeOver ObjectBuffer==%d , Module==%d, ActionNum==%d, CaptureNumber==>%llu, \n", Object_i, Module_i, Action_i, CaptureNumber);
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionObjectTakeOver);
											
											//标记跟踪对象Object_i发生了一次动作传递......................,也就是声明，当前跟踪段的对象X传递出去了。。。
											ObjectBuffer[Object_i].objectDelieverdFlag 		= true;
											ObjectBuffer[Object_i].objectModulePosition 	= ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule;//修改对象所在的跟踪段位置
											
											if((ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].ModuleTrigger == 0) && 
												(ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].CreateObjectFlag == false))		//所需传递到的跟踪段如没有光电和不创建对象,另两种情况交由光电去探测
											{
												//传递对象给所需传送的跟踪段,修改传送目的跟踪段的参考编码器值 = 传送段的参考编码器值 + 传递动作的目标值 
												ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule] = 
																		ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;		
											
											}
/***此两种情况交由光电去处理										
											else if((ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].ModuleTrigger > 0) && 
												(ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].CreateObjectFlag == false))		//所需传递到的跟踪段有光电但不创建对象，不打开对应跟踪段，由光电去打开对应跟踪段
											{																								
												//传递对象给所需传送的跟踪段,修改传送目的跟踪段的参考编码器值 = 传送段的参考编码器值 + 传递动作的目标值 
												ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule] = 
															ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;	
												
												
												
											}
											else if((ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].ModuleTrigger > 0) && 
												(ModuleConfig[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule].CreateObjectFlag == true))		//所需传递到的跟踪段有光电且要创建对象，不打开对应跟踪段，由光电去打开对应跟踪段
											{																								
												//传递对象给所需传送的跟踪段,修改传送目的跟踪段的参考编码器值 = 传送段的参考编码器值 + 传递动作的目标值 
												ObjectBuffer[Object_i].ModuleReferenceEncoderNum[ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.DestinationModule] = 
															ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] + ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionObjectTakeOver.TargetValue;		
												
											}
***/											
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;
											
											exit_Module_Flag = true;				//跳出跟踪段循环==>搜到了相应的动作						
											break;													//跳出动作查询循环.
										}
									}
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.ActionTriggerFlag == true)
								{
									CaptureNumber = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
									//说明对象ObjectBuffer[Object_i]到了ModuleConfig[Module_i].ActionInstanceConfig[Action_i]这个动作的位置
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionPushOut.TargetValue) > 0)		
									{
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)						//此动作未被触发过
										{
											printf("Item_ActionPushOut Module==%d, ActionNum==%d, ObjectBuffer==%d , CaptureNumber==>%llu, \n",Module_i, Action_i, Object_i,CaptureNumber);											
											xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerPush, eSetValueWithOverwrite, NULL);	//发送通知去触发剔除
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionPushOut);
											
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;														
											
											/*************
											******************************销毁对象的时候一定要把它的属性全部恢复初始状态！！！！！！！！！！
											*****************************************************************************************************/
											ObjectBuffer[Object_i].objectAliveFlag = false;				//默认剔除处为对象生命周期的结束点！！！！！！！！！！！！！！！！！！！！！！
											ObjectBuffer[Object_i].objectModulePosition  = 255;		//标记对象不属于任何一个跟踪段
											ObjectBuffer[Object_i].objectDelieverdFlag 	= false;
											
											for(Module_i = 0;Module_i <Module_Count ;Module_i++)	//重置对象的所有动作执行状态为false，将资源交还给缓冲区
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
									
									//说明对象ObjectBuffer[Object_i]到了ModuleConfig[Module_i].ActionInstanceConfig[Action_i]这个动作的位置
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
									
									//说明对象ObjectBuffer[Object_i]到了ModuleConfig[Module_i].ActionInstanceConfig[Action_i]这个动作的位置
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionSetOutput.TargetValue) > 0)		
									{	
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)
										{						
											printf("Item_ActionSetOutput Module==%d, ActionNum==%d, ObjectBuffer==%d , CaptureNumber==>%llu, \n",Module_i, Action_i, Object_i,CaptureNumber);
											xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerOutput, eSetValueWithOverwrite, NULL);	//发送通知去触发输出
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionSetOutput);
											
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;
											
											exit_Module_Flag = true;
											break;		//跳出动作查询循环	
										}										
									}
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.ActionTriggerFlag == true)
								{			
									CaptureNumber = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
									
									//说明对象ObjectBuffer[Object_i]到了ModuleConfig[Module_i].ActionInstanceConfig[Action_i]这个动作的位置
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerCamera.TargetValue) > 0)		
									{	
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)
										{						
											printf("Item_ActionTriggerCamera Module==%d, ActionNum==%d, ObjectBuffer==%d , CaptureNumber==>%llu, \n",Module_i, Action_i, Object_i,CaptureNumber);
											xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerCamera, eSetValueWithOverwrite, NULL);	//发送通知去触发相机
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionTriggerCamera);
																					
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;
		
											exit_Module_Flag = true;
											break;		//跳出动作查询循环
										}
									}
								}
								else if(ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.ActionTriggerFlag == true)
								{					
									CaptureNumber = (OverflowCount_Encoder3*ENCODER_CNT_MAX) + __HAL_TIM_GET_COUNTER(&htimx_Encoder3);
									
									//说明对象ObjectBuffer[Object_i]到了ModuleConfig[Module_i].ActionInstanceConfig[Action_i]这个动作的位置
									if(((CaptureNumber - ObjectBuffer[Object_i].ModuleReferenceEncoderNum[Module_i] ) - 
											 ModuleConfig[Module_i].ActionInstanceConfig[Action_i].Item_ActionTriggerSensor.TargetValue) > 0)		
									{
										if(ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] != true)
										{
											printf("Item_ActionTriggerSensor Module==%d, ActionNum==%d, ObjectBuffer==%d , CaptureNumber==>%llu, \n",Module_i, Action_i, Object_i,CaptureNumber);
											xTaskGenericNotify(ActionExecuteTask_Handler, Message_TrrigerSensor, eSetValueWithOverwrite, NULL);	//发送通知去触发传感器采集
											TCPSendDataBase(ObjectBuffer[Object_i].ClientID, strActionTriggerSensor);
																					
											ObjectBuffer[Object_i].triggerPosition[Module_i][Action_i] 	= true;
		
											exit_Module_Flag = true;
											break;		//跳出动作查询循环
										}
									}
								}
							}							
						}

						
					if(exit_Module_Flag == true)									//判断当前跟踪段是否找到了动作，找到了就退出，没找到就继续找
						{
							exit_Module_Flag = false;
							break;
						}						
					}
				}
			}
/************************************************************************************************************************************************************			
*****************************************************************跟踪过程监控区END*********************************************************************************				
*************************************************************************************************************************************************************/		
			
//		if(Timer > 3020) Timer = 0;		//重复触发测试
		}
		timeCount = xTaskGetTickCount()- TimeCountStart;
		if(timeCount > 0)
		{
			printf("Tracking Thread Running Time ==>%d \n", timeCount);//首次28ms跑一次，后面18ms跑一次
		}

	vTaskDelay(1);																								//不阻塞1ms的话，会一直跑这个程序
	}	
}

