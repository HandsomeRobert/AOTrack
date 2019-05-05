#ifndef __BSP_ENCODER_H__
#define __BSP_ENCODER_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f7xx.h"
#include "core_cm7.h"
#include "stm32f7xx_hal.h"

/* ��չ���� ------------------------------------------------------------------*/
#define ENCODER_CNT_MAX 0xFFFF

extern TIM_HandleTypeDef htimx_Encoder3;
extern TIM_HandleTypeDef htimx_Encoder8;
extern int32_t OverflowCount_Encoder3;	//��ʱ��3�������
extern int32_t OverflowCount_Encoder8;	//��ʱ��8�������
/* �������� ------------------------------------------------------------------*/
void ENCODER_TIM3_Init(void);
void ENCODER_TIM8_Init(void);
#endif	/* __ENCODER_TIM_H__ */
