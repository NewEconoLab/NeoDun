#ifndef __IAP_H__
#define __IAP_H__

#include "sys.h"  
  
//定义一个函数类型的参数. 	
typedef  void (*iapfun)(void);					  

//保留0X08000000~0X0800FFFF的空间为Bootloader使用(共64KB)
//第一个应用程序起始地址(存放在FLASH) 扇区4和扇区5

#define FLASH_APP1_ADDR		0x08010000  	
																				
void iap_load_app(uint32_t appxaddr);				
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);
#endif







































