#include "LightSignal.h"
#include "led.h"

/***************************************************************************
****************************************************************************
  ���������벶�����Դ��ʵ����غ���Դ��
****************************************************************************
****************************************************************************/
TIM_HandleTypeDef TIM2_Handler;         //32λ��ʱ��2���
TIM_HandleTypeDef TIM5_Handler;         //32Bit��ʱ��5���

TIM_HandleTypeDef TIM3_Handler;         //16Bit��ʱ��3���
//TIM_HandleTypeDef TIM4_Handler;         //16Bit��ʱ��4���


//����״̬��������������
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(����16λ��ʱ����˵,1us��������1,���ʱ��:65����)
u8  TIM2CH1_CAPTURE_STA=0;									//���벶��״̬		    				
u32	TIM2CH1_CAPTURE_VAL;										//���벶��ֵ(TIM2��32λ)
u8  TIM5CH1_CAPTURE_STA=0;									//���벶��״̬		    				
u32	TIM5CH1_CAPTURE_VAL;										//���벶��ֵ(TIM5��32λ)

u8  TIM3CH1_CAPTURE_STA=0;									//���벶��״̬		    				
u16	TIM3CH1_CAPTURE_VAL;										//���벶��ֵ(TIM3��16λ)
u8  TIM4CH1_CAPTURE_STA=0;									//���벶��״̬		    				
u16	TIM4CH1_CAPTURE_VAL;										//���벶��ֵ(TIM4��16λ)

//��ʱ��2&5�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_IC_Init()����
//htim:��ʱ�����
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim == &TIM2_Handler)
	{			
		 /*��ʱ��2�ײ�������ʱ��ʹ�ܣ���������*/
		GPIO_InitTypeDef GPIO_Initure;		
    __HAL_RCC_TIM2_CLK_ENABLE();            //ʹ��TIM2ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();						//����GPIOAʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //�����������
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    GPIO_Initure.Alternate=GPIO_AF1_TIM2;   //PA5����ΪTIM2ͨ��1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM2_IRQn,2,0);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM2_IRQn);          //����ITM2�ж� 
	}
	
	if(htim == &TIM5_Handler)
	{
	 GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM5_CLK_ENABLE();            //ʹ��TIM5ʱ��
    __HAL_RCC_GPIOH_CLK_ENABLE();						//����GPIOHʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_10;            //PH10
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //�����������
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    GPIO_Initure.Alternate=GPIO_AF2_TIM5;   //PH10����ΪTIM5ͨ��1
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM5_IRQn,2,0);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM5_IRQn);          //����ITM5�ж�   
	}	
}

//��ʱ��2ͨ��1���벶������
//arr���Զ���װֵ(TIM2��32λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM2_CH1_Cap_Init(u32 arr,u16 psc)
{  
    TIM_IC_InitTypeDef TIM2_CH1Config; 
    
    TIM2_Handler.Instance=TIM2;                          //ͨ�ö�ʱ��2
    TIM2_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM2_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM2_Handler);  

		/*��ʱ��2����ģʽ����*/   
    TIM2_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //�����ز���
    TIM2_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM2_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM2_CH1Config.ICFilter=0;                          //���������˲��������˲�
    HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CH1Config,TIM_CHANNEL_1);//����TIM2ͨ��1
    HAL_TIM_IC_Start_IT(&TIM2_Handler,TIM_CHANNEL_1);   //��ʼ����TIM2��ͨ��1
    __HAL_TIM_ENABLE_IT(&TIM2_Handler,TIM_IT_UPDATE);   //ʹ�ܸ����ж�
}

//��ʱ��2�жϷ�����
void TIM2_IRQHandler(void)
{
/***************************************��ʱ����������ж�*******************************************************************/	
	if(__HAL_TIM_GET_FLAG(&TIM2_Handler, TIM_FLAG_UPDATE))		//�����Ӧ���жϱ�־λ���ж��¼�����
	{
printf("Value Overflowed TIM2CH1_CAPTURE_STA++\n");																///////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////�Ⱥ���ԣ�����////////////////////////////////////////////
		if((TIM2CH1_CAPTURE_STA&0X80)==0)						//��δ�ɹ�����
		{
			if(TIM2CH1_CAPTURE_STA&0X40)							//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM2CH1_CAPTURE_STA&0X3F)==0X3F)		//�ߵ�ƽ̫����00111111
				{
					TIM2CH1_CAPTURE_STA|=0X80;						//��ǳɹ�������һ��
					TIM2CH1_CAPTURE_VAL=0XFFFFFFFF;				//������ܱ�ʾ�����ֵ
				}
				else 
					TIM2CH1_CAPTURE_STA++;
			}	 
		}		
	__HAL_TIM_CLEAR_FLAG(&TIM2_Handler, TIM_FLAG_UPDATE);				//����жϱ�־λ
	}
/***************************************END**********************************************************************************/	
	
