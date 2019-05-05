#ifndef __BSP_ENCODER_H__
#define __BSP_ENCODER_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f7xx.h"
#include "core_cm7.h"
#include "stm32f7xx_hal.h"

/* 扩展变量 ------------------------------------------------------------------*/
#define ENCODER_CNT_MAX 0xFFFF

extern TIM_HandleTypeDef htimx_Encoder3;
extern TIM_HandleTypeDef htimx_Encoder8;
extern int32_t OverflowCount_Encoder3;	//定时器3溢出次数
extern int32_t OverflowCount_Encoder8;	//定时器8溢出次数
/* 函数声明 ------------------------------------------------------------------*/
void ENCODER_TIM3_Init(void);
void ENCODER_TIM8_Init(void);
#endif	/* __ENCODER_TIM_H__ */
