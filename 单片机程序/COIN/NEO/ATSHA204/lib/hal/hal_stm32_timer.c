
#include "stm32f4xx_hal.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//����9M�ļ���Ƶ��
//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void __delay_us(uint32_t nus)
{		
    uint32_t dwCurCounter=0;                                //��ǰʱ�����ֵ
    uint32_t dwPreTickVal=SysTick->VAL;                     //��һ��SYSTICK����ֵ
    uint32_t dwCurTickVal;                                  //��һ��SYSTICK����ֵ
    nus=nus*(HAL_RCC_GetHCLKFreq()/1000000);    			//����ʱʱ�䣬������ʱ�����
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


void atca_delay_ms(uint16_t delay)//��__delay_ms����һ��
{	
			HAL_Delay(delay);
}

