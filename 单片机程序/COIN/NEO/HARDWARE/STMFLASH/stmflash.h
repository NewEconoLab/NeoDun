#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  

#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ

void STMFLASH_WriteWord(uint32_t faddr,uint32_t value);
uint32_t STMFLASH_ReadWord(uint32_t faddr);  
void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
void STMFLASH_Erase_Sectors(uint32_t sector);		


						   
#endif

















