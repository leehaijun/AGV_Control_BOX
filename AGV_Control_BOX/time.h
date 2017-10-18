#pragma once
#include <stm32f0xx_tim.h>
#include <stm32f0xx_misc.h>
#include <stm32f0xx_rcc.h>


void Init_TIM3(uint16_t arr, uint16_t psc);
void Init_TIM14(uint16_t arr, uint16_t psc);
void Init_TIM16(uint16_t arr, uint16_t psc);

//根据arr（计数装载值）,psc（分频系数）,NVIC_IRQChannelPriority（中断优先级）设置定时器TIMx
void Init_TIM(uint16_t arr, uint16_t psc, TIM_TypeDef* TIMx,uint8_t NVIC_IRQChannelPriority);
