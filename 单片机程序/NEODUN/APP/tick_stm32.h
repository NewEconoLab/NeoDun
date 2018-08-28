#ifndef TICK_STM32_H
#define TICK_STM32_H

#include "stm32f4xx_hal.h"

void Start_TIM(uint8_t type);
void Stop_TIM(uint8_t type);	 
int Get_TIM(uint8_t type);

#endif
