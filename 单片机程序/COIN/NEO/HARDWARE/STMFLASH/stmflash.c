#include "stmflash.h"
#include "delay.h"
#include "usart.h"

void STMFLASH_WriteWord(uint32_t faddr,uint32_t value)
{
		FLASH_Unlock();					//����
		FLASH_ProgramWord(faddr,value);
		FLASH_Lock();           //����
}

uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
		return *(vu32*)faddr; 
}

void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)		//��ָ����ַ��ʼд��ָ�����ȵ�����
{
		FLASH_Status status = FLASH_COMPLETE;
		u32 endaddr=0;	
		if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
		FLASH_Unlock();									//���� 
		FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
		
		endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
		if(status==FLASH_COMPLETE)
		{
			while(WriteAddr<endaddr)//д����
			{
				if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)//д������
				{ 
					break;	//д���쳣
				}
				WriteAddr+=4;
				pBuffer++;
			} 
		}
		FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
		FLASH_Lock();//����		
}

void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)   		//��ָ����ַ��ʼ����ָ�����ȵ�����
{
		u32 i;
		for(i=0;i<NumToRead;i++)
		{
				pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
				ReadAddr+=4;//ƫ��4���ֽ�.	
		}
}

void STMFLASH_Erase_Sectors(uint32_t sector)
{
		FLASH_Unlock();
		FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
		FLASH_EraseSector(sector,VoltageRange_3);
		FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
		FLASH_Lock();//����
}














