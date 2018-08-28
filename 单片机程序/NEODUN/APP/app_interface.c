/*****************************************************************
����оƬ�洢���£�һ��16���ۣ�X0-X15��
		X00����֤��ݵ����ģ���������Ķ�Ӧ�İ��ģ�ĩβӦ����CRCУ�飩
		X01���������A+B������
		X02���������A+B������
		X03��˽Կ1����
		X04��˽Կ2����
		X05��˽Կ3����
		X06��˽Կ4����
		X07��˽Կ5����
		X08����ַ1			
		X09����ַ2	
		X10����ַ3
		X11����ַ4
		X12����ַ5
		X13������������ܵĹ�Կ
		X14��ϵͳ��ʶ
		X15��C�����ɶ�д�����ܣ�
X14ϵͳ��ʶ�洢���£�
		0-7��ϵͳ��ʶ
				unsigned char	new_wallet; 			//1��ʾ��Ǯ����0��ʾ��Ǯ��
				unsigned char	update;						//1��ʾ��Ҫ������0��ʾ����Ҫ����
				unsigned char	language;					//1��ʾӢ�ģ�0��ʾ����
				unsigned char count;						//��ַ����
				unsigned char errornum;					//����������
	 8-15����
	16-21�����ñ�ʶ
				unsigned char auto_show;										//����Ǯ�����Զ�������������
				unsigned char Auto_Update_Flag;							//����ʱ�Զ�������
				unsigned char Add_Address_Flag;							//������ַ
				unsigned char Del_Address_Flag;							//ɾ����ַ
				unsigned char Backup_Address_Flag;				  //���ݵ�ַ
				unsigned char Backup_Address_Encrypt_Flag; 	//����Ǯ��ʱ���м��ܱ�ʶ			
	24-27����
	28-31:�¾�Ǯ����ʶ
��ַ���˵����address[32]
			0 -24��Base58����ǰ��25�ֽڣ�0x17��ʼ������
			25-30��6�ֽ���Ϊ��ַ����
			31	 ����4bit��ʾ�ò��е�ַ���Ƶĳ��ȣ���4bit��ʾ��������
******************************************************************		
�ڲ�FLASH����洢�滮��
		����0-����3��BootLoader
		����4			�����ݴ洢(������ת��ʱ����)
		����5+����6������APP��NEODUN��
		����7		 	��APP1
		����8			��APP2
		����9			��APP3
		����10		��APP4
		����11		��APP5
APP�洢����������7-����11����
			������ͣ�����������ַ-4
			����汾������������ַ-2
*****************************************************************/
#include "app_interface.h"
#include <string.h>
#include <stdio.h>
#include "atsha204a.h"
#include "app_utils.h"
#include "app_oled.h"
#include "stmflash.h"
#include "Algorithm.h"

/***************************************************
���ܣ���������ϵͳ��ʶ
���룺	
					��ʼ�����ϵͳ��ʶ�ṹ��ָ��
����ֵ��		1  �ɹ�
					0  ʧ��
***************************************************/
uint8_t Update_PowerOn_SYSFLAG(SYSTEM_NEODUN *flag)
{
		uint8_t i = 0;
		uint8_t slot_data[32];
		uint8_t slot_flag[32];
		uint32_t	chipSN[3];

		memset(slot_data,0,32);
		memset(slot_flag,0,32);
		if(ATSHA_read_data_slot(SLOT_FLAG,slot_flag) == 0)//��ȡ��14���۵�����
				return 0;
	
		//�ж��Ƿ�Ϊ��Ǯ��
		if((slot_flag[31] == 0x88)&&(slot_flag[30] == 0x88)&&(slot_flag[29] == 0x88)&&(slot_flag[28] == 0x88))
				flag->new_wallet = 0;
		else
				flag->new_wallet = 1;
		
		if(flag->new_wallet)//��Ǯ��������Ǯ��
		{
				flag->count = 0;
				flag->update = 0;
				flag->language = 0;
				flag->errornum = 0;
				EmptyWallet();
		}
		else//��Ǯ������ȡϵͳ��ʶ
		{
				flag->update   = slot_flag[1];
				flag->language = slot_flag[2];			
				flag->count    = Update_AddressInfo_To_Ram();
				flag->errornum = slot_flag[4];
				if(flag->count != slot_flag[3])//�������ĵ�ַ�����ͼ�¼��������һ�£�����´洢�ļ���ֵ
				{
						slot_flag[0] = 0;
						slot_flag[3] = flag->count;
						if(ATSHA_write_data_slot(SLOT_FLAG,0,slot_flag,32) == 0)
								return 0;
				}
		}
#ifdef printf_debug
		printf("address count: %d\r\n",flag->count);
#endif
		//��ȡоƬSN��
		chipSN[0] = *(__IO uint32_t*)(0x1FFF7A10);
		chipSN[1] = *(__IO uint32_t*)(0x1FFF7A14);
		chipSN[2] = *(__IO uint32_t*)(0x1FFF7A18);
		for(i=0;i<3;i++)
		{		
				flag->sn[4*i] 	= (chipSN[i] >> 24)&0xff;
				flag->sn[4*i+1] = (chipSN[i] >> 16)&0xff;
				flag->sn[4*i+2] = (chipSN[i] >>  8)&0xff;
				flag->sn[4*i+3] = (chipSN[i])&0xff;
		}
		PowerOn_PACKInfo_To_Ram();
		return 1;
}

