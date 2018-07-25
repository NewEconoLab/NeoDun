#ifndef APP_UTILS_H
#define APP_UTILS_H

#include "main_define.h"
#include "sys.h"

typedef __IO uint32_t  vu32;


uint32_t 	Utils_crc32(uint32_t crc, uint8_t* buff, int len);
uint8_t		Utils_verifycrc(uint8_t data[],int len);
uint16_t	RandomInteger(void);
uint8_t		ArrayCompare(uint8_t* left, uint8_t* right, int len);
void 			RandomArray(uint8_t *array,uint8_t len);
void  		SHA256_Data(uint8_t* input, uint32_t inLen, uint8_t* outPut, uint32_t outLen);
void  		PrintArray(uint8_t data[], int len); 
void 			jump_to_app(uint32_t appxaddr);	 
void 			Power_ON_BLE(void);
void			Power_OFF_BLE(void);
	 
	 
#endif
