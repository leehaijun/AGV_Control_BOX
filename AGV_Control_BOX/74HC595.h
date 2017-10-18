#pragma once

#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>

/*
���벢��оƬ74HC595
RCLK	����Ĵ��������ź�	������ʱ����
SRCLK	����ʱ��	������ʱ����
SER		�����ź�����	Q7����
*/

//�ڱ�Ӧ���У�bit7������Power_LED
#define RCLK_PORT	GPIOA
#define RCLK_PIN	GPIO_Pin_6

#define SRCLK_PORT	GPIOA
#define SRCLK_PIN	GPIO_Pin_7

#define SER_PORT	GPIOA
#define SER_PIN		GPIO_Pin_5

#define RCC_AHBPeriph_HC595	RCC_AHBPeriph_GPIOA

//��ʼ��74HC595��IO
void HC595_Init(void);	
//ʹ��74HC595дһ���ֽڣ��ȷ��͸�λ
//bit7��ӦQ7��bit0��ӦQ0
void HC595_Write_Byte(const uint8_t Byte);
//ʹ��74HC595дָ������number���ַ����飬source�Ǹ��ַ�������׵�ַ
void HC595_Write(const uint8_t *source, uint8_t number);
//����λ�Ĵ����ڵ�����������˿�
void HC595_Refresh(void);