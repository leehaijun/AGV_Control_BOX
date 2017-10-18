#include "74HC595.h"

void HC595_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_HC595, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_InitStructure.GPIO_Pin = RCLK_PIN;
	GPIO_Init(RCLK_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SRCLK_PIN;
	GPIO_Init(SRCLK_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SER_PIN;
	GPIO_Init(SER_PORT, &GPIO_InitStructure);

	RCLK_PORT->BRR = RCLK_PIN;
	SRCLK_PORT->BRR = SRCLK_PIN;
	SER_PORT->BRR = SER_PIN;
}

void HC595_Write_Byte(const uint8_t Byte)
{
	for (int8_t i = 7; i >= 0; i--)
	{
		if ((Byte >> i) & 0x01)
		{
			SER_PORT->BSRR = SER_PIN;
		}
		else
		{
			SER_PORT->BRR = SER_PIN;
		}
		SRCLK_PORT->BSRR = SRCLK_PIN;	//Ê±ÖÓ¸ß
		SRCLK_PORT->BRR = SRCLK_PIN;	//Ê±ÖÓµÍ
	}
}

void HC595_Write(const uint8_t * source, uint8_t number)
{
	while (number)
	{
		HC595_Write_Byte(*source);
		source++;
		number--;
	}
}

void HC595_Refresh(void)
{
	RCLK_PORT->BSRR = RCLK_PIN;
	RCLK_PORT->BRR = RCLK_PIN;
}
