#include "app_interface.h"
#include <string.h>
#include "atsha204a.h"


/***************************************************
�����     pinָ��һ��unsigned char������Ϊ8������
					�洢Ϊ��0x12,0x34,0x56,0x78  �����1,2,3,4,5,6,7,8   
����ֵ��		1  �ɹ�
					0  ʧ��
*****************	**********************************/
unsigned char ReadPinCode(unsigned char pin[8])
{
		unsigned char value = 0;
    unsigned char pin_read[32];
    unsigned char i;
    memset(pin,0,8);
		memset(pin_read,0,32);
		
		//��ȡ��5�����ܲ۵�����
		value = ATSHA_read_encrypted(5,pin_read,15);  
		
		//������۵�ǰ�ĸ����ݴ洢Pin��
    for(i=0;i<4;i++)
    {
        pin[2*i]   = ((pin_read[i]>>4)&0x0F) + 0x30;
        pin[2*i+1] = (pin_read[i]&0x0F) + 0x30;
    }
		return value;
}
/***************************************************
���룺     pinָ��һ��unsigned char������Ϊ8������
					����1,2,3,4,5,6,7,8   �洢Ϊ��0x12,0x34,0x56,0x78
����ֵ��		1  �ɹ�
					0  ʧ��
*****************	**********************************/
unsigned char WritePinCode(unsigned char pin[8])
{
		unsigned char value = 0;
    unsigned char pin_read[32];
    unsigned char i;
		memset(pin_read,0,32);
	
		//��ȡ��5�����ܲ۵�����
		if(ATSHA_read_encrypted(5,pin_read,15) == 0)
		{
				return 0;
		}
	
		for(i=0;i<4;i++)
		{
				pin_read[i] = (pin[2*i] - 0x30)*16+(pin[2*i+1] - 0x30); 				
		}
		
		//�ٰ�������Pin����д�����оƬ
		value = ATSHA_write_encrypted(5,pin_read,15);
		
		return value;
}
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
unsigned char ReadAT204Flag(BOOT_FLAG *flag)
{
		unsigned char pin_read[32];
		memset(flag,0,4);
		memset(pin_read,0,32);

		//��ȡ��5�����ܲ۵�����
		if(ATSHA_read_encrypted(5,pin_read,15) == 0)
				return 0;
	
		if((pin_read[31] == 0x88)&&(pin_read[30] == 0x88)&&(pin_read[29] == 0x88)&&(pin_read[28] == 0x88))
				flag->new_wallet = 0;
		else
				flag->new_wallet = 1;
		
		if(flag->new_wallet == 1)
		{
				flag->update = 0;
				flag->language = 0;
				EmptyWallet();
		}
		else
		{
				flag->update = pin_read[5];
				flag->language = pin_read[6];
		}
		
		return 1;		
}

/***************************************************
���ܣ�
		����Ǯ��������գ���Ϊ��Ǯ��
*****************	**********************************/
void EmptyWallet(void)
{
		unsigned char array_write1[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		unsigned char array_write2[32] = {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
																			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		unsigned char i;	
			
		for(i=0;i<5;i++)
		{
				ATSHA_write_encrypted(i,array_write1,15);
		}																		
		ATSHA_write_encrypted(5,array_write2,15);
		for(i=6;i<16;i++)
		{
				ATSHA_write_data_slot(i,0,array_write1,32);
		}
}

/***************************************************
���ܣ�
		����Ǯ����Ϊ��Ǯ��
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


