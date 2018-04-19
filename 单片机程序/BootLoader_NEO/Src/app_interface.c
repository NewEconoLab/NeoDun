#include "app_interface.h"
#include <string.h>
#include "atsha204a.h"
#include "stmflash.h"
#include "aw9136.h"
#include "oled.h"

#define SLOT_FLAG		14
#define SLOT_SECRET	15

extern SIGN_KEY_FLAG Key_Flag;
extern volatile uint8_t touch_motor_flag;    //1表示开启触摸振动，0表示关闭振动

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
uint8_t ReadAT204Flag(BOOT_SYS_FLAG *flag)
{
		uint8_t slot_data[32];
	
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

uint8_t Have_App(void)
{
		if(STMFLASH_ReadWord(0x08010000) == 0XFFFFFFFF)//不存在APP程序
				return 0;
		else
				return 1;
}

uint8_t NEO_Test(void)
{
		int i=0;
		int value = 0;
		Fill_RAM(0x00);
		Asc8_16( 52,4 ,"Enter NeoDun Test ?");
		Asc8_16(24,44,"Cancel");
		Asc8_16(124,44,"OK");
		Asc8_16(184,44,"Cancel");
	
		while(1)
		{														
				if(Key_Flag.Key_center_Flag)
				{
						touch_motor_flag = 0;
						Key_Control(0);				//清空按键标志位，开启按键无效

						Fill_RAM(0x00);
						Asc8_16( 84,4 ,"NeoDun Test");		
						HAL_Delay(2000);
						
						for(i=0;i<5;i++)
						{
								Fill_RAM(0xFF);
								HAL_Delay(1000);
								Fill_RAM(0x00);
								HAL_Delay(1000);
						}
				/******************************************
										NeoDun Test
									Motor and Key Test
						On						Exit					Off
				******************************************/		
						Asc8_16( 84,4 ,"NeoDun Test");
						Asc8_16( 56,24 ,"Motor and Key Test");
						Asc8_16( 30,44 ,"On");
						Asc8_16( 112,44 ,"Exit");
						Asc8_16( 206,44 ,"Off");
						Key_Control(1);				//清空按键标志位，开启按键有效
						
						while(1) 
						{
								if(Key_Flag.Key_left_Flag)
								{
										Key_Flag.Key_left_Flag = 0;
										HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
								}
								if(Key_Flag.Key_right_Flag)
								{
										Key_Flag.Key_right_Flag = 0;
										HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);						
								}
								if(Key_Flag.Key_center_Flag)//中间按键按下，退出
								{
										Key_Flag.Key_center_Flag = 0;
										break;
								}
						}
						touch_motor_flag = 1;
						value = 1;						
						break;
				}
				else if((Key_Flag.Key_right_Flag == 1)||(Key_Flag.Key_left_Flag == 1))
				{							
						value = 0;
						break;
				}
		}
		Fill_RAM(0x00);
		return value;
}

