/*
 * Hal.cpp
 *
 *  Created on: 2017年7月9日
 *      Author: Administrator
 */

#include "Hal.h"
#include "OLED281/oled281.h"
#include "usbd_customhid.h"
#include "aw9136.h"

extern UART_HandleTypeDef huart2;
extern USBD_HandleTypeDef hUsbDeviceFS;


Hal::Hal() {

}

Hal::Hal(const Hal& copy) {
}


Hal& Hal::getInstance() 
{
		static Hal hal;
		return hal;
}

void Hal::WritePin(char port, int pin, bool state) 
{
		HAL_GPIO_WritePin(getPort(port), this->getPin(pin), state ? GPIO_PIN_SET:GPIO_PIN_RESET);
}

u16 Hal::getPin(int idx) 
{
		switch(idx)
		{
				case 0:		return GPIO_PIN_0;
				case 1:		return GPIO_PIN_1;
				case 2:		return GPIO_PIN_2;
				case 3:		return GPIO_PIN_3;
				case 4:		return GPIO_PIN_4;
				case 5:		return GPIO_PIN_5;
				case 6:		return GPIO_PIN_6;
				case 7:		return GPIO_PIN_7;
				case 8:		return GPIO_PIN_8;
				case 9:		return GPIO_PIN_9;
				case 10:	return GPIO_PIN_10;
				case 11:	return GPIO_PIN_11;
				case 12:	return GPIO_PIN_12;
				case 13:	return GPIO_PIN_13;
				case 14:	return GPIO_PIN_14;
				case 15:	return GPIO_PIN_15;
		}
		return 0;
}

GPIO_TypeDef* Hal::getPort(char port) 
{
		GPIO_TypeDef* rt = NULL;
		switch(port)
		{
			case 'A' :
				rt = GPIOA;
				break;
			case 'B' :
				rt = GPIOB;
				break;
			case 'C' :
				rt = GPIOC;
				break;
			case 'D' :
				rt = GPIOD;
				break;
			case 'E' :
				rt = GPIOE;
				break;
			case 'F' :
				rt = GPIOF;
				break;
			case 'G' :
				rt = GPIOG;
				break;
		}
		return rt;
}

bool Hal::ReadPin(char port, int pin) 
{
		return HAL_GPIO_ReadPin(getPort(port),getPin(pin));
}

void Hal::Delay(int ms) 
{
		HAL_Delay(ms);
}

//USB HID发送方法
void Hal::SendUSBData(u8 *data , int len)
{
		USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,data,len);
//		USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)&hUsbDeviceFS;
//		while(hhid->state == CUSTOM_HID_BUSY);
}


void Hal::HardwareInit() 
{
		OLED281_Init();
		__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
		AW9136_Init();
}

void Hal::Start_Motor(int mtime)
{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_Delay(mtime);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

