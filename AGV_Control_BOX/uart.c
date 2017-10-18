#include "uart.h"

unsigned char Uart_RX_BUF[UART_RX_BUFF_SIZE] = { 0 };

unsigned char rx_buf_cnt=0; //���ڽ��ջ������ֽ����ļ���

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

	//Usart1 NVIC �ж����� ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�
	USART_Cmd(USART1, ENABLE); //ʹ�ܴ���


}

//************************************
// Method:    uart1_send
// FullName:  uart1_send
// Access:    public 
// Returns:   void
// Parameter: const char * p
// Parameter: int length
// Description:	����ָ�����ȵ��ֽ�
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
// Description:	ʹ��usart1����һ���ֽ�
//************************************
void uart1_send_byte(const char p)
{
	while (!((USART1->ISR)&USART_FLAG_TXE));		//�������ݼĴ�����
	USART1->TDR = p;
}

//************************************
// Method:    uart1_send_str
// FullName:  uart1_send_str
// Access:    public 
// Returns:   void
// Parameter: const char * _str
// Description:	�����ַ���
//************************************
void uart1_send_str(const char * _str)
{
	while (*_str)
	{
		uart1_send_byte(*_str);
		_str++;
	}
}