/***************************************��ʱ�������ж�***********************************************************************/		
	if(__HAL_TIM_GET_FLAG(&TIM2_Handler, TIM_FLAG_CC1))
	{
//printf("TIM2 SIGNAL 1 TestFlag333\n");	
		if((TIM2CH1_CAPTURE_STA&0X80)==0)							//��δ�ɹ�����
		{
			if(TIM2CH1_CAPTURE_STA&0X40)								//����һ���½��� 		
			{	  			
				TIM2CH1_CAPTURE_STA|=0X80;							//��ǳɹ�����һ�θߵ�ƽ����
//printf("TestFlag111\n");
				TIM2CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_1);				//��ȡ��ǰ�Ĳ���ֵ.
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1);   												//һ��Ҫ�����ԭ�������ã���
				TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);				//����TIM2ͨ��1�����ز���
			}
			else  //��δ��ʼ,��һ�β���������
			{
				TIM2CH1_CAPTURE_STA=0;									//���
				TIM2CH1_CAPTURE_VAL=0;
				TIM2CH1_CAPTURE_STA|=0X40;							//��ǲ����������أ���ʼ����������Ϊ�����ز��������϶��˴�Ϊ��ʼ���񴦣�
//printf("TestFlag222\n");	
				__HAL_TIM_DISABLE(&TIM2_Handler);       //�رն�ʱ��2
				__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);	//����ֵ����
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1);   										//һ��Ҫ�����ԭ�������ã���
				TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);	//��ʱ��2ͨ��1����Ϊ�½��ز���
				__HAL_TIM_ENABLE(&TIM2_Handler);				//ʹ�ܶ�ʱ��2
			}		    
		}	
	__HAL_TIM_CLEAR_FLAG(&TIM2_Handler, TIM_FLAG_CC1);				//��������жϱ�־λ
	}
/***************************************END**********************************************************************************/		
}
/***************************************TIM2 Initial END*********************************************************************/	
/****************************************************************************************************************************/	


//��ʱ��5ͨ��1���벶������
//arr���Զ���װֵ(TIM5��32λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{  
    TIM_IC_InitTypeDef TIM5_CH1Config;  
	
    TIM5_Handler.Instance=TIM5;                          //ͨ�ö�ʱ��5
    TIM5_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM5_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM5_Handler);  

		/*��ʱ��5����ģʽ����*/
    TIM5_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //�����ز���
    TIM5_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM5_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM5_CH1Config.ICFilter=0;                          //���������˲��������˲�
    HAL_TIM_IC_ConfigChannel(&TIM5_Handler,&TIM5_CH1Config,TIM_CHANNEL_1);//����TIM5ͨ��1
    HAL_TIM_IC_Start_IT(&TIM5_Handler,TIM_CHANNEL_1);   //��ʼ����TIM5��ͨ��1
    __HAL_TIM_ENABLE_IT(&TIM5_Handler,TIM_IT_UPDATE);   //ʹ�ܸ����ж�
}

//��ʱ��5�жϷ�����
void TIM5_IRQHandler(void)
{
/***************************************��ʱ����������ж�*******************************************************************/	
	if(__HAL_TIM_GET_FLAG(&TIM5_Handler, TIM_FLAG_UPDATE))		//�ж��Ƿ�������������¼�
	{
printf("Value Overflowed TIM5CH1_CAPTURE_STA++\n");																///////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////�Ⱥ���ԣ�����////////////////////////////////////////////
		if((TIM5CH1_CAPTURE_STA&0X80)==0)							//��δ�ɹ�����
		{
			if(TIM5CH1_CAPTURE_STA&0X40)							//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)		//�ߵ�ƽ̫����00111111
				{
					TIM5CH1_CAPTURE_STA|=0X80;						//��ǳɹ�������һ��
					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;				//������ܱ�ʾ�����ֵ
				}
				else 
					TIM5CH1_CAPTURE_STA++;
			}	 
		}		
	__HAL_TIM_CLEAR_FLAG(&TIM5_Handler, TIM_FLAG_UPDATE);				//����жϱ�־λ
	}
/***************************************END**********************************************************************************/	
	
/***************************************��ʱ�������ж�***********************************************************************/		
	if(__HAL_TIM_GET_FLAG(&TIM5_Handler, TIM_FLAG_CC1))	//�ж��Ƿ񲶻�������
	{
//printf("TIM5 SIGNAL 2 TestFlag333\n");	
		if((TIM5CH1_CAPTURE_STA&0X80)==0)							//��δ�ɹ�����
		{
			if(TIM5CH1_CAPTURE_STA&0X40)								//����һ���½��� 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;							//��ǳɹ�����һ�θߵ�ƽ����
//printf("TestFlag111\n");
				TIM5CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM5_Handler,TIM_CHANNEL_1);				//��ȡ��ǰ�Ĳ���ֵ.
				TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);   												//һ��Ҫ�����ԭ�������ã���
				TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);				//����TIM5ͨ��1�����ز���
			}
			else  //��δ��ʼ,��һ�β���������
			{
				TIM5CH1_CAPTURE_STA=0;									//���
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;							//��ǲ����������أ���ʼ����������Ϊ�����ز��������϶��˴�Ϊ��ʼ���񴦣�
//printf("TestFlag222\n");	
				__HAL_TIM_DISABLE(&TIM5_Handler);       //�رն�ʱ��5
				__HAL_TIM_SET_COUNTER(&TIM5_Handler,0);	//����ֵ����
				TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);   										//һ��Ҫ�����ԭ�������ã���
				TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);	//��ʱ��5ͨ��1����Ϊ�½��ز���
				__HAL_TIM_ENABLE(&TIM5_Handler);				//ʹ�ܶ�ʱ��5
			}		    
		}	
	__HAL_TIM_CLEAR_FLAG(&TIM5_Handler, TIM_FLAG_CC1);				//��������жϱ�־λ
	}
/***************************************END**********************************************************************************/		
}
/***************************************TIM5 Initial END*********************************************************************/	
/****************************************************************************************************************************/	


