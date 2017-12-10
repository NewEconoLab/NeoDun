#ifndef __STMFLASH_H__
#define __STMFLASH_H__ 

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "main_define.h"

typedef __IO uint32_t  vu32;

//FLASH��ʼ��ַ
//BOOT0 �� BOOT1 ����GNDʱ�����д��뿪ʼ�ڴ˴�
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH����ʼ��ַ
#define FLASH_WAITETIME  50000          //FLASH�ȴ���ʱʱ��

//���������
#define ADDR_FLASH_COUNT				((uint32_t)0x08040000)
#define ADDR_FLASH_SET					((uint32_t)0x08040200)
#define ADDR_FLASH_PASSPORT			((uint32_t)0x08040400)
#define ADDR_FLASH_ACTUAL	      ADDR_FLASH_SECTOR_7
#define FLASH_SECTOR_USE				FLASH_SECTOR_6

//����ʹ��
//#define ADDR_FLASH_COUNT				((uint32_t)0x0800C000)
//#define ADDR_FLASH_SET					((uint32_t)0x0800C200)
//#define ADDR_FLASH_PASSPORT			((uint32_t)0x0800C400)
//#define ADDR_FLASH_ACTUAL    	  ADDR_FLASH_SECTOR_6
//#define FLASH_SECTOR_USE				FLASH_SECTOR_3


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
#define MAX_ADDR_FLASH					((uint32_t)0x08100000)

#ifdef __cplusplus
 extern "C" {
#endif

uint32_t STMFLASH_ReadWord(uint32_t faddr);		  	//������  
void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
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

















