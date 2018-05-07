#ifndef _TIMER_H
#define _TIMER_H
	
#include "stm32f4xx_hal.h"	
	
#ifdef __cplusplus
 extern "C" {
#endif	
	
extern TIM_HandleTypeDef TIM3_Handler;      //¶¨Ê±Æ÷¾ä±ú 
void TIM3_Init(uint16_t arr,uint16_t psc);
void Start_TIM(uint8_t type);
void Stop_TIM(uint8_t type);	 
int Get_TIM(uint8_t type);
	 
#ifdef __cplusplus
 }
#endif
	 
	 
#endif

