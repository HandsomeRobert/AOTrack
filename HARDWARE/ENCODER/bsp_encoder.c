/***
*		Using Timer3&TImer8 to Count Encoder
*/
#include "bsp_encoder.h"

int32_t OverflowCount_Encoder3 = 0;	//定时器3溢出次数
int32_t OverflowCount_Encoder8 = 0;	//定时器2溢出次数
/* Timer handler declaration */
TIM_HandleTypeDef    htimx_Encoder3;		//使用定时器3
TIM_HandleTypeDef    htimx_Encoder8;		//使用定时器8

/**
  * 函数功能: 基本定时器硬件初始化回调函数配置
  * 输入参数: htim_base：基本定时器句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部HAL_TIM_Encoder_Init(&htimx_Encoder, &sEncoderConfig)调用
  */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_base)
{
 GPIO_InitTypeDef GPIO_InitStruct;
	
	if(htim_base == &htimx_Encoder3)
	{		
		/* 基本定时器外设时钟使能 */
		__HAL_RCC_GPIOA_CLK_ENABLE();//TIM3_CH1 PA6
		__HAL_RCC_GPIOB_CLK_ENABLE();//TIM3_CH2 PB5

    /* 定时器通道1 PA6功能引脚IO初始化PA6&PB5 */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 定时器通道2 PB5功能引脚IO初始化 */
		GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
	
  if(htim_base == &htimx_Encoder8)
  {
    /* 基本定时器外设时钟使能 */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* 定时器通道1功能引脚IO初始化PC6&PC7 */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* 定时器通道2功能引脚IO初始化 */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  } 
}

/**
  * 函数功能: 高级定时器1初始化并配置Encoder模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void ENCODER_TIM3_Init(void)
{ 
  /* Timer Encoder Configuration Structure declaration */
  TIM_Encoder_InitTypeDef sEncoderConfig;

  __HAL_RCC_TIM3_CLK_ENABLE();
  htimx_Encoder3.Instance = TIM3;//TIM3
  htimx_Encoder3.Init.Prescaler = 0;
  htimx_Encoder3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx_Encoder3.Init.Period = 0xFFFF;//16位定时器
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
  __HAL_TIM_SET_COUNTER(&htimx_Encoder3,0);								//计数值清零
  
  HAL_TIM_Encoder_Init(&htimx_Encoder3, &sEncoderConfig);//Initializes the TIM Encoder Interface and create the associated handle
	
	__HAL_TIM_ENABLE(&htimx_Encoder3);//打开定时器
	
  __HAL_TIM_CLEAR_IT(&htimx_Encoder3, TIM_IT_UPDATE);  // 清除更新中断标志位
  __HAL_TIM_URS_ENABLE(&htimx_Encoder3);               // 仅允许计数器溢出才产生更新中断
  __HAL_TIM_ENABLE_IT(&htimx_Encoder3,TIM_IT_UPDATE);  // 使能更新中断
  
  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);			//定时器3溢出更新中断
  HAL_NVIC_EnableIRQ(TIM3_IRQn);							
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{	
////	printf("Encoder Number Overflowed \n");
	if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htimx_Encoder3))
    OverflowCount_Encoder3--;       //向下计数溢出
  else
    OverflowCount_Encoder3++;       //向上计数溢出 
	
	__HAL_TIM_CLEAR_FLAG(&htimx_Encoder3, TIM_FLAG_UPDATE);				//清除中断标志位
}


/**
  * 函数功能: 高级定时器8初始化并配置Encoder模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void ENCODER_TIM8_Init(void)
{ 
  /* Timer Encoder Configuration Structure declaration */
  TIM_Encoder_InitTypeDef sEncoderConfig;

  __HAL_RCC_TIM8_CLK_ENABLE();
  htimx_Encoder8.Instance = TIM8;//TIM8
  htimx_Encoder8.Init.Prescaler = 0;
  htimx_Encoder8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx_Encoder8.Init.Period = 0xFFFF;//16位定时器
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
  __HAL_TIM_SET_COUNTER(&htimx_Encoder8,0);								//计数值清零
  
  HAL_TIM_Encoder_Init(&htimx_Encoder8, &sEncoderConfig);//Initializes the TIM Encoder Interface and create the associated handle

	__HAL_TIM_ENABLE(&htimx_Encoder8);//打开定时器
	
  __HAL_TIM_CLEAR_IT(&htimx_Encoder8, TIM_IT_UPDATE);  // 清除更新中断标志位
  __HAL_TIM_URS_ENABLE(&htimx_Encoder8);               // 仅允许计数器溢出才产生更新中断
  __HAL_TIM_ENABLE_IT(&htimx_Encoder8,TIM_IT_UPDATE);  // 使能更新中断
  
  HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 0, 0);			//定时器1溢出更新中断
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);							
}

//定时器8中断服务函数
void TIM8_UP_TIM13_IRQHandler(void)
{	
////	printf("Encoder Number Overflowed \n");
	if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htimx_Encoder8))
    OverflowCount_Encoder8--;       //向下计数溢出
  else
    OverflowCount_Encoder8++;       //向上计数溢出 
	
	__HAL_TIM_CLEAR_FLAG(&htimx_Encoder8, TIM_FLAG_UPDATE);				//清除中断标志位
}

