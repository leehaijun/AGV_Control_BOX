#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>

void Delay()
{
	int i;
	for (i = 0; i < 1000000; i++)
		asm("nop");
}

int main()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  for (;;)
  {
	  GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_SET);
	  Delay();
	  GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_RESET);
	  Delay();
  }
}