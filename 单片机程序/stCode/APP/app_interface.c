/*****************************************************************
加密芯片存储如下：一个16个槽（X0-X15）
		X00：验证身份的明文（这里的明文对应的暗文，末尾应加上CRC校验）
		X01：随机数（A+B）明文
		X02：随机数（A+B）明文
		X03：私钥1明文
		X04：私钥2明文
		X05：私钥3明文
		X06：私钥4明文
		X07：私钥5明文
		X08：地址1
		X09：地址2
		X10：地址3
		X11：地址4
		X12：地址5
		X13：升级程序解密的公钥
		X14：系统标识
		X15：C（不可读写，保密）
X14系统标识存储如下：
		0-7：系统标识
				unsigned char	new_wallet; 			//1表示新钱包，0表示旧钱包
				unsigned char	update;						//1表示需要升级，0表示不需要升级
				unsigned char	language;					//1表示英文，0表示中文
				unsigned char count;						//地址数量
	 8-15：空
	16-21：设置标识
				unsigned char auto_show;								//连接钱包后自动弹出驱动界面
				unsigned char Auto_Update_Flag;							//开机时自动检查更新
				unsigned char Add_Address_Flag;							//新增地址
				unsigned char Del_Address_Flag;							//删除地址
				unsigned char Backup_Address_Flag;				  //备份地址
				unsigned char Backup_Address_Encrypt_Flag; 	//备份钱包时进行加密标识			
	24-27：空
	28-31:新旧钱包标识
*****************************************************************/
#include "app_interface.h"
#include <string.h>
#include <stdio.h>
#include "atsha204a.h"
#include "main_define.h"
#include "app_utils.h"

/***************************************************
功能：开机更新系统标识
输入：	
					初始化后的系统标识结构体指针
返回值：		1  成功
					0  失败
*****************	**********************************/
uint8_t Update_PowerOn_SYSFLAG(SYSTEM_FLAG *flag)
{
		uint8_t i = 0;
		uint8_t slot_data[32];
		uint8_t slot_flag[32];

		memset(slot_data,0,32);
		memset(slot_flag,0,32);
		if(ATSHA_read_data_slot(SLOT_FLAG,slot_flag) == 0)//读取第14个槽的数据
				return 0;
	
		//判断是否为新钱包
		if((slot_flag[31] == 0x88)&&(slot_flag[30] == 0x88)&&(slot_flag[29] == 0x88)&&(slot_flag[28] == 0x88))
				flag->new_wallet = 0;
		else
				flag->new_wallet = 1;
		
		if(flag->new_wallet)//新钱包，重置钱包
		{
				flag->count = 0;
				flag->update = 0;
				flag->language = 0;
				EmptyWallet();
		}
		else//旧钱包，读取系统标识
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
				if(flag->count != slot_flag[3])//开机检测的地址数量和记录的数量不一致，则更新存储的计数值
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
功能：开机更新设置标识
输入：	
					初始化后的系统标识结构体指针
返回值：		1  成功
					0  失败
*****************	**********************************/
uint8_t Update_PowerOn_SetFlag(SET_FLAG *Flag,SYSTEM_FLAG *flag)
{
		uint8_t slot_data[32];
	
		memset(slot_data,0,32);
		if(ATSHA_read_data_slot(SLOT_FLAG,slot_data) == 0)//读取第14个槽的数据
				return 0;
		
		if(flag->new_wallet)//新钱包
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
更新系统的地址计数值  
返回值：		1  成功
					0  失败
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
更新系统的设置标识  
返回值：		1  成功
					0  失败
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
功能：
		将RAM中的设置标识和系统标识，更新到加密芯片
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
功能：
		将此钱包数据清空，改为新钱包
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
功能：
		得到地址的ID号
输入：
		25字节的地址数组
输出：
		1-5 	地址ID号
		0			没有该地址
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
功能：
		得到一个空的槽号
输出：
		槽号ID
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





