#ifndef _TIMER_H
#define _TIMER_H
	
#include "stm32f4xx_hal.h"	
	
#ifdef __cplusplus
 extern "C" {
#endif	
	
extern TIM_HandleTypeDef TIM3_Handler;      //¶¨Ê±Æ÷¾ä±ú 
void TIM3_Init(uint16_t arr,uint16_t psc);
void Start_TIM(void);
void Stop_TIM(void);	 
int Get_TIM(void);	 
	 
#ifdef __cplusplus
 }
#endif
	 
	 
#endif

