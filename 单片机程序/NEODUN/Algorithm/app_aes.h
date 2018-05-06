#ifndef APP_AES_H_
#define APP_AES_H_

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
 extern "C" {
#endif

int32_t STM32_AES_ECB_Encrypt(uint8_t* InputMessage,
                        uint32_t InputMessageLength,
                        uint8_t  *AES256_Key,
                        uint8_t  *OutputMessage,
                        uint32_t *OutputMessageLength);

int32_t STM32_AES_ECB_Decrypt(uint8_t* InputMessage,
                        uint32_t InputMessageLength,
                        uint8_t *AES256_Key,
                        uint8_t  *OutputMessage,
                        uint32_t *OutputMessageLength);

void Test_AES(void);

#ifdef __cplusplus
}
#endif		 
	 
#endif


