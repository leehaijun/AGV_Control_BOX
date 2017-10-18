#include "time.h"


//************************************
// Method:    Init_TIM3
// FullName:  Init_TIM3
// Access:    public 
// Returns:   void
// Parameter: uint16_t arr	计数器装载值
// Parameter: uint16_t psc	分频器装载值
// Description:	定时器3	用于等待应答超时检测，亮灯时间判断,中断优先级4
//************************************
void Init_TIM3(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = arr - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);	//使能更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 4;

	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3, ENABLE);
}

//************************************
// Method:    Init_TIM14
// FullName:  Init_TIM14
// Access:    public 
// Returns:   void
// Parameter: uint16_t arr	计数器装载值
// Parameter: uint16_t psc	分频器装载值
// Description:	定时器14	用于串口Modbus检测，中断优先级1
//************************************
void Init_TIM14(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);	//使能时钟

	TIM_TimeBaseStructure.TIM_Period = arr - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);	//使能更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;

	NVIC_Init(&NVIC_InitStructure);

	//TIM_Cmd(TIM14, ENABLE);
}

//************************************
// Method:    Init_TIM16
// FullName:  Init_TIM16
// Access:    public 
// Returns:   void
// Parameter: uint16_t arr	计数器装载值
// Parameter: uint16_t psc	计数器分频值
// Description: 定时器16	用于按键扫描，看门狗喂狗,电量指示灯（State_LED），中断优先级3
//************************************
void Init_TIM16(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);	//使能时钟

	TIM_TimeBaseStructure.TIM_Period = arr - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);	//使能更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;

	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM16, ENABLE);
}

//************************************
// Method:    Init_TIM
// FullName:  Init_TIM
// Access:    public 
// Returns:   void
// Parameter: uint16_t arr	计数装载值
// Parameter: uint16_t psc	分频系数
// Parameter: TIM_TypeDef * TIMx	待设置的定时器
// Parameter: uint8_t NVIC_IRQChannelPriority	中断优先级
// Description:	根据arr（计数装载值）,psc（分频系数）,NVIC_IRQChannelPriority（中断优先级）设置定时器TIMx
//************************************
void Init_TIM(uint16_t arr, uint16_t psc, TIM_TypeDef * TIMx, uint8_t NVIC_IRQChannelPriority)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	TIM_TimeBaseStructure.TIM_Period = arr - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	switch ((uint32_t)TIMx)
	{
	case (uint32_t)TIM3:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能时钟
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		break;
	case (uint32_t)TIM14:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);	//使能时钟
		NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
		break;
	case (uint32_t)TIM16:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);	//使能时钟
		NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;
		break;
	default:
		break;
	}

	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);	//使能更新中断

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPriority = NVIC_IRQChannelPriority;

	NVIC_Init(&NVIC_InitStructure);
}