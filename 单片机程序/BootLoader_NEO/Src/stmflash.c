#include "stmflash.h"

unsigned char wallet_status = 0;//0��ʾ��Ǯ����1��ʾ��Ǯ��

//��ȡָ����ַ�İ���(16λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(vu32*)faddr; 
}  
//��ȡĳ����ַ���ڵ�flash����
//addr:flash��ַ
//����ֵ:0~11,��addr���ڵ�����
uint16_t STMFLASH_GetFlashSector(uint32_t addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_SECTOR_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_SECTOR_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_SECTOR_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_SECTOR_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_SECTOR_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_SECTOR_10; 
	return FLASH_SECTOR_11;	
}
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ر�ע��:��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����
//         д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
//         д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
//         û����Ҫ����,��������������Ȳ�����,Ȼ����������д. 
//�ú�����OTP����Ҳ��Ч!��������дOTP��!
//OTP�����ַ��Χ:0X1FFF7800~0X1FFF7A0F
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
//pBuffer:����ָ��
//NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.) 
//void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)	
//{ 
//	FLASH_EraseInitTypeDef FlashEraseInit;	
//  HAL_StatusTypeDef FlashStatus = HAL_OK;
//	uint32_t SectorError=0;	
//	uint32_t addrx=0;
//	uint32_t endaddr=0;	
//  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
//	
//	HAL_FLASH_Unlock();									//���� 
//	addrx=WriteAddr;				//д�����ʼ��ַ
//	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
//	
//	if(addrx<0X1FFF0000)			//ֻ�����洢��,����Ҫִ�в�������!!
//	{
//		while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
//		{
//			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
//			{   
//				FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //�������ͣ��������� 
//				FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //Ҫ����������
//				FlashEraseInit.NbSectors=1;                             //һ��ֻ����һ������
//				FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //��ѹ��Χ��VCC=2.7~3.6V֮��!!
//				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
//				{
//					break;//����������	
//				}
//			}else addrx+=4;
//			FLASH_WaitForLastOperation(FLASH_WAITETIME);                //�ȴ��ϴβ������			
//		} 
//	}
//	if(FlashStatus==HAL_OK)
//	{
//		while(WriteAddr<endaddr)//д����
//		{
//			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//д������
//			{ 
//				break;	//д���쳣
//			}
//			WriteAddr+=4;
//			pBuffer++;
//		} 
//	}
//	HAL_FLASH_Lock();           //����
//} 

void STMFLASH_WriteWord(uint32_t faddr,uint32_t value)
{
		HAL_FLASH_Unlock();					//����
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,faddr,value);
		HAL_FLASH_Lock();           //����
}


void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)	
{ 
	uint32_t addrx=0;
	uint32_t endaddr=0;	
  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
	
	HAL_FLASH_Unlock();									//���� 
	addrx=WriteAddr;				//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
	
	if(addrx<0X1FFF0000)
	{
		while(WriteAddr<endaddr)//д����
		{
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//д������
			{ 
				break;	//д���쳣
			}
			WriteAddr+=4;
			pBuffer++;
		} 
	}
	HAL_FLASH_Lock();           //����
} 

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(4λ)��
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)   	
{
	uint32_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}

void STMFLASH_Erase_Sectors(uint32_t sector)
{
		FLASH_EraseInitTypeDef FlashEraseInit;	
		HAL_StatusTypeDef FlashStatus = HAL_OK;		
		uint32_t SectorError=0;
	
		HAL_FLASH_Unlock();																					//����
		FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;      			//�������ͣ���������
		FlashEraseInit.Sector=sector;   														//Ҫ����������
		FlashEraseInit.NbSectors=1;                             		//һ��ֻ����һ������
		FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      		//��ѹ��Χ��VCC=2.7~3.6V֮��!!
		if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) //����������
		{
					
		}	
		FLASH_WaitForLastOperation(FLASH_WAITETIME);                //�ȴ��ϴβ������
		HAL_FLASH_Lock();           																//����
		
		if(FlashStatus != HAL_OK)
		{
		}
}

//�Ӵ洢�ĵط�ȥ��PIN�룬û�����þ�����Ǯ��,��Ȼ���Ǿ�Ǯ��
void Get_StatusOfWallet(void)
{
	
	
	
		if(1)
		{
				wallet_status = 1;
		}
		else
				wallet_status = 0;		
}










