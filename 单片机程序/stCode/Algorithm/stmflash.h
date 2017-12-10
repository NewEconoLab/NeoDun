#ifndef __STMFLASH_H__
#define __STMFLASH_H__ 

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "main_define.h"

typedef __IO uint32_t  vu32;

//FLASH起始地址
//BOOT0 和 BOOT1 都接GND时，运行代码开始于此处
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH的起始地址
#define FLASH_WAITETIME  50000          //FLASH等待超时时间

//开发板调试
#define ADDR_FLASH_COUNT				((uint32_t)0x08040000)
#define ADDR_FLASH_SET					((uint32_t)0x08040200)
#define ADDR_FLASH_PASSPORT			((uint32_t)0x08040400)
#define ADDR_FLASH_ACTUAL	      ADDR_FLASH_SECTOR_7
#define FLASH_SECTOR_USE				FLASH_SECTOR_6

//正常使用
//#define ADDR_FLASH_COUNT				((uint32_t)0x0800C000)
//#define ADDR_FLASH_SET					((uint32_t)0x0800C200)
//#define ADDR_FLASH_PASSPORT			((uint32_t)0x0800C400)
//#define ADDR_FLASH_ACTUAL    	  ADDR_FLASH_SECTOR_6
//#define FLASH_SECTOR_USE				FLASH_SECTOR_3


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
#define MAX_ADDR_FLASH					((uint32_t)0x08100000)

#ifdef __cplusplus
 extern "C" {
#endif

uint32_t STMFLASH_ReadWord(uint32_t faddr);		  	//读出字  
void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead);   		//从指定地址开始读出指定长度的数据
void STMFLASH_Erase_Sectors(uint32_t sector);		
	 
void STMFLASH_WriteWord(uint32_t faddr,uint32_t value);	 


void Update_Count_Num(uint32_t count);
uint32_t Get_Count_Num(void);	 
void Update_PowerOn_Count(void);
void Updata_Set_Flag(SIGN_SET_FLAG *Flag);	 
void Update_Passport(uint32_t* passport);	 
void Get_Passport(uint32_t* passport);	 
uint32_t Get_Flash_EMPTY(void);
uint32_t Get_Flash_Address(char *add,uint32_t count);
void Update_PowerOn_SetFlag(SIGN_SET_FLAG *Flag);	 

#ifdef __cplusplus
 }
#endif
		
#endif

















