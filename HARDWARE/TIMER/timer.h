#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

extern TIM_HandleTypeDef TIM6_Handler;      //定时器3PWM句柄 

void TIM6_Init(u16 arr,u16 psc);    				//定时器初始化
#endif