/***************************************************
���ܣ������������ñ�ʶ
���룺	
					��ʼ�����ϵͳ��ʶ�ṹ��ָ��
����ֵ��		1  �ɹ�
					0  ʧ��
*****************	**********************************/
uint8_t Update_PowerOn_SetFlag(SET_FLAG *Flag,SYSTEM_NEODUN *flag)
{
		uint8_t slot_data[32];
	
		memset(slot_data,0,32);
		if(ATSHA_read_data_slot(SLOT_FLAG,slot_data) == 0)//��ȡ��14���۵�����
				return 0;
		
		if(flag->new_wallet)//��Ǯ��
		{
				Flag->flag.add_address = 1;
				Flag->flag.del_address = 1;
				Flag->flag.backup_address = 1;
				Flag->flag.auto_show = 0;
				Flag->flag.auto_update = 0;
				Flag->flag.backup_address_encrypt = 0;
				if(Updata_Set_Flag(Flag))
						return 1;
				else
						return 0;
		}
		else
		{
				Flag->flag.auto_show   = slot_data[16];
				Flag->flag.auto_update = slot_data[17];
				Flag->flag.add_address = slot_data[18];
				Flag->flag.del_address = slot_data[19];
				Flag->flag.backup_address = slot_data[20];
				Flag->flag.backup_address_encrypt = slot_data[21];
				return 1;
		}
}

/***************************************************
����ϵͳ�ĵ�ַ����ֵ  
����ֵ��		1  �ɹ�
					0  ʧ��
*****************	**********************************/
uint8_t Updata_SYS_Count(unsigned char count)
{
		uint8_t slot_data[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		if(ATSHA_read_data_slot(SLOT_FLAG,slot_data) == 0)
				return 0;
		slot_data[3] = count;
		if(ATSHA_write_data_slot(SLOT_FLAG,0,slot_data,32) == 0)
				return 0;
		return 1;
}

/***************************************************
�ϵ�󣬸��²����Ϣֵ���ڴ���
*****************	**********************************/
void PowerOn_PACKInfo_To_Ram(void)
{
		coinrecord.count = 0;
		//�����Ϣ
		if(STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP1) != 0xFFFF)
		{
				coinrecord.count++;
				coinrecord.coin1 = STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP1+COIN_TYPE_OFFSET);
				coinrecord.version1 = STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP1+COIN_VERSION_OFFSET);
		}
		else
		{
				coinrecord.coin1 = 0xffff;
				coinrecord.version1 = 0xffff;
		}
		if(STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP2) != 0xFFFF)
		{
				coinrecord.count++;
				coinrecord.coin2 = STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP2+COIN_TYPE_OFFSET);
				coinrecord.version2 = STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP2+COIN_VERSION_OFFSET);
		}
		else
		{
				coinrecord.coin2 = 0xffff;
				coinrecord.version2 = 0xffff;
		}		
		if(STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP3) != 0xFFFF)
		{
				coinrecord.count++;
				coinrecord.coin3 = STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP3+COIN_TYPE_OFFSET);
				coinrecord.version3 = STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP3+COIN_VERSION_OFFSET);
		}
		else
		{
				coinrecord.coin3 = 0xffff;
				coinrecord.version3 = 0xffff;
		}		
		if(STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP4) != 0xFFFF)
		{
				coinrecord.count++;
				coinrecord.coin4 = STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP4+COIN_TYPE_OFFSET);
				coinrecord.version4 = STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP4+COIN_VERSION_OFFSET);
		}		
		else
		{
				coinrecord.coin4 = 0xffff;
				coinrecord.version4 = 0xffff;
		}		
		if(STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP5) != 0xFFFF)
		{
				coinrecord.count++;
				coinrecord.coin5 = STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP5+COIN_TYPE_OFFSET);
				coinrecord.version5 = STMFLASH_ReadHalfWord(FLASH_ADDRESS_APP5+COIN_VERSION_OFFSET);
		}	
		else
		{
				coinrecord.coin5 = 0xffff;
				coinrecord.version5 = 0xffff;
		}		
}

