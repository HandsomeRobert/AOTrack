/**************************************************************************************************
**************************���������߳�**************************************************************
***************************************************************************************************/
#include "ParametersLoad.h"
#include <string.h>
#include <math.h>
#include "ScanIO.h"
#include "malloc.h"

TaskHandle_t ParametersLoad_Handler;
static void ParametersLoadThread(void *arg);

//������������
TrackingConfiguration 	TrackingConfig;														//����һ�����ø��ٶι��õ�����
ModuleConfiguration		ModuleConfig[maxTrackingModule];					//��������������ٶ�����
uint8_t Module_Count = 0;

//����ת������
////��char*ָ�������ת��Ϊuint32����
uint32_t pChartoUint32(char* p)
{
	uint32_t sum =0;
	int length = strlen(p);
	while((*p) != '\0')												//'\0'Ϊ�ַ���������
	{
		sum += ((*p)-'0') * pow(10, length-1);	//10��length-1�η�
		p++;
		length--;
	}
	return sum;
}
//��char* ָ����ַ�boolת��Ϊbool��
bool pChartoBool(char* p)
{
	char str_true[] 	= "true"; 
	
	if(strcmp(p, str_true) == 0) return true;
	else 
		return false;
	
}

//�������������߳�
//����ֵ:0 �����ɹ�
//		���� ����ʧ��
uint8_t ParametersLoadTask_init(void)
{
	uint8_t res;
	taskENTER_CRITICAL();              
	res = xTaskCreate((TaskFunction_t)ParametersLoadThread,
					(const char*  )"TracingTread",
					(uint16_t     )PARAMETERSLOAD_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )PARAMETERSLOAD_TASK_PRIO,
					(TaskHandle_t*)&ParametersLoad_Handler);
	taskEXIT_CRITICAL();

	return res;
}

