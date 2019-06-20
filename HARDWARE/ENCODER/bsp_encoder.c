/***
*		Using Timer3&TImer8 to Count Encoder
*/
#include "bsp_encoder.h"

int32_t OverflowCount_Encoder3 = 0;	//��ʱ��3�������
int32_t OverflowCount_Encoder8 = 0;	//��ʱ��2�������
/* Timer handler declaration */
TIM_HandleTypeDef    htimx_Encoder3;		//ʹ�ö�ʱ��3
TIM_HandleTypeDef    htimx_Encoder8;		//ʹ�ö�ʱ��8

/**
  * ��������: ������ʱ��Ӳ����ʼ���ص���������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�HAL_TIM_Encoder_Init(&htimx_Encoder, &sEncoderConfig)����
  */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_base)
{
 GPIO_InitTypeDef GPIO_InitStruct;
	
	if(htim_base == &htimx_Encoder3)
	{		
		/* ������ʱ������ʱ��ʹ�� */
		__HAL_RCC_GPIOA_CLK_ENABLE();//TIM3_CH1 PA6
		__HAL_RCC_GPIOB_CLK_ENABLE();//TIM3_CH2 PB5

    /* ��ʱ��ͨ��1 PA6��������IO��ʼ��PA6&PB5 */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* ��ʱ��ͨ��2 PB5��������IO��ʼ�� */
		GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
	
  if(htim_base == &htimx_Encoder8)
  {
    /* ������ʱ������ʱ��ʹ�� */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* ��ʱ��ͨ��1��������IO��ʼ��PC6&PC7 */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* ��ʱ��ͨ��2��������IO��ʼ�� */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  } 
}

/**
  * ��������: �߼���ʱ��1��ʼ��������Encoderģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void ENCODER_TIM3_Init(void)
{ 
  /* Timer Encoder Configuration Structure declaration */
  TIM_Encoder_InitTypeDef sEncoderConfig;

  __HAL_RCC_TIM3_CLK_ENABLE();
  htimx_Encoder3.Instance = TIM3;//TIM3
  htimx_Encoder3.Init.Prescaler = 0;
  htimx_Encoder3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx_Encoder3.Init.Period = 0xFFFF;//16λ��ʱ��
  htimx_Encoder3.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;

  sEncoderConfig.EncoderMode        = TIM_ENCODERMODE_TI1;
	
  sEncoderConfig.IC1Polarity        = TIM_ICPOLARITY_RISING;   
  sEncoderConfig.IC1Selection       = TIM_ICSELECTION_DIRECTTI;  
  sEncoderConfig.IC1Prescaler       = TIM_ICPSC_DIV1; 
  sEncoderConfig.IC1Filter          = 0;
  
  sEncoderConfig.IC2Polarity        = TIM_ICPOLARITY_RISING;   
  sEncoderConfig.IC2Selection       = TIM_ICSELECTION_DIRECTTI;  
  sEncoderConfig.IC2Prescaler       = TIM_ICPSC_DIV1; 
  sEncoderConfig.IC2Filter          = 0;
  __HAL_TIM_SET_COUNTER(&htimx_Encoder3,0);								//����ֵ����
  
  HAL_TIM_Encoder_Init(&htimx_Encoder3, &sEncoderConfig);//Initializes the TIM Encoder Interface and create the associated handle
	
	__HAL_TIM_ENABLE(&htimx_Encoder3);//�򿪶�ʱ��
	
  __HAL_TIM_CLEAR_IT(&htimx_Encoder3, TIM_IT_UPDATE);  // ��������жϱ�־λ
  __HAL_TIM_URS_ENABLE(&htimx_Encoder3);               // ���������������Ų��������ж�
  __HAL_TIM_ENABLE_IT(&htimx_Encoder3,TIM_IT_UPDATE);  // ʹ�ܸ����ж�
  
  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);			//��ʱ��3��������ж�
  HAL_NVIC_EnableIRQ(TIM3_IRQn);							
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{	
////	printf("Encoder Number Overflowed \n");
	if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htimx_Encoder3))
    OverflowCount_Encoder3--;       //���¼������
  else
    OverflowCount_Encoder3++;       //���ϼ������ 
	
	__HAL_TIM_CLEAR_FLAG(&htimx_Encoder3, TIM_FLAG_UPDATE);				//����жϱ�־λ
}


/**
  * ��������: �߼���ʱ��8��ʼ��������Encoderģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void ENCODER_TIM8_Init(void)
{ 
  /* Timer Encoder Configuration Structure declaration */
  TIM_Encoder_InitTypeDef sEncoderConfig;

  __HAL_RCC_TIM8_CLK_ENABLE();
  htimx_Encoder8.Instance = TIM8;//TIM8
  htimx_Encoder8.Init.Prescaler = 0;
  htimx_Encoder8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx_Encoder8.Init.Period = 0xFFFF;//16λ��ʱ��
  htimx_Encoder8.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;

  sEncoderConfig.EncoderMode        = TIM_ENCODERMODE_TI1;
	
  sEncoderConfig.IC1Polarity        = TIM_ICPOLARITY_RISING;   
  sEncoderConfig.IC1Selection       = TIM_ICSELECTION_DIRECTTI;  
  sEncoderConfig.IC1Prescaler       = TIM_ICPSC_DIV1; 
  sEncoderConfig.IC1Filter          = 0;
  
  sEncoderConfig.IC2Polarity        = TIM_ICPOLARITY_RISING;   
  sEncoderConfig.IC2Selection       = TIM_ICSELECTION_DIRECTTI;  
  sEncoderConfig.IC2Prescaler       = TIM_ICPSC_DIV1; 
  sEncoderConfig.IC2Filter          = 0;
  __HAL_TIM_SET_COUNTER(&htimx_Encoder8,0);								//����ֵ����
  
  HAL_TIM_Encoder_Init(&htimx_Encoder8, &sEncoderConfig);//Initializes the TIM Encoder Interface and create the associated handle

	__HAL_TIM_ENABLE(&htimx_Encoder8);//�򿪶�ʱ��
	
  __HAL_TIM_CLEAR_IT(&htimx_Encoder8, TIM_IT_UPDATE);  // ��������жϱ�־λ
  __HAL_TIM_URS_ENABLE(&htimx_Encoder8);               // ���������������Ų��������ж�
  __HAL_TIM_ENABLE_IT(&htimx_Encoder8,TIM_IT_UPDATE);  // ʹ�ܸ����ж�
  
  HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 0, 0);			//��ʱ��1��������ж�
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);							
}

//��ʱ��8�жϷ�����
void TIM8_UP_TIM13_IRQHandler(void)
{	
////	printf("Encoder Number Overflowed \n");
	if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htimx_Encoder8))
    OverflowCount_Encoder8--;       //���¼������
  else
    OverflowCount_Encoder8++;       //���ϼ������ 
	
	__HAL_TIM_CLEAR_FLAG(&htimx_Encoder8, TIM_FLAG_UPDATE);				//����жϱ�־λ
}

