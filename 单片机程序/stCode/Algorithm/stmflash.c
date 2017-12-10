#include "stmflash.h"
#include "string.h"

//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(vu32*)faddr; 
}  
//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
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
////从指定地址开始写入指定长度的数据
////特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
////         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
////         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
////         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
////该函数对OTP区域也有效!可以用来写OTP区!
////OTP区域地址范围:0X1FFF7800~0X1FFF7A0F
////WriteAddr:起始地址(此地址必须为4的倍数!!)
////pBuffer:数据指针
////NumToWrite:字(32位)数(就是要写入的32位数据的个数.) 
//void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)	
//{ 
//	FLASH_EraseInitTypeDef FlashEraseInit;	
//  HAL_StatusTypeDef FlashStatus = HAL_OK;
//	uint32_t SectorError=0;	
//	uint32_t addrx=0;
//	uint32_t endaddr=0;	
//  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
//	
//	HAL_FLASH_Unlock();									//解锁 
//	addrx=WriteAddr;				//写入的起始地址
//	endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
//	
//	if(addrx<0X1FFF0000)			//只有主存储区,才需要执行擦除操作!!
//	{
//		while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
//		{
//			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
//			{   
//				FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除 
//				FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //要擦除的扇区
//				FlashEraseInit.NbSectors=1;                             //一次只擦除一个扇区
//				FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
//				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
//				{
//					break;//发生错误了	
//				}
//			}else addrx+=4;
//			FLASH_WaitForLastOperation(FLASH_WAITETIME);                //等待上次操作完成			
//		} 
//	}
//	if(FlashStatus==HAL_OK)
//	{
//		while(WriteAddr<endaddr)//写数据
//		{
//			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//写入数据
//			{ 
//				break;	//写入异常
//			}
//			WriteAddr+=4;
//			pBuffer++;
//		} 
//	}
//	HAL_FLASH_Lock();           //上锁
//} 

void STMFLASH_WriteWord(uint32_t faddr,uint32_t value)
{
		HAL_FLASH_Unlock();					//解锁
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,faddr,value);
		HAL_FLASH_Lock();           //上锁
}


void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)	
{ 
		uint32_t endaddr=0;	
		if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
		
		HAL_FLASH_Unlock();							 //解锁 
	
		endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
		while(WriteAddr<endaddr)//写数据
		{
				if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//写入数据
				{ 
						break;	//写入异常
				}
				WriteAddr+=4;
				pBuffer++;
		} 

		HAL_FLASH_Lock();           //上锁
} 

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(4位)数
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)   	
{
		uint32_t i;
		for(i=0;i<NumToRead;i++)
		{
				pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
				ReadAddr+=4;//偏移4个字节.	
		}
}

void STMFLASH_Erase_Sectors(uint32_t sector)
{
		FLASH_EraseInitTypeDef FlashEraseInit;	
		HAL_StatusTypeDef FlashStatus = HAL_OK;		
		uint32_t SectorError=0;
	
		HAL_FLASH_Unlock();																					//解锁
//		__HAL_FLASH_DATA_CACHE_DISABLE();														//FLASH擦除期间,必须禁止数据缓存	
	
		FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;      			//擦除类型，扇区擦除
		FlashEraseInit.Sector=sector;   														//要擦除的扇区
		FlashEraseInit.NbSectors=1;                             		//一次只擦除一个扇区
		FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      		//电压范围，VCC=2.7~3.6V之间!!
		if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) //发生错误了
		{
		}	
		FLASH_WaitForLastOperation(FLASH_WAITETIME);                //等待上次操作完成
		
//		__HAL_FLASH_DATA_CACHE_ENABLE();														//FLASH擦除结束,开启数据缓存		
		HAL_FLASH_Lock();           																//上锁
		
		if(FlashStatus != HAL_OK)
		{
				printf("Erase Flash Sector Error !\r\n");
		}
}

uint32_t Get_Count_Num(void)
{	
		uint32_t count = STMFLASH_ReadWord(ADDR_FLASH_COUNT);
		if(count == 0xffffffff)//表示为新钱包
				return 0;
		else
				return count;
}

//上电检测总的计数的值是否正确，不正确则擦除，重新写入计数值
void Update_PowerOn_Count(void)
{
		uint32_t address = ADDR_FLASH_ACTUAL;
		uint32_t index = 0;
		uint32_t tmp=0;
		uint32_t count = Get_Count_Num();
		
		if(count == 0)//表示为新钱包
				return;
	
		while(address < MAX_ADDR_FLASH)
		{		
				tmp = STMFLASH_ReadWord(address);
				//值为0xffffffff表示该地址还未写过值，值为0表示该地址被擦除，即删除了地址私钥对
				if((tmp != 0xffffffff)&&(tmp != 0))
				{
						index += 1;
				}
				address += 80;//每个地址和私钥对占用80个字节空间
		}
		
		if(index != count)//值不一样,把新值更新写入
		{
				Update_Count_Num(index);
		}
		printf("all count: %d \r\n",index);
}

