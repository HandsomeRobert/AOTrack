#ifndef _LIGHTSIGNAL_H
#define _LIGHTSIGNAL_H
#include "sys.h"


extern TIM_HandleTypeDef TIM10_Handler;      				//��ʱ��10���
extern u8  TIM2CH1_CAPTURE_STA;											//���벶��״̬		    				
extern u32	TIM2CH1_CAPTURE_VAL;										//���벶��ֵ(TIM2��32λ)
extern u8  TIM5CH1_CAPTURE_STA;											//���벶��״̬		    				
extern u32	TIM5CH1_CAPTURE_VAL;										//���벶��ֵ(TIM5��32λ)

extern u8  TIM3CH1_CAPTURE_STA;											//���벶��״̬		    				
extern u16	TIM3CH1_CAPTURE_VAL;										//���벶��ֵ(TIM3��16λ)
extern u8  TIM4CH1_CAPTURE_STA;											//���벶��״̬		    				
extern u16	TIM4CH1_CAPTURE_VAL;										//���벶��ֵ(TIM4��16λ)

void TIM2_CH1_Cap_Init(u32 arr,u16 psc); 					//��ʼ��TIM2_CH1���벶��
void TIM5_CH1_Cap_Init(u32 arr,u16 psc); 					//��ʼ��TIM5_CH1���벶��

#endif

