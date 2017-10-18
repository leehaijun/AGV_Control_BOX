#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>
#include "delay.h"
#include "config.h"

uint16_t Init_System(void);
void Init_WDT(void);	//��ʼ��ι�������IO(PA0)

uint16_t DIP_switch;	//���ڱ��沦�뿪�ؼ�ֵ

int main(void)
{
	DIP_switch = Init_System();
}

//************************************
// Method:    Init_System
// FullName:  Init_System
// Access:    public 
// Returns:   uint16_t
// Parameter: void
// Description:	��ʼ��ϵͳ�����ز��뿪�ؼ�ֵ
//************************************
uint16_t Init_System(void)
{
	delay_init();
	Init_WDT();
}

void Init_WDT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
