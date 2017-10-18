#pragma once
#ifndef _DELAY_H
#define _DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f0xx.h>
#include <stm32f0xx_misc.h>

void delay_init(void);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#ifdef __cplusplus
}
#endif

#endif