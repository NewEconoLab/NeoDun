#include "app_interface.h"
#include <string.h>
#include "atsha204a.h"

/***************************************************
输出：     pin指向一个unsigned char型容量为8的数组
					存储为：0x12,0x34,0x56,0x78  输出：1,2,3,4,5,6,7,8   
返回值：		1  成功
					0  失败
*****************	**********************************/
unsigned char ReadPinCode(unsigned char *pin)
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
unsigned char WritePinCode(unsigned char *pin)
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
		第6个标识为：
		第7个标识为：
输出：     flag为指向一个unsigned char型容量为4的数组
返回值：		1  成功
					0  失败
*****************	**********************************/
unsigned char ReadAT204Flag(unsigned char *flag)
{
		unsigned char value = 0;
		unsigned char pin_read[32];
    unsigned char i;
		memset(flag,0,4);
		memset(pin_read,0,32);

		//读取第5个加密槽的数据
		value = ATSHA_read_encrypted(5,pin_read,15);
		
		//第五个槽中，第4-7个数据存储FLAG
    for(i=0;i<4;i++)
    {
        flag[i] = pin_read[i+4];
    }
		
		return value;
}

/***************************************************
功能：
		//从存储的地方去读PIN码，没有设置就是新钱包,不然就是旧钱包
返回值：		1  旧钱包
					0  新钱包
*****************	**********************************/
unsigned char Get_StatusOfWallet(void)
{


		
		if(1)
		{
				return 1;
		}
		else
				return 0;		
}





