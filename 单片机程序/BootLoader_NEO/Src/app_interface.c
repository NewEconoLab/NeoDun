#include "app_interface.h"
#include <string.h>
#include "atsha204a.h"

#define SLOT_FLAG		14
#define SLOT_SECRET	15

/***************************************************
输出：     pin指向一个unsigned char型容量为8的数组
					存储为：0x12,0x34,0x56,0x78  输出：1,2,3,4,5,6,7,8   
返回值：		1  成功
					0  失败
*****************	**********************************/
unsigned char ReadPinCode(unsigned char pin[8])
{
		unsigned char value = 0;
    unsigned char pin_read[32];
    unsigned char i;
    memset(pin,0,8);
		memset(pin_read,0,32);
		
		//读取第5个加密槽的数据
		value = ATSHA_read_encrypted(5,pin_read,15);  
		
		//第五个槽的前四个数据存储Pin码
    for(i=0;i<4;i++)
    {
        pin[2*i]   = ((pin_read[i]>>4)&0x0F) + 0x30;
        pin[2*i+1] = (pin_read[i]&0x0F) + 0x30;
    }
		return value;
}
/***************************************************
输入：     pin指向一个unsigned char型容量为8的数组
					例：1,2,3,4,5,6,7,8   存储为：0x12,0x34,0x56,0x78
返回值：		1  成功
					0  失败
*****************	**********************************/
unsigned char WritePinCode(unsigned char pin[8])
{
		unsigned char value = 0;
    unsigned char pin_read[32];
    unsigned char i;
		memset(pin_read,0,32);
	
		//读取第5个加密槽的数据
		if(ATSHA_read_encrypted(5,pin_read,15) == 0)
		{
				return 0;
		}
	
		for(i=0;i<4;i++)
		{
				pin_read[i] = (pin[2*i] - 0x30)*16+(pin[2*i+1] - 0x30); 				
		}
		
		//再把重组后的Pin重新写入加密芯片
		value = ATSHA_write_encrypted(5,pin_read,15);
		
		return value;
}
/***************************************************
加密芯片第4-7字节存标识如下：
		第4个标识为：新钱包标识
		第5个标识为：升级标识
		第6个标识为：中英文切换
		第7个标识为：
输出：     flag为指向一个unsigned char型容量为4的数组
返回值：		1  成功
					0  失败
*****************	**********************************/
unsigned char ReadAT204Flag(BOOT_FLAG *flag)
{
		unsigned char slot_data[32];
	
		memset(slot_data,0,32);
		if(ATSHA_read_data_slot(SLOT_FLAG,slot_data) == 0)//读取第14个槽的数据
				return 0;
	
		if(ATSHA_read_data_slot(SLOT_SECRET,slot_data) == 1)//第15个槽，设置为不可读写，如果读取成功，则未先对加密芯片处理，出错
				return 0;
		
		if((slot_data[31] == 0x88)&&(slot_data[30] == 0x88)&&(slot_data[29] == 0x88)&&(slot_data[28] == 0x88))
				flag->new_wallet = 0;
		else
				flag->new_wallet = 1;
		
		if(flag->new_wallet)
		{
				flag->update = 0;
				flag->language = 0;
				EmptyWallet();
		}
		else
		{
				flag->update = slot_data[1];
				flag->language = slot_data[2];
		}
		
		return 1;		
}

/***************************************************
功能：
		将此钱包数据清空，改为新钱包
*****************	**********************************/
void EmptyWallet(void)
{
		unsigned char array_write1[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		unsigned char array_write2[32] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		unsigned char i = 0;
			
		for(i=3;i<14;i++)
		{
				ATSHA_write_data_slot(i,0,array_write1,32);
		}																		
		ATSHA_write_data_slot(SLOT_FLAG,0,array_write2,32);
}

/***************************************************
功能：
		将此钱包改为旧钱包
*****************	**********************************/
void ChangeWallet(void)
{
		unsigned char pin_read[32];
		unsigned char i;
		memset(pin_read,0,32);
		ATSHA_read_encrypted(5,pin_read,15);
		pin_read[4] = 0;
		for(i=28;i<32;i++)
				pin_read[i] = 0x88;
		ATSHA_write_encrypted(5,pin_read,15);
}



