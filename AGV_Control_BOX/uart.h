#pragma once
/*
* 串口查询发送
* 串口中断接收
* 接收使用Modbus协议
*/
#include <stm32f0xx_usart.h>
#include <stm32f0xx_gpio.h>
#include <stm32f0xx_misc.h>
#include <stm32f0xx_rcc.h>

#define UART_RX_BUFF_SIZE 64 //串口接受缓冲区大小

extern unsigned char Uart_RX_BUF[UART_RX_BUFF_SIZE];
extern unsigned char rx_buf_cnt; //串口接收缓冲区字节数的计数

void Uart_Init(uint32_t baudrate);
void uart1_send(const char *p, int length);	//发送固定长度的字符数组
void uart1_send_byte(const char p);			//发送一个字符变量
void uart1_send_str(const char *_str);		//发送一个字符串

