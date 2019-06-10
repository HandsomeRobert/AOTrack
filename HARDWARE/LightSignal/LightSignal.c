#include "LightSignal.h"
#include "led.h"

/***************************************************************************
****************************************************************************
  下面是输入捕获相关源码实验相关函数源码
****************************************************************************
****************************************************************************/
TIM_HandleTypeDef TIM2_Handler;         //32位定时器2句柄
TIM_HandleTypeDef TIM5_Handler;         //32Bit定时器5句柄

TIM_HandleTypeDef TIM3_Handler;         //16Bit定时器3句柄
//TIM_HandleTypeDef TIM4_Handler;         //16Bit定时器4句柄


//捕获状态！！！！！！！
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于16位定时器来说,1us计数器加1,溢出时间:65毫秒)
u8  TIM2CH1_CAPTURE_STA=0;									//输入捕获状态		    				
u32	TIM2CH1_CAPTURE_VAL;										//输入捕获值(TIM2是32位)
u8  TIM5CH1_CAPTURE_STA=0;									//输入捕获状态		    				
u32	TIM5CH1_CAPTURE_VAL;										//输入捕获值(TIM5是32位)

u8  TIM3CH1_CAPTURE_STA=0;									//输入捕获状态		    				
u16	TIM3CH1_CAPTURE_VAL;										//输入捕获值(TIM3是16位)
u8  TIM4CH1_CAPTURE_STA=0;									//输入捕获状态		    				
u16	TIM4CH1_CAPTURE_VAL;										//输入捕获值(TIM4是16位)

//定时器2&5底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_IC_Init()调用
//htim:定时器句柄
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim == &TIM2_Handler)
	{			
		 /*定时器2底层驱动，时钟使能，引脚配置*/
		GPIO_InitTypeDef GPIO_Initure;		
    __HAL_RCC_TIM2_CLK_ENABLE();            //使能TIM2时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();						//开启GPIOA时钟
	
    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    GPIO_Initure.Alternate=GPIO_AF1_TIM2;   //PA5复用为TIM2通道1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM2_IRQn,2,0);    //设置中断优先级，抢占优先级2，子优先级0
    HAL_NVIC_EnableIRQ(TIM2_IRQn);          //开启ITM2中断 
	}
	
	if(htim == &TIM5_Handler)
	{
	 GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM5_CLK_ENABLE();            //使能TIM5时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();						//开启GPIOH时钟
	
    GPIO_Initure.Pin=GPIO_PIN_10;            //PH10
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    GPIO_Initure.Alternate=GPIO_AF2_TIM5;   //PH10复用为TIM5通道1
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM5_IRQn,2,0);    //设置中断优先级，抢占优先级2，子优先级0
    HAL_NVIC_EnableIRQ(TIM5_IRQn);          //开启ITM5中断   
	}	
}

//定时器2通道1输入捕获配置
//arr：自动重装值(TIM2是32位的!!)
//psc：时钟预分频数
void TIM2_CH1_Cap_Init(u32 arr,u16 psc)
{  
    TIM_IC_InitTypeDef TIM2_CH1Config; 
    
    TIM2_Handler.Instance=TIM2;                          //通用定时器2
    TIM2_Handler.Init.Prescaler=psc;                     //分频
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM2_Handler.Init.Period=arr;                        //自动装载值
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM2_Handler);  

		/*定时器2捕获模式配置*/   
    TIM2_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //上升沿捕获
    TIM2_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
    TIM2_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM2_CH1Config.ICFilter=0;                          //配置输入滤波器，不滤波
    HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CH1Config,TIM_CHANNEL_1);//配置TIM2通道1
    HAL_TIM_IC_Start_IT(&TIM2_Handler,TIM_CHANNEL_1);   //开始捕获TIM2的通道1
    __HAL_TIM_ENABLE_IT(&TIM2_Handler,TIM_IT_UPDATE);   //使能更新中断
}

//定时器2中断服务函数
void TIM2_IRQHandler(void)
{
/***************************************定时器溢出更新中断*******************************************************************/	
	if(__HAL_TIM_GET_FLAG(&TIM2_Handler, TIM_FLAG_UPDATE))		//获得相应的中断标志位，中断事件类型
	{
printf("Value Overflowed TIM2CH1_CAPTURE_STA++\n");																///////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////等候测试！！！////////////////////////////////////////////
		if((TIM2CH1_CAPTURE_STA&0X80)==0)						//还未成功捕获
		{
			if(TIM2CH1_CAPTURE_STA&0X40)							//已经捕获到高电平了
			{
				if((TIM2CH1_CAPTURE_STA&0X3F)==0X3F)		//高电平太长了00111111
				{
					TIM2CH1_CAPTURE_STA|=0X80;						//标记成功捕获了一次
					TIM2CH1_CAPTURE_VAL=0XFFFFFFFF;				//输出所能表示的最大值
				}
				else 
					TIM2CH1_CAPTURE_STA++;
			}	 
		}		
	__HAL_TIM_CLEAR_FLAG(&TIM2_Handler, TIM_FLAG_UPDATE);				//清除中断标志位
	}
/***************************************END**********************************************************************************/	
	
/***************************************定时器捕获中断***********************************************************************/		
	if(__HAL_TIM_GET_FLAG(&TIM2_Handler, TIM_FLAG_CC1))
	{
//printf("TIM2 SIGNAL 1 TestFlag333\n");	
		if((TIM2CH1_CAPTURE_STA&0X80)==0)							//还未成功捕获
		{
			if(TIM2CH1_CAPTURE_STA&0X40)								//捕获到一个下降沿 		
			{	  			
				TIM2CH1_CAPTURE_STA|=0X80;							//标记成功捕获到一次高电平脉宽
//printf("TestFlag111\n");
				TIM2CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_1);				//获取当前的捕获值.
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1);   												//一定要先清除原来的设置！！
				TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);				//配置TIM2通道1上升沿捕获
			}
			else  //还未开始,第一次捕获上升沿
			{
				TIM2CH1_CAPTURE_STA=0;									//清空
				TIM2CH1_CAPTURE_VAL=0;
				TIM2CH1_CAPTURE_STA|=0X40;							//标记捕获到了上升沿（初始化处已设置为上升沿捕获，所以认定此处为开始捕获处）
//printf("TestFlag222\n");	
				__HAL_TIM_DISABLE(&TIM2_Handler);       //关闭定时器2
				__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);	//计数值清零
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1);   										//一定要先清除原来的设置！！
				TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);	//定时器2通道1设置为下降沿捕获
				__HAL_TIM_ENABLE(&TIM2_Handler);				//使能定时器2
			}		    
		}	
	__HAL_TIM_CLEAR_FLAG(&TIM2_Handler, TIM_FLAG_CC1);				//清除捕获中断标志位
	}
