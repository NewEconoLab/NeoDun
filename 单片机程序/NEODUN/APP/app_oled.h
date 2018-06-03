#ifndef APP_OLED_H
#define APP_OLED_H

#include "main_define.h"



uint8_t SetCode(uint8_t code_array[8]);
uint8_t VerifyCode(uint8_t code_array[8],uint8_t state);
uint8_t setCode(void);
uint8_t verifyCode(uint8_t state);
uint8_t verifyCodeGetPin(uint8_t state,uint8_t PinCode[8]);
uint8_t EncryptDataNew(uint8_t *data,int len,uint8_t num_slot,uint8_t PinCode[8]);	
uint8_t EncryptData(uint8_t *data,int len,uint8_t num_slot,uint8_t PinCode[8]);
uint8_t DecryptData(uint8_t *data,int len,uint8_t Pin[8],uint8_t *Output);


	 
 
	 
#endif
