#include "timer.h"

TIM_HandleTypeDef TIM6_Handler;      	//定时器句柄 
int32_t OverflowCount_TIM6 = 0;		//定时器溢出计数值
//通用定时器6中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器6!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM6_Init(u16 arr,u16 psc)
{
  	__HAL_RCC_TIM6_CLK_ENABLE(); 
    TIM6_Handler.Instance=TIM6;                          		//通用定时器6
    TIM6_Handler.Init.Prescaler=psc;                     		//分频
    TIM6_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    		//向上计数器
    TIM6_Handler.Init.Period=arr;                        		//自动装载值
    TIM6_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//时钟分频因子
    //HAL_TIM_Base_Init(&TIM6_Handler);
    
		//使能TIM6时钟
		__HAL_TIM_SET_COUNTER(&TIM6_Handler, 0);								//计数值清零

//    HAL_TIM_Base_Start_IT(&TIM6_Handler); 				//使能定时器6和定时器6更新中断：TIM_IT_UPDATE   
		__HAL_TIM_ENABLE(&TIM6_Handler);//打开定时器

	  __HAL_TIM_CLEAR_IT(&TIM6_Handler, TIM_IT_UPDATE);  // 清除更新中断标志位
		__HAL_TIM_URS_ENABLE(&TIM6_Handler);               // 仅允许计数器溢出才产生更新中断
		__HAL_TIM_ENABLE_IT(&TIM6_Handler,TIM_IT_UPDATE);  // 使能更新中断
	
           			
		HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 3, 0);    //设置中断优先级，抢占优先级3，子优先级0
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);          	//开启ITM6中断   
}

//定时器6中断服务函数
void TIM6_DAC_IRQHandler(void)
{
    __HAL_TIM_CLEAR_FLAG(&TIM6_Handler, TIM_FLAG_UPDATE);				//清除中断标志位
}


