#ifndef APP_HAL_H
#define APP_HAL_H

#include "main_define.h"


void USB_Init(void);	 
void Deal_USB_ERROR(void);	 
void SendUSBData(uint8_t *data ,int len);
uint8_t Scan_USB(void);
void System_Reset(void);
	 

	 
#endif
 
 
 
 
 
