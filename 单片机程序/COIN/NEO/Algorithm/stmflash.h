#ifndef __STMFLASH_H__
#define __STMFLASH_H__ 

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

typedef __IO uint32_t  vu32;
typedef __IO uint16_t  vu16;
typedef __IO uint8_t   vu8;

//FLASH起始地址
//BOOT0 和 BOOT1 都接GND时，运行代码开始于此处
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH的起始地址
#define FLASH_WAITETIME  50000          //FLASH等待超时时间

//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//扇区11起始地址,128 Kbytes  


uint32_t STMFLASH_ReadWord(uint32_t faddr);		  	//读出字  
void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead);   		//从指定地址开始读出指定长度的数据
void STMFLASH_Erase_Sectors(uint32_t sector);		
void STMFLASH_WriteWord(uint32_t faddr,uint32_t value);
void STMFLASH_WriteHalfWord(uint32_t faddr,uint16_t value);
void STMFLASH_WriteByte(uint32_t faddr,uint8_t value);

uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);
	 
void STMFLASH_Write_ByteArray(uint32_t WriteAddr,uint8_t *pBuffer,uint32_t NumToWrite);		//从指定地址开始写入指定长度的数据	 
void STMFLASH_Read_ByteArray(uint32_t ReadAddr,uint8_t *pBuffer,uint32_t NumToRead);   		//从指定地址开始读出指定长度的数据	 

		
#endif

















