#include "uart.h"

unsigned char Uart_RX_BUF[UART_RX_BUFF_SIZE] = { 0 };

unsigned char rx_buf_cnt=0; //串口接收缓冲区字节数的计数

void Uart_Init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

	USART_DeInit(USART1);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	//Usart1 NVIC 中断配置 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接收中断
	USART_Cmd(USART1, ENABLE); //使能串口


}

//************************************
// Method:    uart1_send
// FullName:  uart1_send
// Access:    public 
// Returns:   void
// Parameter: const char * p
// Parameter: int length
// Description:	发送指定长度的字节
//************************************
void uart1_send(const char * p, int length)
{
	for (; length > 0; length--)
	{
		uart1_send_byte(*p);
		p++;
	}
}

//************************************
// Method:    uart1_send_byte
// FullName:  uart1_send_byte
// Access:    public 
// Returns:   void
// Parameter: const char p
// Description:	使用usart1发送一个字节
//************************************
void uart1_send_byte(const char p)
{
	while (!((USART1->ISR)&USART_FLAG_TXE));		//发送数据寄存器空
	USART1->TDR = p;
}

//************************************
// Method:    uart1_send_str
// FullName:  uart1_send_str
// Access:    public 
// Returns:   void
// Parameter: const char * _str
// Description:	发送字符串
//************************************
void uart1_send_str(const char * _str)
{
	while (*_str)
	{
		uart1_send_byte(*_str);
		_str++;
	}
}