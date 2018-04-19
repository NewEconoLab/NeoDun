#include "app_interface.h"
#include <string.h>
#include "atsha204a.h"
#include "stmflash.h"
#include "aw9136.h"
#include "oled.h"

#define SLOT_FLAG		14
#define SLOT_SECRET	15

extern SIGN_KEY_FLAG Key_Flag;
extern volatile uint8_t touch_motor_flag;    //1��ʾ���������񶯣�0��ʾ�ر���

/***************************************************
����оƬ��4-7�ֽڴ��ʶ���£�
		��4����ʶΪ����Ǯ����ʶ
		��5����ʶΪ��������ʶ
		��6����ʶΪ����Ӣ���л�
		��7����ʶΪ��
�����     flagΪָ��һ��unsigned char������Ϊ4������
����ֵ��		1  �ɹ�
					0  ʧ��
*****************	**********************************/
uint8_t ReadAT204Flag(BOOT_SYS_FLAG *flag)
{
		uint8_t slot_data[32];
	
		memset(slot_data,0,32);
		if(ATSHA_read_data_slot(SLOT_FLAG,slot_data) == 0)//��ȡ��14���۵�����
				return 0;
	
		if(ATSHA_read_data_slot(SLOT_SECRET,slot_data) == 1)//��15���ۣ�����Ϊ���ɶ�д�������ȡ�ɹ�����δ�ȶԼ���оƬ��������
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
���ܣ�
		����Ǯ��������գ���Ϊ��Ǯ��
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
		if(STMFLASH_ReadWord(0x08010000) == 0XFFFFFFFF)//������APP����
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
						Key_Control(0);				//��հ�����־λ������������Ч

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
						Key_Control(1);				//��հ�����־λ������������Ч
						
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
								if(Key_Flag.Key_center_Flag)//�м䰴�����£��˳�
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

