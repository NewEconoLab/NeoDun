#include "app_interface.h"
#include <string.h>
#include "atsha204a.h"

/***************************************************
�����     pinָ��һ��unsigned char������Ϊ8������
					�洢Ϊ��0x12,0x34,0x56,0x78  �����1,2,3,4,5,6,7,8   
����ֵ��		1  �ɹ�
					0  ʧ��
*****************	**********************************/
unsigned char ReadPinCode(unsigned char *pin)
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
unsigned char WritePinCode(unsigned char *pin)
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
		��6����ʶΪ��
		��7����ʶΪ��
�����     flagΪָ��һ��unsigned char������Ϊ4������
����ֵ��		1  �ɹ�
					0  ʧ��
*****************	**********************************/
unsigned char ReadAT204Flag(unsigned char *flag)
{
		unsigned char value = 0;
		unsigned char pin_read[32];
    unsigned char i;
		memset(flag,0,4);
		memset(pin_read,0,32);

		//��ȡ��5�����ܲ۵�����
		value = ATSHA_read_encrypted(5,pin_read,15);
		
		//��������У���4-7�����ݴ洢FLAG
    for(i=0;i<4;i++)
    {
        flag[i] = pin_read[i+4];
    }
		
		return value;
}

/***************************************************
���ܣ�
		//�Ӵ洢�ĵط�ȥ��PIN�룬û�����þ�����Ǯ��,��Ȼ���Ǿ�Ǯ��
����ֵ��		1  ��Ǯ��
					0  ��Ǯ��
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