//���������߳�
static void ParametersLoadThread(void *arg)
{
	//�����ɶ�ֵ�ź��������������ź������������ز���
																									//���ٶ�������׷��
	uint8_t j_cycle = 0;	
	char* pConfigTemp = NULL;
	//���ٶζ���������׷��
	char str_ActionRequestMachineData[]  	= "ActionRequestMachineData";//��������
	char str_ActionSetOutput[] 				   	= "ActionSetOutput";
	char str_ActionObjectTakeOver[] 			= "ActionObjectTakeOver";
	char str_ActionTriggerCamera[] 				= "ActionTriggerCamera";
	char str_ActionTriggerSensor[]		 		= "ActionTriggerSensor";
	char str_ActionPushOut[] 							= "ActionPushOut";
	
	while(1)
	{
		if(xSemaphoreTake(OnSysRestart, 0) == pdTRUE)		//�ж��Ƿ�����������...�˴���ȡ�ź������ֹ���ȡʧ��...������configASSERT erro
		{
			NVIC_SystemReset();//��������ϵͳ������ʹ��HAL_NVIC_SystemReset();
		}
		
		if(xSemaphoreTake(OnLoadParametersFromPC, 1000) == pdTRUE)//�ź���Ϊ1ʱ
		{
			//������
			ezxml_t track_x, moduleList_x, module_x, actionList_x, action_x;
printf("{TestFlag}: Starting Paraser the xml \n");
			track_x = ezxml_parse_file("tracking.xml");	
			
printf("{TestFlag2}: whether paraser the tracking.xml Successfully \n");

			TrackingConfig.ConfigurationID 	= pChartoUint32((ezxml_child(track_x, "ConfigurationID")->txt));
			TrackingConfig.HostClientID 		= pChartoUint32((ezxml_child(track_x, "HostClientID")->txt));
			TrackingConfig.SimClientID 			= pChartoUint32((ezxml_child(track_x, "SimClientID")->txt));
			TrackingConfig.Timer 						= pChartoUint32((ezxml_child(track_x, "Timer")->txt));
			TrackingConfig.ReadySignal 			= pChartoUint32((ezxml_child(track_x, "ReadySignal")->txt));
			TrackingConfig.ErrorSignal 			= pChartoUint32((ezxml_child(track_x, "ErrorSignal")->txt));
			TrackingConfig.MessageSignal 		= pChartoUint32((ezxml_child(track_x, "MessageSignal")->txt));
			TrackingConfig.WarningSignal 		= pChartoUint32((ezxml_child(track_x, "WarningSignal")->txt));
		
printf("{TestFlag3}: start parasering the TrackingConfiguration \n");	
		
			for(moduleList_x = ezxml_child(track_x, "ModuleList"); moduleList_x; moduleList_x = moduleList_x -> ordered)
			{
printf("{TestFlag4}: Checking whether into ModuleList \n");
				
				for(module_x = ezxml_child(moduleList_x, "Module"); module_x; module_x = module_x->ordered)//���ٶν���
				{
					if(Module_Count < 8)			//���ֻ����8�����ٶ�
					{
printf("{TestFlag5}: Checking whether into Module \n");
						ModuleConfig[Module_Count].ConfigurationID 		= pChartoUint32((ezxml_child(module_x, "ConfigurationID")->txt));
						ModuleConfig[Module_Count].Name 							= ezxml_child(module_x, "Name")->txt;
						ModuleConfig[Module_Count].ModuleID 					= pChartoUint32((ezxml_child(module_x, "ModuleID")->txt));
						ModuleConfig[Module_Count].LineID 						= pChartoUint32((ezxml_child(module_x, "LineID")->txt));
						ModuleConfig[Module_Count].CreateObjectFlag 	= pChartoBool((ezxml_child(module_x, "CreateObjectFlag")->txt));
						ModuleConfig[Module_Count].ModuleTrigger 			= pChartoUint32((ezxml_child(module_x, "ModuleTrigger")->txt));
						ModuleConfig[Module_Count].ModuleTriggerEdge 	= pChartoUint32((ezxml_child(module_x, "ModuleTriggerEdge")->txt));
						ModuleConfig[Module_Count].HighActive 				= pChartoUint32((ezxml_child(module_x, "HighActive")->txt));
						ModuleConfig[Module_Count].Encoder 						= pChartoUint32((ezxml_child(module_x, "Encoder")->txt));
						ModuleConfig[Module_Count].EncoderSignal 			= pChartoUint32((ezxml_child(module_x, "EncoderSignal")->txt));
						ModuleConfig[Module_Count].EncoderEdge 				= pChartoUint32((ezxml_child(module_x, "EncoderEdge")->txt));
						ModuleConfig[Module_Count].TrackingWindow 		= pChartoUint32((ezxml_child(module_x, "TrackingWindow")->txt));
						ModuleConfig[Module_Count].DoubleTrigger 			= pChartoUint32((ezxml_child(module_x, "DoubleTrigger")->txt));
						ModuleConfig[Module_Count].Debounce 					= pChartoUint32((ezxml_child(module_x, "Debounce")->txt));
						ModuleConfig[Module_Count].HighestSpeed 			= pChartoUint32((ezxml_child(module_x, "HighestSpeed")->txt));
						ModuleConfig[Module_Count].LowestSpeed 				= pChartoUint32((ezxml_child(module_x, "LowestSpeed")->txt));
						ModuleConfig[Module_Count].ObjectWidth 				= pChartoUint32((ezxml_child(module_x, "ObjectWidth")->txt));
						ModuleConfig[Module_Count].ObjectInterval 		= pChartoUint32((ezxml_child(module_x, "ObjectInterval")->txt));
						ModuleConfig[Module_Count].NumberOfActions 		= 0;//pChartoUint32((ezxml_child(module_x, "NumberOfActions")->txt))
						
						for(actionList_x = ezxml_child(module_x, "ActionList"); actionList_x; actionList_x = actionList_x->ordered)
						{
printf("{TestFlag6}: Checking whether into ActionList \n");
							for(action_x = actionList_x ->child; action_x ; action_x = action_x ->ordered)
							{
								
								if(strcmp(action_x->name, str_ActionRequestMachineData) == 0)
								{
printf("{TestFlag7}: Checking whether into ActionRequestMachineData \n");
								
									//��ָ�����洢��Ӧ��������Ϣ������ʱʹ��ǿ������ת������������
									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].ActionType 		= ActRequestMachineData;
									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig 	= (char*)mymalloc(SRAMEX, sizeof(propActionRequestMachineData));
									pConfigTemp = ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig;
									
									((propActionRequestMachineData*)pConfigTemp)->ConfigurationID = pChartoUint32((ezxml_child(action_x, "ConfigurationID")->txt));	
									((propActionRequestMachineData*)pConfigTemp)->Name 						= ezxml_child(action_x, "Name")->txt;
									((propActionRequestMachineData*)pConfigTemp)->TargetValue 		= pChartoUint32((ezxml_child(action_x, "TargetValue")->txt));
									((propActionRequestMachineData*)pConfigTemp)->HighestTargetValueAdjust = pChartoUint32((ezxml_child(action_x, "HighestTargetValueAdjust")->txt));
																		
								}
								else if(strcmp(action_x->name, str_ActionSetOutput) == 0)
								{
printf("{TestFlag8}: Checking whether into ActionSetOutput \n");

									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].ActionType 		= ActSetOutput;
									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig 	= (char*)mymalloc(SRAMEX, sizeof(propActionSetOutput));
									pConfigTemp = ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig;
									
									((propActionSetOutput*)pConfigTemp)->ConfigurationID 	= pChartoUint32((ezxml_child(action_x, "ConfigurationID")->txt));
									((propActionSetOutput*)pConfigTemp)->Name 						= ezxml_child(action_x, "Name")->txt;
									((propActionSetOutput*)pConfigTemp)->TargetValue 			= pChartoUint32((ezxml_child(action_x, "TargetValue")->txt));
									((propActionSetOutput*)pConfigTemp)->HighestTargetValueAdjust = pChartoUint32((ezxml_child(action_x, "HighestTargetValueAdjust")->txt));
									((propActionSetOutput*)pConfigTemp)->DigitalOutput 		= pChartoUint32((ezxml_child(action_x, "DigitalOutput")->txt));
									((propActionSetOutput*)pConfigTemp)->OutputDuration 	= pChartoUint32((ezxml_child(action_x, "OutputDuration")->txt));
									((propActionSetOutput*)pConfigTemp)->OutputInvert 		= pChartoBool((ezxml_child(action_x, "OutputInvert")->txt));//�˴���ȡfalse����
								}
								else if(strcmp(action_x->name, str_ActionObjectTakeOver) == 0)
								{
printf("{TestFlag9}: Checking whether into ActionObjectTakeOver \n");
									
									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].ActionType 		= ActObjectTakeOver;
									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig 	= (char*)mymalloc(SRAMEX, sizeof(propActionObjectTakeOver));
									pConfigTemp = ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig;
									
									((propActionObjectTakeOver*)pConfigTemp)->ConfigurationID 					= pChartoUint32((ezxml_child(action_x, "ConfigurationID")->txt));
									((propActionObjectTakeOver*)pConfigTemp)->Name 											= ezxml_child(action_x, "ConfigurationID")->txt;
									((propActionObjectTakeOver*)pConfigTemp)->TargetValue 							= pChartoUint32((ezxml_child(action_x, "TargetValue")->txt));
									((propActionObjectTakeOver*)pConfigTemp)->HighestTargetValueAdjust	= pChartoUint32((ezxml_child(action_x, "HighestTargetValueAdjust")->txt));
									((propActionObjectTakeOver*)pConfigTemp)->DestinationModule 				= pChartoUint32((ezxml_child(action_x, "DestinationModule")->txt));										
									
								}
								else if(strcmp(action_x->name, str_ActionTriggerCamera) == 0)
								{
printf("{TestFlag10}: Checking whether into ActionTriggerCamera \n");

									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].ActionType 		= ActTriggerCamera;
									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig 	= (char*)mymalloc(SRAMEX, sizeof(propActionTriggerCamera));
									pConfigTemp = ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig;
									
									((propActionTriggerCamera*)pConfigTemp)->ConfigurationID 	= pChartoUint32((ezxml_child(action_x, "ConfigurationID")->txt));
									((propActionTriggerCamera*)pConfigTemp)->Name 						= ezxml_child(action_x, "ConfigurationID")->txt;
									((propActionTriggerCamera*)pConfigTemp)->TargetValue 			= pChartoUint32((ezxml_child(action_x, "TargetValue")->txt));
									((propActionTriggerCamera*)pConfigTemp)->HighestTargetValueAdjust = pChartoUint32((ezxml_child(action_x, "HighestTargetValueAdjust")->txt));
									((propActionTriggerCamera*)pConfigTemp)->ClientID 				= pChartoUint32((ezxml_child(action_x, "ClientID")->txt));
									((propActionTriggerCamera*)pConfigTemp)->CameraID 				= pChartoUint32((ezxml_child(action_x, "CameraID")->txt));
									((propActionTriggerCamera*)pConfigTemp)->ImageIndex 			= pChartoUint32((ezxml_child(action_x, "CameraID")->txt));
									((propActionTriggerCamera*)pConfigTemp)->DigitalOutput 		= pChartoUint32((ezxml_child(action_x, "DigitalOutput")->txt));
									((propActionTriggerCamera*)pConfigTemp)->OutputDuration 	= pChartoUint32((ezxml_child(action_x, "OutputDuration")->txt));					
									
									
								}
								else if(strcmp(action_x->name, str_ActionTriggerSensor) == 0)
								{
printf("{TestFlag11}: Checking whether into ActionTriggerSensor \n");

									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].ActionType = ActTriggerSensor;
									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig 	= (char*)mymalloc(SRAMEX, sizeof(propActionTriggerSensor));
									pConfigTemp = ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig;
									
									((propActionTriggerSensor*)pConfigTemp)->ConfigurationID 	= pChartoUint32((ezxml_child(action_x, "ConfigurationID")->txt));
									((propActionTriggerSensor*)pConfigTemp)->Name 						= ezxml_child(action_x, "ConfigurationID")->txt;
									((propActionTriggerSensor*)pConfigTemp)->TargetValue 			= pChartoUint32((ezxml_child(action_x, "TargetValue")->txt));
									((propActionTriggerSensor*)pConfigTemp)->HighestTargetValueAdjust = pChartoUint32((ezxml_child(action_x, "HighestTargetValueAdjust")->txt));
									((propActionTriggerSensor*)pConfigTemp)->ClientID 				= pChartoUint32((ezxml_child(action_x, "ClientID")->txt));
									((propActionTriggerSensor*)pConfigTemp)->SensorID 				= pChartoUint32((ezxml_child(action_x, "SensorID")->txt));						
						
								}
								else if(strcmp(action_x->name, str_ActionPushOut) == 0)
								{
printf("{TestFlag12}: Checking whether into ActionPushOut \n");
								//�ӱ�־λ����ȷ��ActionInstanceConfig�����ĵ������ĸ�����
									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].ActionType 		= ActPushOut;
									ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig 	= (char*)mymalloc(SRAMEX, sizeof(propActionPushOut));
									pConfigTemp = ModuleConfig[Module_Count].ActionInstanceConfig[j_cycle].pActionConfig;	
									
									((propActionPushOut*)pConfigTemp)->ConfigurationID 				= pChartoUint32((ezxml_child(action_x, "ConfigurationID")->txt));
									((propActionPushOut*)pConfigTemp)->Name 									= ezxml_child(action_x, "ConfigurationID")->txt;
									((propActionPushOut*)pConfigTemp)->TargetValue 						= pChartoUint32((ezxml_child(action_x, "TargetValue")->txt));
									((propActionPushOut*)pConfigTemp)->HighestTargetValueAdjust = pChartoUint32((ezxml_child(action_x, "HighestTargetValueAdjust")->txt));
									((propActionPushOut*)pConfigTemp)->Active 								= pChartoBool((ezxml_child(action_x, "Active")->txt));
									((propActionPushOut*)pConfigTemp)->DigitalOutput 					= pChartoUint32((ezxml_child(action_x, "DigitalOutput")->txt));
									((propActionPushOut*)pConfigTemp)->PushLength 						= pChartoUint32((ezxml_child(action_x, "PushLength")->txt));
									((propActionPushOut*)pConfigTemp)->PushLine 							= pChartoUint32((ezxml_child(action_x, "PushLine")->txt));
									((propActionPushOut*)pConfigTemp)->MinResultNum 					= pChartoUint32((ezxml_child(action_x, "MinResultNum")->txt));
										
									
								}
								
								ModuleConfig[Module_Count].NumberOfActions++;//ͳ�Ƶ���Module��Ķ����������ⲿ&ȫ��ʹ�ã�
								j_cycle++;					//ͳ�Ƶ���Module��Ķ�������(�ڲ�ʹ��)
							}
						j_cycle = 0;						//!!!!!��һ�����ٶα���Ҫ����j_cycle������������һֱ�ڵ�����........���º��涯���ŵ�λ�ò���........
						}	
					}
				
				Module_Count++;							//ͳ��Module �ĸ���
				}					
			}
			ezxml_free(track_x);					//�ͷŶ���


		}
	vTaskDelay(5000);									//5sɨ��һ���Ƿ���Ҫ���ز���
	}	
}