/***************************************END**********************************************************************************/		
}
/***************************************TIM2 Initial END*********************************************************************/	
/****************************************************************************************************************************/	


//定时器5通道1输入捕获配置
//arr：自动重装值(TIM5是32位的!!)
//psc：时钟预分频数
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{  
    TIM_IC_InitTypeDef TIM5_CH1Config;  
	
    TIM5_Handler.Instance=TIM5;                          //通用定时器5
    TIM5_Handler.Init.Prescaler=psc;                     //分频
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM5_Handler.Init.Period=arr;                        //自动装载值
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM5_Handler);  

		/*定时器5捕获模式配置*/
    TIM5_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //上升沿捕获
    TIM5_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
    TIM5_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM5_CH1Config.ICFilter=0;                          //配置输入滤波器，不滤波
    HAL_TIM_IC_ConfigChannel(&TIM5_Handler,&TIM5_CH1Config,TIM_CHANNEL_1);//配置TIM5通道1
    HAL_TIM_IC_Start_IT(&TIM5_Handler,TIM_CHANNEL_1);   //开始捕获TIM5的通道1
    __HAL_TIM_ENABLE_IT(&TIM5_Handler,TIM_IT_UPDATE);   //使能更新中断
}

//定时器5中断服务函数
void TIM5_IRQHandler(void)
{
/***************************************定时器溢出更新中断*******************************************************************/	
	if(__HAL_TIM_GET_FLAG(&TIM5_Handler, TIM_FLAG_UPDATE))		//判断是否发生了溢出更新事件
	{
printf("Value Overflowed TIM5CH1_CAPTURE_STA++\n");																///////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////等候测试！！！////////////////////////////////////////////
		if((TIM5CH1_CAPTURE_STA&0X80)==0)							//还未成功捕获
		{
			if(TIM5CH1_CAPTURE_STA&0X40)							//已经捕获到高电平了
			{
				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)		//高电平太长了00111111
				{
					TIM5CH1_CAPTURE_STA|=0X80;						//标记成功捕获了一次
					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;				//输出所能表示的最大值
				}
				else 
					TIM5CH1_CAPTURE_STA++;
			}	 
		}		
	__HAL_TIM_CLEAR_FLAG(&TIM5_Handler, TIM_FLAG_UPDATE);				//清除中断标志位
	}
/***************************************END**********************************************************************************/	
	
/***************************************定时器捕获中断***********************************************************************/		
	if(__HAL_TIM_GET_FLAG(&TIM5_Handler, TIM_FLAG_CC1))	//判断是否捕获到了脉冲
	{
//printf("TIM5 SIGNAL 2 TestFlag333\n");	
		if((TIM5CH1_CAPTURE_STA&0X80)==0)							//还未成功捕获
		{
			if(TIM5CH1_CAPTURE_STA&0X40)								//捕获到一个下降沿 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;							//标记成功捕获到一次高电平脉宽
//printf("TestFlag111\n");
				TIM5CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM5_Handler,TIM_CHANNEL_1);				//获取当前的捕获值.
				TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);   												//一定要先清除原来的设置！！
				TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);				//配置TIM5通道1上升沿捕获
			}
			else  //还未开始,第一次捕获上升沿
			{
				TIM5CH1_CAPTURE_STA=0;									//清空
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;							//标记捕获到了上升沿（初始化处已设置为上升沿捕获，所以认定此处为开始捕获处）
//printf("TestFlag222\n");	
				__HAL_TIM_DISABLE(&TIM5_Handler);       //关闭定时器5
				__HAL_TIM_SET_COUNTER(&TIM5_Handler,0);	//计数值清零
				TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);   										//一定要先清除原来的设置！！
				TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);	//定时器5通道1设置为下降沿捕获
				__HAL_TIM_ENABLE(&TIM5_Handler);				//使能定时器5
			}		    
		}	
	__HAL_TIM_CLEAR_FLAG(&TIM5_Handler, TIM_FLAG_CC1);				//清除捕获中断标志位
	}
/***************************************END**********************************************************************************/		
}
/***************************************TIM5 Initial END*********************************************************************/	
/****************************************************************************************************************************/	


