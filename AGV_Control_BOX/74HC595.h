#pragma once

#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>

/*
串入并出芯片74HC595
RCLK	输出寄存器锁存信号	上升沿时采样
SRCLK	串行时钟	上升沿时采样
SER		串行信号输入	Q7先入
*/

//在本应用中，bit7代表着Power_LED
#define RCLK_PORT	GPIOA
#define RCLK_PIN	GPIO_Pin_6

#define SRCLK_PORT	GPIOA
#define SRCLK_PIN	GPIO_Pin_7

#define SER_PORT	GPIOA
#define SER_PIN		GPIO_Pin_5

#define RCC_AHBPeriph_HC595	RCC_AHBPeriph_GPIOA

//初始化74HC595的IO
void HC595_Init(void);	
//使用74HC595写一个字节，先发送高位
//bit7对应Q7，bit0对应Q0
void HC595_Write_Byte(const uint8_t Byte);
//使用74HC595写指定数量number的字符数组，source是该字符数组的首地址
void HC595_Write(const uint8_t *source, uint8_t number);
//将移位寄存器内的数据输出到端口
void HC595_Refresh(void);