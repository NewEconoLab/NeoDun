#ifndef APP_COMMAND_H
#define APP_COMMAND_H

#include <stm32f4xx_hal.h>
#include "main_define.h"

void Hid_Data_Analysis(uint8_t data[],int len);
uint8_t isAllPackGot(void);
uint16_t Get_Serial_ID(void);
void USBHIDSend(uint8_t *data,int len);
uint32_t command_crc32(uint32_t crc, uint8_t* buff, int len);
uint8_t	command_verifycrc(uint8_t data[],int len);
void Printf_array(uint8_t data[],int len);
void SHA256_Data(uint8_t* input, uint32_t inLen, uint8_t* outPut, uint32_t outLen);
void SHA256_test(void);

#endif


