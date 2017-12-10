#ifndef _TIMER_H
#define _TIMER_H
	
#include "stm32f4xx_hal.h"	
	
extern TIM_HandleTypeDef TIM3_Handler;      //¶¨Ê±Æ÷¾ä±ú 
void TIM3_Init(uint16_t arr,uint16_t psc);

#endif

