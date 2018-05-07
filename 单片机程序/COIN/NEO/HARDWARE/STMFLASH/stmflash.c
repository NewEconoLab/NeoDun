#include "stmflash.h"
#include "delay.h"
#include "usart.h"

void STMFLASH_WriteWord(uint32_t faddr,uint32_t value)
{
		FLASH_Unlock();					//解锁
		FLASH_ProgramWord(faddr,value);
		FLASH_Lock();           //上锁
}

uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
		return *(vu32*)faddr; 
}

void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)		//从指定地址开始写入指定长度的数据
{
		FLASH_Status status = FLASH_COMPLETE;
		u32 endaddr=0;	
		if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
		FLASH_Unlock();									//解锁 
		FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
		
		endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
		if(status==FLASH_COMPLETE)
		{
			while(WriteAddr<endaddr)//写数据
			{
				if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)//写入数据
				{ 
					break;	//写入异常
				}
				WriteAddr+=4;
				pBuffer++;
			} 
		}
		FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
		FLASH_Lock();//上锁		
}

void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)   		//从指定地址开始读出指定长度的数据
{
		u32 i;
		for(i=0;i<NumToRead;i++)
		{
				pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
				ReadAddr+=4;//偏移4个字节.	
		}
}

void STMFLASH_Erase_Sectors(uint32_t sector)
{
		FLASH_Unlock();
		FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
		FLASH_EraseSector(sector,VoltageRange_3);
		FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
		FLASH_Lock();//上锁
}














