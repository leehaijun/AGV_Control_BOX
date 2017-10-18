#pragma once
/*
* ���ڲ�ѯ����
* �����жϽ���
* ����ʹ��ModbusЭ��
*/
#include <stm32f0xx_usart.h>
#include <stm32f0xx_gpio.h>
#include <stm32f0xx_misc.h>
#include <stm32f0xx_rcc.h>

#define UART_RX_BUFF_SIZE 64 //���ڽ��ܻ�������С

extern unsigned char Uart_RX_BUF[UART_RX_BUFF_SIZE];
extern unsigned char rx_buf_cnt; //���ڽ��ջ������ֽ����ļ���

void Uart_Init(uint32_t baudrate);
void uart1_send(const char *p, int length);	//���͹̶����ȵ��ַ�����
void uart1_send_byte(const char p);			//����һ���ַ�����
void uart1_send_str(const char *_str);		//����һ���ַ���

