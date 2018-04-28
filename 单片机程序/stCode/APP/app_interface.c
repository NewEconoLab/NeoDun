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
	 8-15����
	16-21�����ñ�ʶ
				unsigned char auto_show;								//����Ǯ�����Զ�������������
				unsigned char Auto_Update_Flag;							//����ʱ�Զ�������
				unsigned char Add_Address_Flag;							//������ַ
				unsigned char Del_Address_Flag;							//ɾ����ַ
				unsigned char Backup_Address_Flag;				  //���ݵ�ַ
				unsigned char Backup_Address_Encrypt_Flag; 	//����Ǯ��ʱ���м��ܱ�ʶ			
	24-27����
	28-31:�¾�Ǯ����ʶ
*****************************************************************/
#include "app_interface.h"
#include <string.h>
#include <stdio.h>
#include "atsha204a.h"
#include "main_define.h"
#include "app_utils.h"

/***************************************************
���ܣ���������ϵͳ��ʶ
���룺	
					��ʼ�����ϵͳ��ʶ�ṹ��ָ��
����ֵ��		1  �ɹ�
					0  ʧ��
*****************	**********************************/
uint8_t Update_PowerOn_SYSFLAG(SYSTEM_FLAG *flag)
{
		uint8_t i = 0;
		uint8_t slot_data[32];
		uint8_t slot_flag[32];

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
				EmptyWallet();
		}
		else//��Ǯ������ȡϵͳ��ʶ
		{
				flag->update = slot_flag[1];
				flag->language = slot_flag[2];
				for(i=8;i<13;i++)
				{
						if(ATSHA_read_data_slot(i,slot_data) == 0)
								return 0;
						if(slot_data[0] != 0x00)
								flag->count++;
						memset(slot_data,0,32);
				}
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
		return 1;
}

/***************************************************
���ܣ������������ñ�ʶ
���룺	
					��ʼ�����ϵͳ��ʶ�ṹ��ָ��
����ֵ��		1  �ɹ�
					0  ʧ��
*****************	**********************************/
uint8_t Update_PowerOn_SetFlag(SET_FLAG *Flag,SYSTEM_FLAG *flag)
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
void Update_Flag_ATSHA(SET_FLAG *Flag,SYSTEM_FLAG *flag)
{
		uint8_t array_write[32];
		array_write[0] = flag->new_wallet;
		array_write[1] = flag->update;
		array_write[2] = flag->language;
		array_write[3] = flag->count;
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
		uint8_t j = 0;
		uint8_t flag = 0;
		uint8_t slot_data[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
		for(i=0;i<5;i++)
		{
				ATSHA_read_data_slot(i+8,slot_data);				
				if(slot_data[0] == 0)
						continue;
				for(j=0;j<25;j++)
				{
						if(array_address[j] != slot_data[j])
						{		
								flag = 1;
								break;
						}
				}
				if(flag == 0)
						return (i+1);
				memset(slot_data,0,32);
				flag = 0;
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