//上电检测设置值，不正确则写入默认值，重新写入计数值
void Update_PowerOn_SetFlag(SIGN_SET_FLAG *Flag)
{
		uint32_t flag[7];

		memset(flag,0,sizeof(flag));
		STMFLASH_Read(ADDR_FLASH_SET,flag,7);	
	
		if((flag[0] == 0xffffffff)||(flag[0] == 1))
		{
				Flag->New_Device_Flag = 1;
				flag[0] = 1;
		}
		else
		{
				Flag->New_Device_Flag = 0;
		}		
		if((flag[3] != 0)&&(flag[3] != 1))
		{		
				Flag->Add_Address_Flag = 1;
				flag[3] = 1;
		}
		else
		{
				Flag->Add_Address_Flag = flag[3];
		}		
		if((flag[4] != 0)&&(flag[4] != 1))
		{
				Flag->Del_Address_Flag = 1;
				flag[4] = 1;
		}
		else
		{
				Flag->Del_Address_Flag = flag[4];
		}
		if((flag[5] != 0)&&(flag[5] != 1))
		{		
				Flag->Backup_Address_Flag = 1;		
				flag[5] = 1;
		}
		else
		{
				Flag->Backup_Address_Flag = flag[5];
		}
		
		//这三个标志位暂时未用到，赋默认值
		Flag->Auto_Show_Flag = 0;
		Flag->Auto_Update_Flag = 0;
		Flag->Backup_Address_Encrypt_Flag = 0;
		flag[1] = 0;
		flag[2] = 0;
		flag[6] = 0;
		
		Updata_Set_Flag(Flag);
}

//更新私钥和地址对的计数值，该值存在扇区3，每次更新，必须擦除该扇区
void Update_Count_Num(uint32_t count)
{
		uint32_t passport[6];
		uint32_t flag[7];
	
		memset(passport,0,sizeof(passport));
		memset(flag,0,sizeof(flag));			
	
		//擦除扇区之前，记录下该扇区其它存储的值
		STMFLASH_Read(ADDR_FLASH_SET,flag,7);
		STMFLASH_Read(ADDR_FLASH_PASSPORT,passport,6);
		STMFLASH_Erase_Sectors(FLASH_SECTOR_USE);//清除密码存储地址的值		
	
		//将其它存储的值也重新写进去
		STMFLASH_WriteWord(ADDR_FLASH_COUNT,count); //重新写入新的计数值	
		STMFLASH_Write(ADDR_FLASH_SET,flag,7);		
		STMFLASH_Write(ADDR_FLASH_PASSPORT,passport,6);		
}

//更新设置值，该值存在扇区3，每次更新，必须擦除该扇区
void Updata_Set_Flag(SIGN_SET_FLAG *Flag)
{		
		uint32_t count=0;
		uint32_t passport[6];
		uint32_t flag[7];
	
		memset(passport,0,sizeof(passport));
		memset(flag,0,sizeof(flag));		
	
		flag[0] = Flag->New_Device_Flag;
		flag[1] = Flag->Auto_Show_Flag;
		flag[2] = Flag->Auto_Update_Flag;
		flag[3] = Flag->Add_Address_Flag;
		flag[4] = Flag->Del_Address_Flag;
		flag[5] = Flag->Backup_Address_Flag;
		flag[6] = Flag->Backup_Address_Encrypt_Flag;
	
		//擦除扇区之前，记录下该扇区其它存储的值
		count = STMFLASH_ReadWord(ADDR_FLASH_COUNT);
		STMFLASH_Read(ADDR_FLASH_PASSPORT,passport,6);
		STMFLASH_Erase_Sectors(FLASH_SECTOR_USE);//清除密码存储地址的值
		
		//将其它存储的值也重新写进去
		STMFLASH_WriteWord(ADDR_FLASH_COUNT,count);
		STMFLASH_Write(ADDR_FLASH_SET,flag,7);	  //重新写入新的设置值	
		STMFLASH_Write(ADDR_FLASH_PASSPORT,passport,6);
}

//更新密码值，该值存在扇区3，每次更新，必须擦除该扇区
void Update_Passport(uint32_t* passport)
{
		uint32_t count=0;
		uint32_t flag[7];
	
		memset(flag,0,sizeof(flag));		
	
		//擦除扇区之前，记录下该扇区其它存储的值
		count = STMFLASH_ReadWord(ADDR_FLASH_COUNT);
		STMFLASH_Read(ADDR_FLASH_SET,flag,7);
		STMFLASH_Erase_Sectors(FLASH_SECTOR_USE);//清除密码存储地址的值	
	
		//将其它存储的值也重新写进去
		STMFLASH_WriteWord(ADDR_FLASH_COUNT,count); 	
		STMFLASH_Write(ADDR_FLASH_SET,flag,7);		
		STMFLASH_Write(ADDR_FLASH_PASSPORT,passport,6); 		//重新写入新的密码		
}



void Get_Passport(uint32_t* passport)
{
		STMFLASH_Read(ADDR_FLASH_PASSPORT,passport,6);
}

//从初始地址扇区6开始找到一个能写的地址
uint32_t Get_Flash_EMPTY(void)
{
		uint32_t address = ADDR_FLASH_ACTUAL;
	
		while(STMFLASH_ReadWord(address)!=0xffffffff)
		{
				address += 80;
				if(address > MAX_ADDR_FLASH)
						return 0;
		}
	
		return address;
}

//从已知账户的地址得到对应存储在FLASH的位置地址的值
uint32_t Get_Flash_Address(char *add,uint32_t count)
{
		uint32_t i = 0;
		uint32_t address = ADDR_FLASH_ACTUAL;
		uint32_t tmp=0;
		char address_flash[40] = "";
		
		while(i<count)
		{		
				tmp = STMFLASH_ReadWord(address);
				if((tmp != 0xffffffff)&&(tmp != 0))
				{
						STMFLASH_Read(address,(uint32_t *)address_flash,10);
						if(strncmp(address_flash,add,strlen(address_flash)) == 0)
						{
								break;
						}
						i++;
				}
				address += 80;
		}
		
		if(i == count)
				return 0;
		else
				return address;
}



