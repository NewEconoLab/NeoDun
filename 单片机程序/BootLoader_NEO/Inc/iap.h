#ifndef __IAP_H__
#define __IAP_H__

#include "sys.h"  
  
//定义一个函数类型的参数. 	
typedef  void (*iapfun)(void);					  

//保留0X08000000~0X0800FFFF的空间为Bootloader使用(共64KB)

#define FLASH_APP0_ADDR		0x08020000 	  //扇区5
#define FLASH_APP1_ADDR		0x08060000 		//扇区6
#define FLASH_APP2_ADDR		0x08080000 		//扇区7
#define FLASH_APP3_ADDR		0x080A0000 		//扇区8
#define FLASH_APP4_ADDR		0x080C0000 		//扇区9
#define FLASH_APP5_ADDR		0x080E0000		//扇区10

void iap_load_app(uint32_t appxaddr);				
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);
#endif







































