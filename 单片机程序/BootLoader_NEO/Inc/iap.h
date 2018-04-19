#ifndef __IAP_H__
#define __IAP_H__

#include "sys.h"  
  
//����һ���������͵Ĳ���. 	
typedef  void (*iapfun)(void);					  

//����0X08000000~0X0800FFFF�Ŀռ�ΪBootloaderʹ��(��64KB)
//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH) ����4������5

#define FLASH_APP1_ADDR		0x08010000  	
																				
void iap_load_app(uint32_t appxaddr);				
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);
#endif







































