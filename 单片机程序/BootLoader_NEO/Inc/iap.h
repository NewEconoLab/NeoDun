#ifndef __IAP_H__
#define __IAP_H__

#include "sys.h"  
  
//����һ���������͵Ĳ���. 	
typedef  void (*iapfun)(void);					  

//����0X08000000~0X0800FFFF�Ŀռ�ΪBootloaderʹ��(��64KB)

#define FLASH_APP0_ADDR		0x08020000 	  //����5
#define FLASH_APP1_ADDR		0x08060000 		//����6
#define FLASH_APP2_ADDR		0x08080000 		//����7
#define FLASH_APP3_ADDR		0x080A0000 		//����8
#define FLASH_APP4_ADDR		0x080C0000 		//����9
#define FLASH_APP5_ADDR		0x080E0000		//����10

void iap_load_app(uint32_t appxaddr);				
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);
#endif







































