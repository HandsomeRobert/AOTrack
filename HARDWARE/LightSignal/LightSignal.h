#ifndef _LIGHTSIGNAL_H
#define _LIGHTSIGNAL_H
#include "sys.h"


extern TIM_HandleTypeDef TIM10_Handler;      				//定时器10句柄
extern u8  TIM2CH1_CAPTURE_STA;											//输入捕获状态		    				
extern u32	TIM2CH1_CAPTURE_VAL;										//输入捕获值(TIM2是32位)
extern u8  TIM5CH1_CAPTURE_STA;											//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;										//输入捕获值(TIM5是32位)

extern u8  TIM3CH1_CAPTURE_STA;											//输入捕获状态		    				
extern u16	TIM3CH1_CAPTURE_VAL;										//输入捕获值(TIM3是16位)
extern u8  TIM4CH1_CAPTURE_STA;											//输入捕获状态		    				
extern u16	TIM4CH1_CAPTURE_VAL;										//输入捕获值(TIM4是16位)

void TIM2_CH1_Cap_Init(u32 arr,u16 psc); 					//初始化TIM2_CH1输入捕获
void TIM5_CH1_Cap_Init(u32 arr,u16 psc); 					//初始化TIM5_CH1输入捕获

#endif

