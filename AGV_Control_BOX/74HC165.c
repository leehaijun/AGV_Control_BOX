#include "74HC165.h"

void HC165_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_HC165, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_InitStructure.GPIO_Pin = CP_PIN;
	GPIO_Init(CP_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PL_PIN;
	GPIO_Init(PL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = Q7_PIN;
	GPIO_Init(Q7_PORT, &GPIO_InitStructure);

	CP_PORT->BRR = CP_PIN;	//Çå0
	PL_PORT->BSRR = PL_PIN;	//ÖÃ1
}

uint32_t HC165_Read(uint8_t bit_num)
{
	uint32_t temp = 0;
	PL_PORT->BRR = PL_PIN;
	//asm("nop");
	PL_PORT->BSRR = PL_PIN;		//Ëø´æ

	for (int8_t i = 0; i < bit_num; i++)
	{
		temp <<= 1;
		if (((CP_PORT->IDR)&Q7_PIN))
		{
			temp |= 0x01;
		}
		CP_PORT->BSRR = CP_PIN;
		//asm("nop");
		CP_PORT->BRR = CP_PIN;
	}
	return temp;
}
