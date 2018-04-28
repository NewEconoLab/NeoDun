#ifndef APP_UTILS_H
#define APP_UTILS_H

#include "main_define.h"



uint32_t 	Utils_crc32(uint32_t crc, uint8_t* buff, int len);
uint8_t		Utils_verifycrc(uint8_t data[],int len);
uint16_t	RandomInteger(void);
uint8_t		ArrayCompare(uint8_t* left, uint8_t* right, int len);
void 			RandomArray(uint8_t *array,uint8_t len);
void  		SHA256_Data(uint8_t* input, uint32_t inLen, uint8_t* outPut, uint32_t outLen);
void  		PrintArray(uint8_t data[], int len); 
	 

	 
#endif
