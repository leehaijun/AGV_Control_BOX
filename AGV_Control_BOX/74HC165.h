#pragma once

#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>

/*
���봮���߼�оƬ74HC165
CP	������λʱ��	�����ز���
PL	��������ʱ��	�͵�ƽ����
Q7	�����������	D7�ȳ�
*/

//�ڱ�Ӧ���У�����˳��Ϊ���ذ�ť�����а�ť���������������첦��14���ٶ����������1-8
//�ڱ�Ӧ���У�������ֵȫΪ���࣬���������¼�⵽Ϊ1�����뿪�ز���ONΪ1

#define CP_PORT	GPIOA
#define CP_PIN	GPIO_Pin_2

#define PL_PORT	GPIOA
#define PL_PIN	GPIO_Pin_1

#define Q7_PORT	GPIOA
#define Q7_PIN	GPIO_Pin_3

#define RCC_AHBPeriph_HC165	RCC_AHBPeriph_GPIOA

//��ʼ��74HC165
void HC165_Init(void);

//ʹ��74HC165��ȡָ��������λ���ȶ�ȡ��λ
uint32_t HC165_Read(uint8_t bit_num);