/*
 * Hal.h
 *
 *  Created on: 2017Äê7ÔÂ9ÈÕ
 *      Author: Administrator
 */

#ifndef CPP_HAL_H_
#define CPP_HAL_H_

#include "myType.h"
#include "stm32f4xx_hal.h"


class Hal {
private:
	Hal();
	Hal(const Hal & copy);
	u16 getPin(int idx);
	GPIO_TypeDef *getPort(char port);
public:
	static  Hal & getInstance();
	void Delay(int ms);
	void WritePin(char port , int pin , bool state);
	bool ReadPin(char port , int  pin );
	void HardwareInit();
	void SendUSBData(u8 *data ,int len);
	void Start_Motor(int mtime);
}; 


#endif /* CPP_HAL_H_ */
