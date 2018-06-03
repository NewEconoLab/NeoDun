#ifndef __STMFLASH_H__
#define __STMFLASH_H__ 

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

typedef __IO uint32_t  vu32;
typedef __IO uint16_t  vu16;
typedef __IO uint8_t   vu8;

//FLASH��ʼ��ַ
//BOOT0 �� BOOT1 ����GNDʱ�����д��뿪ʼ�ڴ˴�
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH����ʼ��ַ
#define FLASH_WAITETIME  50000          //FLASH�ȴ���ʱʱ��

//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes  


uint32_t STMFLASH_ReadWord(uint32_t faddr);		  	//������  
void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
void STMFLASH_Erase_Sectors(uint32_t sector);		
void STMFLASH_WriteWord(uint32_t faddr,uint32_t value);
void STMFLASH_WriteHalfWord(uint32_t faddr,uint16_t value);
void STMFLASH_WriteByte(uint32_t faddr,uint8_t value);

uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);
	 
void STMFLASH_Write_ByteArray(uint32_t WriteAddr,uint8_t *pBuffer,uint32_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����	 
void STMFLASH_Read_ByteArray(uint32_t ReadAddr,uint8_t *pBuffer,uint32_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����	 

		
#endif

















