#include "timer.h"

TIM_HandleTypeDef TIM6_Handler;      	//��ʱ����� 
int32_t OverflowCount_TIM6 = 0;		//��ʱ���������ֵ
//ͨ�ö�ʱ��6�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��6!(��ʱ��3����APB1�ϣ�ʱ��ΪHCLK/2)
void TIM6_Init(u16 arr,u16 psc)
{
  	__HAL_RCC_TIM6_CLK_ENABLE(); 
    TIM6_Handler.Instance=TIM6;                          		//ͨ�ö�ʱ��6
    TIM6_Handler.Init.Prescaler=psc;                     		//��Ƶ
    TIM6_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    		//���ϼ�����
    TIM6_Handler.Init.Period=arr;                        		//�Զ�װ��ֵ
    TIM6_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//ʱ�ӷ�Ƶ����
    //HAL_TIM_Base_Init(&TIM6_Handler);
    
		//ʹ��TIM6ʱ��
		__HAL_TIM_SET_COUNTER(&TIM6_Handler, 0);								//����ֵ����

//    HAL_TIM_Base_Start_IT(&TIM6_Handler); 				//ʹ�ܶ�ʱ��6�Ͷ�ʱ��6�����жϣ�TIM_IT_UPDATE   
		__HAL_TIM_ENABLE(&TIM6_Handler);//�򿪶�ʱ��

	  __HAL_TIM_CLEAR_IT(&TIM6_Handler, TIM_IT_UPDATE);  // ��������жϱ�־λ
		__HAL_TIM_URS_ENABLE(&TIM6_Handler);               // ���������������Ų��������ж�
		__HAL_TIM_ENABLE_IT(&TIM6_Handler,TIM_IT_UPDATE);  // ʹ�ܸ����ж�
	
           			
		HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 3, 0);    //�����ж����ȼ�����ռ���ȼ�3�������ȼ�0
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);          	//����ITM6�ж�   
}

//��ʱ��6�жϷ�����
void TIM6_DAC_IRQHandler(void)
{
    __HAL_TIM_CLEAR_FLAG(&TIM6_Handler, TIM_FLAG_UPDATE);				//����жϱ�־λ
}


