#ifndef __HARDWARELIST_H
#define __HARDWARELIST_H
/**************************************************************************************************
************************************本文件用于统一管理各硬件资源的代号定义*************************
***************************************************************************************************/

#define LightSignal_1 		1							//光电PA5均接入定时器，来实现采集，但目前只初始化定时器2-5
#define LightSignal_2 		2							//PH10
#define LightSignal_3 		3							//PB7
#define LightSignal_4 		4							//PE5
#define LightSignal_5 		5							//PB8
#define LightSignal_6 		6							//PB9
#define LightSignal_7 		7							//PH6

#define Encoder_1 				8							//编码器TIM8 PC6&PC7
#define Encoder_2 				9							//编码器TIM1 PB13&PB14

#define DigitalOutput_1		10						//数字输出PH7
#define DigitalOutput_2		11						//PH8
#define DigitalOutput_3		12						//PH9			
#define DigitalOutput_4		13						//PH11			
#define DigitalOutput_5		14						//PH12		
#define DigitalOutput_6		15						//PH13			
#define DigitalOutput_7		16						//PH14			
#define DigitalOutput_8		17						//PH15			

#define DigitalInput_1		18						//数字输入 暂未使用，因为全在光电，定时器中已有输入
#define DigitalInput_2		19						//
#define DigitalInput_3		20						//
#define DigitalInput_4		21						//
#define DigitalInput_5		22
#define DigitalInput_6		23
#define DigitalInput_7		24	
#define DigitalInput_8		25


#endif
