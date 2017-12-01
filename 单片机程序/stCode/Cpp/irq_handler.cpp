#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "Hal.h"
#include "OLED281/oled281.h"
#include <string.h>

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern UART_HandleTypeDef huart2;

unsigned char ble_data[32];

extern "C" {


void SysTick_Handler(void)
{
		HAL_IncTick();
		HAL_SYSTICK_IRQHandler();
}


void OTG_FS_IRQHandler(void)
{
		HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

void USART2_IRQHandler(void)
{
		unsigned char data = USART2->SR;
		static uint8_t index =0;
		ble_data[index++] = USART2->DR;
//		Hal::getInstance().WritePin('D' , 12 , true);		//老板子的LED引脚
//		Asc8_16(0,16, (u8*)"Receive bluetooth request");//8*16点阵ASCII码
//		char * s = "";
//		HAL_UART_Transmit(&huart2,&data,1,0xffff);
}

}
