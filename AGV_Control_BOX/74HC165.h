#pragma once

#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>

/*
并入串出逻辑芯片74HC165
CP	串行移位时钟	上升沿采样
PL	输入锁存时钟	低电平锁存
Q7	串行数据输出	D7先出
*/

//在本应用中，读出顺序为返回按钮，呼叫按钮，保留、保留、红拨码14，再读出蓝拨码的1-8
//在本应用中，读出的值全为反相，即按键按下检测到为1，拨码开关拨到ON为1

#define CP_PORT	GPIOA
#define CP_PIN	GPIO_Pin_2

#define PL_PORT	GPIOA
#define PL_PIN	GPIO_Pin_1

#define Q7_PORT	GPIOA
#define Q7_PIN	GPIO_Pin_3

#define RCC_AHBPeriph_HC165	RCC_AHBPeriph_GPIOA

//初始化74HC165
void HC165_Init(void);

//使用74HC165读取指定数量的位，先读取高位
uint32_t HC165_Read(uint8_t bit_num);