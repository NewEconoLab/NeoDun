
#ifndef __BITBANG_I2C_H
#define	__BITBANG_I2C_H

#include "stm32f4xx_hal.h"
#include "atca_status.h"//用于返回错误代码

/*&&&&&&&&&&&&&&&&&&&        主要函数      &&&&&&&&&&&&&&&&&&&&&&&&&&&*/

void bitBang_I2c_Init(void);

#define uchar uint8_t

void ATSHA_i2c_send_wake_token(void);

ATCA_STATUS ATSHA_ISendNBytes(uchar *s,uchar no);

ATCA_STATUS ATSHA_IRcvNBytes(uchar *s,uchar no);


#endif