/***************************************************
����ϵͳ�����ñ�ʶ  
����ֵ��		1  �ɹ�
					0  ʧ��
*****************	**********************************/
uint8_t Updata_Set_Flag(SET_FLAG *Flag)
{
		uint8_t slot_data[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		if(ATSHA_read_data_slot(SLOT_FLAG,slot_data) == 0)
				return 0;
		slot_data[16] = Flag->flag.auto_show;
		slot_data[17] = Flag->flag.auto_update;
		slot_data[18] = Flag->flag.add_address;
		slot_data[19] = Flag->flag.del_address;
		slot_data[20] = Flag->flag.backup_address;
		slot_data[21] = Flag->flag.backup_address_encrypt;
		if(ATSHA_write_data_slot(SLOT_FLAG,0,slot_data,32) == 0)
				return 0;		
		return 1;
}

/***************************************************
���ܣ�
		��RAM�е����ñ�ʶ��ϵͳ��ʶ�����µ�����оƬ
***************************************************/
void Update_Flag_ATSHA(SET_FLAG *Flag,SYSTEM_NEODUN *flag)
{
		uint8_t array_write[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		array_write[0] = flag->new_wallet;
		array_write[1] = flag->update;
		array_write[2] = flag->language;
		array_write[3] = flag->count;
		array_write[4] = flag->errornum;
		array_write[16] = Flag->flag.auto_show;
		array_write[17] = Flag->flag.auto_update;
		array_write[18] = Flag->flag.add_address;
		array_write[19] = Flag->flag.del_address;
		array_write[20] = Flag->flag.backup_address;
		array_write[21] = Flag->flag.backup_address_encrypt;
		array_write[28] = 0x88;
		array_write[29] = 0x88;
		array_write[30] = 0x88;
		array_write[31] = 0x88;
		ATSHA_write_data_slot(SLOT_FLAG,0,array_write,32);
}

/***************************************************
���ܣ�
		����Ǯ��������գ���Ϊ��Ǯ��
***************************************************/
void EmptyWallet(void)
{
		uint8_t array_write1[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		uint8_t array_write2[32] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		uint8_t i = 0;
			
		for(i=3;i<14;i++)
		{
				ATSHA_write_data_slot(i,0,array_write1,32);
				HAL_Delay(50);
		}
		ATSHA_write_data_slot(SLOT_FLAG,0,array_write2,32);
}

/***************************************************
���ܣ�
		�õ���ַ��ID��
���룺
		25�ֽڵĵ�ַ����
�����
		1-5 	��ַID��
		0			û�иõ�ַ
*****************	**********************************/
uint8_t Get_Address_ID(uint8_t array_address[25])
{
		uint8_t i = 0;		
		for(i=0;i<5;i++)
		{
				if(ArrayCompare(array_address,showaddress[i].content,25))
						return (i+1);
		}
		return 0;
}

/***************************************************
���ܣ�
		�õ�һ���յĲۺ�
�����
		�ۺ�ID
*****************	**********************************/
uint8_t Get_Empty_SlotID(void)
{
		uint8_t slot_data[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		uint8_t i = 0;
		for(i=0;i<5;i++)
		{
				ATSHA_read_data_slot(i+8,slot_data);
				if(slot_data[0] == 0)
						return (i+1);
		}
		return 0;
}

/***************************************************
���ܣ�
		�õ�һ���յ�APP�������
�����
		������ַ
*****************	**********************************/
uint32_t Get_Plugin_Write_Flash_Sector(uint16_t type)
{
		if(type == coinrecord.coin1)
				return FLASH_ADDRESS_APP1;
		if(type == coinrecord.coin2)
				return FLASH_ADDRESS_APP2;
		if(type == coinrecord.coin3)
				return FLASH_ADDRESS_APP3;
		if(type == coinrecord.coin4)
				return FLASH_ADDRESS_APP4;
		if(type == coinrecord.coin5)
				return FLASH_ADDRESS_APP5;
		
		if(STMFLASH_ReadWord(FLASH_ADDRESS_APP1) == 0xFFFFFFFF)
				return FLASH_ADDRESS_APP1;
		if(STMFLASH_ReadWord(FLASH_ADDRESS_APP2) == 0xFFFFFFFF)
				return FLASH_ADDRESS_APP2;
		if(STMFLASH_ReadWord(FLASH_ADDRESS_APP3) == 0xFFFFFFFF)
				return FLASH_ADDRESS_APP3;
		if(STMFLASH_ReadWord(FLASH_ADDRESS_APP4) == 0xFFFFFFFF)
				return FLASH_ADDRESS_APP4;
		if(STMFLASH_ReadWord(FLASH_ADDRESS_APP5) == 0xFFFFFFFF)
				return FLASH_ADDRESS_APP5;
		return 0;
}

/***************************************************
���ܣ�
		SlotIDΪ��ַ����Ӧ�Ĳۺ�
		stateΪ1ʱ�����ص�ַ
		stateΪ0ʱ���ָ���ַ					
*****************	**********************************/
void Hide_address(uint8_t SlotID,uint8_t state)
{
		uint8_t slot_data[32];
		ATSHA_read_data_slot(SlotID+7,slot_data);
		if(state)
				slot_data[31] = 1;
		else
				slot_data[31] = 0;
		ATSHA_write_data_slot(SlotID+7,0,slot_data,32);
}

/***************************************************
���ܣ�
		SlotIDΪ��ַ����Ӧ�Ĳۺ�
		�õ���ַ����Ӧ��˽Կ
***************************************************/
uint8_t	Get_Privekey(uint8_t SlotID,uint8_t pin[8],uint8_t Privekey[32])
{
		uint8_t slot_data[32];
		ATSHA_read_data_slot(SlotID-5,slot_data);
		return (DecryptData(slot_data,32,pin,Privekey));
}

/***************************************************
���ܣ�
		����Ǯ���ĵ�ַ��Ϣ���ڴ���
*****************	**********************************/
uint8_t Update_AddressInfo_To_Ram(void)
{
		uint8_t count = 0;
		uint8_t i;
		uint8_t slot_data[32];
		char base58char[40];
		int len_base58char = 0;

		for(i=0;i<5;i++)
		{
				ATSHA_read_data_slot(i+8,slot_data);
				if(slot_data[0] == 0x00)
						continue;
				showaddress[i].hide     = slot_data[31]&0xF;
				showaddress[i].len_name = (slot_data[31]>>4)&0xF;
				memmove(showaddress[i].name,slot_data+25,6);
				memmove(showaddress[i].content,slot_data,25);
				Alg_Base58Encode(showaddress[i].content,25,base58char,&len_base58char);
				memmove(showaddress[i].address,base58char,len_base58char);
				count++;
		}
		return count;
}

void Sort_One_Address(void)
{
		uint8_t i = 0;
		uint8_t slot_buff[32];
		memset(slot_buff,0,32);
		if(showaddress[0].content[0] != 0)
				return;
		else
		{
				for(i = 1;i<=4;i++)
				{
						if(showaddress[i].content[0] != 0)
						{
								memmove(&showaddress[0],&showaddress[i],sizeof(ADDRESS));
								memmove(slot_buff,showaddress[i].content,25);
								memmove(slot_buff+25,showaddress[i].name,showaddress[i].len_name);
								slot_buff[31] = (showaddress[i].len_name<<4)|(showaddress[i].hide);
								ATSHA_write_data_slot(8,0,slot_buff,32);
								ATSHA_read_data_slot(3+i,slot_buff);
								ATSHA_write_data_slot(3,0,slot_buff,32);							
								//���ǰһ����ַ����Ϣ
								memset(&showaddress[i],0,sizeof(ADDRESS));
								memset(slot_buff,0,32);
								ATSHA_write_data_slot(8+i,0,slot_buff,32);
								ATSHA_write_data_slot(3+i,0,slot_buff,32);						
								break;
						}
				}
		}
}
void Sort_Two_Address(void)
{
		uint8_t i = 0;
		uint8_t slot_buff[32];
		memset(slot_buff,0,32);
		if(showaddress[0].content[0] != 0)
		{
				if(showaddress[1].content[0] != 0)
						return;
				else
				{
						for(i = 2;i<=4;i++)
						{
								if(showaddress[i].content[0] != 0)
								{
										memmove(&showaddress[1],&showaddress[i],sizeof(ADDRESS));
										memmove(slot_buff,showaddress[i].content,25);
										memmove(slot_buff+25,showaddress[i].name,showaddress[i].len_name);
										slot_buff[31] = (showaddress[i].len_name<<4)|(showaddress[i].hide);
										ATSHA_write_data_slot(8,0,slot_buff,32);
										ATSHA_read_data_slot(3+i,slot_buff);
										ATSHA_write_data_slot(3+1,0,slot_buff,32);										
										//���ǰһ����ַ����Ϣ
										memset(&showaddress[i],0,sizeof(ADDRESS));
										memset(slot_buff,0,32);
										ATSHA_write_data_slot(8+i,0,slot_buff,32);
										ATSHA_write_data_slot(3+i,0,slot_buff,32);									
										break;
								}
						}
				}
		}
		else
		{
				for(i = 1;i<=4;i++)
				{
						if(showaddress[i].content[0] != 0)
						{
								memmove(&showaddress[0],&showaddress[i],sizeof(ADDRESS));
								memmove(slot_buff,showaddress[i].content,25);
								memmove(slot_buff+25,showaddress[i].name,showaddress[i].len_name);
								slot_buff[31] = (showaddress[i].len_name<<4)|(showaddress[i].hide);
								ATSHA_write_data_slot(8,0,slot_buff,32);
								ATSHA_read_data_slot(3+i,slot_buff);
								ATSHA_write_data_slot(3,0,slot_buff,32);							
								//���ǰһ����ַ����Ϣ
								memset(&showaddress[i],0,sizeof(ADDRESS));
								memset(slot_buff,0,32);
								ATSHA_write_data_slot(8+i,0,slot_buff,32);
								ATSHA_write_data_slot(3+i,0,slot_buff,32);
								break;
						}
				}
				if(showaddress[1].content[0] != 0)
						return;
				else
				{
						for(i = 2;i<=4;i++)
						{
								if(showaddress[i].content[0] != 0)
								{
										memmove(&showaddress[1],&showaddress[i],sizeof(ADDRESS));
										memmove(slot_buff,showaddress[i].content,25);
										memmove(slot_buff+25,showaddress[i].name,showaddress[i].len_name);
										slot_buff[31] = (showaddress[i].len_name<<4)|(showaddress[i].hide);
										ATSHA_write_data_slot(8,0,slot_buff,32);
										ATSHA_read_data_slot(3+i,slot_buff);
										ATSHA_write_data_slot(3+1,0,slot_buff,32);									
										//���ǰһ����ַ����Ϣ
										memset(&showaddress[i],0,sizeof(ADDRESS));
										memset(slot_buff,0,32);
										ATSHA_write_data_slot(8+i,0,slot_buff,32);
										ATSHA_write_data_slot(3+i,0,slot_buff,32);
										break;
								}
						}
				}				
		}
}
void Sort_Three_Address(void)
{
		uint8_t i = 0;
		uint8_t slot_buff[32];
		memset(slot_buff,0,32);
	
		if(showaddress[4].content[0] == 0)
		{
				if(showaddress[3].content[0] == 0)
						return;
				else
				{
						for(i = 0;i<=2;i++)
						{
								if(showaddress[i].content[0] == 0)
								{
										memmove(&showaddress[i],&showaddress[3],sizeof(ADDRESS));
										memmove(slot_buff,showaddress[3].content,25);
										memmove(slot_buff+25,showaddress[3].name,showaddress[3].len_name);
										slot_buff[31] = (showaddress[3].len_name<<4)|(showaddress[3].hide);
										ATSHA_write_data_slot(8+i,0,slot_buff,32);
										ATSHA_read_data_slot(3+3,slot_buff);
										ATSHA_write_data_slot(3+i,0,slot_buff,32);										
										//���ǰһ����ַ����Ϣ
										memset(&showaddress[3],0,sizeof(ADDRESS));
										memset(slot_buff,0,32);
										ATSHA_write_data_slot(8+3,0,slot_buff,32);
										ATSHA_write_data_slot(3+3,0,slot_buff,32);									
										break;
								}
						}						
				}
		}
		else
		{
				for(i = 0;i<=3;i++)
				{
						if(showaddress[i].content[0] == 0)
						{
								memmove(&showaddress[i],&showaddress[4],sizeof(ADDRESS));
								memmove(slot_buff,showaddress[4].content,25);
								memmove(slot_buff+25,showaddress[4].name,showaddress[4].len_name);
								slot_buff[31] = (showaddress[4].len_name<<4)|(showaddress[4].hide);
								ATSHA_write_data_slot(8,0,slot_buff,32);
								ATSHA_read_data_slot(3+4,slot_buff);
								ATSHA_write_data_slot(3+i,0,slot_buff,32);
								//���ǰһ����ַ����Ϣ
								memset(&showaddress[4],0,sizeof(ADDRESS));
								memset(slot_buff,0,32);
								ATSHA_write_data_slot(8+4,0,slot_buff,32);
								ATSHA_write_data_slot(3+4,0,slot_buff,32);							
								break;
						}
				}
				if(showaddress[3].content[0] == 0)
						return;
				else
				{
						for(i = 0;i<=2;i++)
						{
								if(showaddress[i].content[0] == 0)
								{
										memmove(&showaddress[i],&showaddress[3],sizeof(ADDRESS));
										memmove(slot_buff,showaddress[3].content,25);
										memmove(slot_buff+25,showaddress[3].name,showaddress[3].len_name);
										slot_buff[31] = (showaddress[3].len_name<<4)|(showaddress[3].hide);
										ATSHA_write_data_slot(8,0,slot_buff,32);
										ATSHA_read_data_slot(3+3,slot_buff);
										ATSHA_write_data_slot(3+i,0,slot_buff,32);									
										//���ǰһ����ַ����Ϣ
										memset(&showaddress[3],0,sizeof(ADDRESS));
										memset(slot_buff,0,32);
										ATSHA_write_data_slot(8+3,0,slot_buff,32);
										ATSHA_write_data_slot(3+3,0,slot_buff,32);
										break;
								}
						}						
				}				
		}
}
void Sort_Four_Address(void)
{
		uint8_t i = 0;
		uint8_t slot_buff[32];
		memset(slot_buff,0,32);	
		if(showaddress[4].content[0] == 0)
				return;
		for(i = 0;i<=3;i++)
		{
				if(showaddress[i].content[0] == 0)
				{
						memmove(&showaddress[i],&showaddress[4],sizeof(ADDRESS));
						memmove(slot_buff,showaddress[4].content,25);
						memmove(slot_buff+25,showaddress[4].name,showaddress[4].len_name);
						slot_buff[31] = (showaddress[4].len_name<<4)|(showaddress[4].hide);
						ATSHA_write_data_slot(8+i,0,slot_buff,32);
						ATSHA_read_data_slot(3+4,slot_buff);
						ATSHA_write_data_slot(3+i,0,slot_buff,32);
						//���ǰһ����ַ����Ϣ
						memset(&showaddress[4],0,sizeof(ADDRESS));
						memset(slot_buff,0,32);
						ATSHA_write_data_slot(8+4,0,slot_buff,32);
						ATSHA_write_data_slot(3+4,0,slot_buff,32);
						break;
				}
		}
}
/***************************************************
���ܣ�
		����Ǯ����ַ���ڴ�ͼ���оƬ�洢����
���ó�����
		�û�����USBɾ����ַ��������ַ���Ͽ�USB��ˢ����Ļǰ
*****************	**********************************/
void Sort_wallet_Address(void)
{
		switch (Neo_System.count)
		{
			case 0:					
					break;
			case 1:
					Sort_One_Address();
					break;
			case 2:
					Sort_Two_Address();
					break;
			case 3:
					Sort_Three_Address();
					break;
			case 4:
					Sort_Four_Address();
					break;
			case 5:
					break;			
		}
}




