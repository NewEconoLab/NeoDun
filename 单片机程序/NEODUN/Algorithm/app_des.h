#ifndef APP_DES_H_
#define APP_DES_H_

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
 extern "C" {
#endif

int32_t My_DES_Encrypt(uint8_t* InputMessage,
													uint32_t  InputMessageLength,
													uint8_t  *DES_Key,
													uint8_t  *OutputMessage,
													uint32_t *OutputMessageLength);

int32_t My_DES_Decrypt(uint8_t*  InputMessage,
													uint32_t  InputMessageLength,
													uint8_t  *DES_Key,
													uint8_t  *OutputMessage,
													uint32_t *OutputMessageLength
													);

int32_t STM32_DES_CBC_Encrypt(uint8_t*  InputMessage,
                        uint32_t  InputMessageLength,
                        uint8_t  *DES_Key,
                        uint8_t  *InitializationVector,
                        uint32_t  IvLength,
                        uint8_t  *OutputMessage,
                        uint32_t *OutputMessageLength);

int32_t STM32_DES_CBC_Decrypt(uint8_t*  InputMessage,
                        uint32_t  InputMessageLength,
                        uint8_t  *DES_Key,
                        uint8_t  *InitializationVector,
                        uint32_t  IvLength,
                        uint8_t  *OutputMessage,
                        uint32_t *OutputMessageLength);

void Test_DES(void);

#ifdef __cplusplus
}
#endif	

#endif


