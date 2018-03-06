
#include "stm32f4xx_hal.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//采用9M的计数频率
//延时nus
//nus为要延时的us数.		    								   
void __delay_us(uint32_t nus)
{		
    uint32_t dwCurCounter=0;                                //当前时间计数值
    uint32_t dwPreTickVal=SysTick->VAL;                     //上一次SYSTICK计数值
    uint32_t dwCurTickVal;                                  //上一次SYSTICK计数值
    nus=nus*(HAL_RCC_GetHCLKFreq()/1000000);    			//需延时时间，共多少时间节拍
    for(;;)
		{
        dwCurTickVal=SysTick->VAL;
        if(dwCurTickVal<dwPreTickVal)
				{
            dwCurCounter=dwCurCounter+dwPreTickVal-dwCurTickVal;
        }
        else
				{
            dwCurCounter=dwCurCounter+dwPreTickVal+SysTick->LOAD-dwCurTickVal;
        }
        dwPreTickVal=dwCurTickVal;
        if(dwCurCounter>=nus)
				{
            return;
        }
    }
}


void atca_delay_ms(uint16_t delay)//跟__delay_ms函数一样
{	
			HAL_Delay(delay);
}

