#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

extern TIM_HandleTypeDef TIM6_Handler;      //��ʱ��3PWM��� 

void TIM6_Init(u16 arr,u16 psc);    				//��ʱ����ʼ��
#endif

