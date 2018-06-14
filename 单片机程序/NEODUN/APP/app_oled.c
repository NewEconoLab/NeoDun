#include "app_oled.h"
#include <string.h>
#include "OLED281/oled281.h"
#include "timer.h"
#include "atsha204a.h"
#include "app_des.h"
#include "app_aes.h"
#include "app_utils.h"
#include "aw9136.h"
#include "OledMenu.h"

/************************************************
设置密码函数
返回值： 0  失败
				1  成功
*************************************************/
uint8_t SetCode(uint8_t code_array[8])
{
		uint8_t num[2] = {'0','\0'};
    uint8_t index = 0;	
		memset(code_array,0x35,8);//将数组都设置为字符 ‘5’
		
		Fill_RAM(0x00);
		if(Neo_System.language == 0)
		{
				//第一行显示
				Show_HZ12_12(96,7,2,5);//设置密码
		}
		else
		{
		
		}
		Show_Pattern(&gImage_emptypin[0],15,17,26,38);
		Show_Pattern(&gImage_emptypin[0],21,23,26,38);
		Show_Pattern(&gImage_emptypin[0],27,29,26,38);
		Show_Pattern(&gImage_emptypin[0],33,35,26,38);
		Show_Pattern(&gImage_emptypin[0],39,41,26,38);
		Show_Pattern(&gImage_emptypin[0],45,47,26,38);
		clearArea(60,38,200,1);
		Display_arrow(0);
		Display_arrow(1);
		Asc8_16(124,52,"5");
		
		Key_Control(1);
    while(index < 6)
    {
        if(Key_Flag.flag.left)//左键按下
        {
						Key_Flag.flag.left = 0;
            if(code_array[index] == '0')
            {
								clearArea(124,52,8,16);
								Show_Pattern(&gImage_delete[0],31,32,48,64);
                code_array[index] = 0xFF;
            }
						else if(code_array[index] == 0xFF)
						{
								clearArea(124,48,8,16);
								Asc8_16(124,52,"9");
                code_array[index] = '9';								
						}
            else
            {
                num[0] = --code_array[index];
								Asc8_16(124,52,num);
            }
        }
        if(Key_Flag.flag.right)//右键按下
        {
						Key_Flag.flag.right = 0;
            if(code_array[index] == '9')
            {
								clearArea(124,52,8,16);
								Show_Pattern(&gImage_delete[0],31,32,48,64);
                code_array[index] = 0xFF;
            }
						else if(code_array[index] == 0xFF)
						{
								clearArea(124,48,8,16);
								Asc8_16(124,52,"0");
                code_array[index] = '0';		
						}
            else
            {
                num[0] = ++code_array[index];
								Asc8_16(124,52,num);
            }
        }
        if(Key_Flag.flag.middle)//中间建按下
        {
						Key_Flag.flag.middle = 0;
						if(code_array[index] == 0xFF)
						{
								code_array[index] = '5';
								if(index>0)
										index--;
								code_array[index] = '5';
								Show_Pattern(&gImage_emptypin[0],15+6*index,17+6*index,26,38);
								clearArea(58+index*24,38,16,1);
						}
						else
						{
								Show_Pattern(&gImage_fullpin[0],15+6*index,17+6*index,26,38);
								clearArea(58+index*24,38,16,1);
								index++;
						}
            if(index == 6)
            {
								break;
            }
            else
            {
								Asc8_16(124,52,"5");
            }
        }
    }
		Key_Control(0);//清空按键标志位，开启按键无效
		return 1;
}
/************************************************
密码验证函数
返回值： 
				0  失败
				1  成功
*************************************************/
uint8_t VerifyCode(uint8_t code_array[8],uint8_t state)
{
		uint8_t num[2] = {'0','\0'};
    uint8_t index = 0;
    memset(code_array,0x35,8);//将数组都设置为字符 ‘5’

		Fill_RAM(0x00);
		if(Neo_System.language == 0)
		{
				//第一行显示
				Show_HZ12_12(96,7,133,133);//输
				Show_HZ12_12(112,7,25,25);//入
				Show_HZ12_12(128,7,4,5);//密码		
		}
		else
		{
		
		}
		Show_Pattern(&gImage_emptypin[0],15,17,26,38);
		Show_Pattern(&gImage_emptypin[0],21,23,26,38);
		Show_Pattern(&gImage_emptypin[0],27,29,26,38);
		Show_Pattern(&gImage_emptypin[0],33,35,26,38);
		Show_Pattern(&gImage_emptypin[0],39,41,26,38);
		Show_Pattern(&gImage_emptypin[0],45,47,26,38);
		clearArea(60,38,200,1);
		Display_arrow(0);
		Display_arrow(1);
		Asc8_16(124,52,"5");
		if(state)
				Asc8_16(226,26,"s");
		
		Key_Control(1);//清空按键标志位，开启按键有效
		Start_TIM(OLED_INPUT_TIME);
    while(1)
    {
				if(state)
				{
						if((Get_TIM(OLED_INPUT_TIME))%INPUT_TIME_DIV == 0)
						{		
								unsigned char temp = 30 - Get_TIM(OLED_INPUT_TIME)/INPUT_TIME_DIV;
								if(temp == 0)
								{
										Stop_TIM(OLED_INPUT_TIME);
										return 0;//超时
								}
								num[0] = temp/10+0x30;
								Asc8_16(210,26,num);
								num[0] = temp%10+0x30;
								Asc8_16(218,26,num);
						}
				}
        if(Key_Flag.flag.left)//左键按下
        {
						Key_Flag.flag.left = 0;
            if(code_array[index] == '0')
            {
								clearArea(124,52,8,16);
								Show_Pattern(&gImage_delete[0],31,32,48,64);
                code_array[index] = 0xFF;
            }
						else if(code_array[index] == 0xFF)
						{
								clearArea(124,48,8,16);
								Asc8_16(124,52,"9");
                code_array[index] = '9';								
						}
            else
            {
                num[0] = --code_array[index];
								Asc8_16(124,52,num);
            }
        }
        if(Key_Flag.flag.right)//右键按下
        {
						Key_Flag.flag.right = 0;
            if(code_array[index] == '9')
            {
								clearArea(124,52,8,16);
								Show_Pattern(&gImage_delete[0],31,32,48,64);
                code_array[index] = 0xFF;
            }
						else if(code_array[index] == 0xFF)
						{
								clearArea(124,48,8,16);
								Asc8_16(124,52,"0");
                code_array[index] = '0';		
						}
            else
            {
                num[0] = ++code_array[index];
								Asc8_16(124,52,num);
            }
        }
        if(Key_Flag.flag.middle)//中间建按下
        {
						Key_Flag.flag.middle = 0;
						if(code_array[index] == 0xFF)
						{
								code_array[index] = '5';
								if(index>0)
										index--;
								code_array[index] = '5';
								Show_Pattern(&gImage_emptypin[0],15+6*index,17+6*index,26,38);
								clearArea(58+index*24,38,16,1);
						}
						else
						{
								Show_Pattern(&gImage_fullpin[0],15+6*index,17+6*index,26,38);
								clearArea(58+index*24,38,16,1);
								index++;
						}
            if(index == 6)
            {
								break;
            }
            else
            {
								Asc8_16(124,52,"5");
            }
        }
    }
		Key_Control(0);//清空按键标志位，开启按键无效
		return 1;	
}
/************************************************
设置密码
返回值： 
				0  设置成功
				1  加密芯片随机数出错  
				2  DES加密出错
				3  加密芯片写入出错
*************************************************/
uint8_t setCode(void)
{
		uint8_t pin[8];		
		uint8_t random[32];
		uint8_t MingWen[32];
		uint32_t Len_Out = 0;
		uint32_t crc = 0;
		
		SetCode(pin);								//得到用户输入的有效6位PIN码
		crc = Utils_crc32(0,pin,6);//组合成8位的PIN码
		pin[6] = crc & 0xff;
		pin[7] = (crc>>8) & 0xff;
		
		memset(random,0,32);
		memset(MingWen,0,32);
		if(ATSHA_get_random(random) == 0)
		{
				Asc8_16(176,9,(uint8_t *)"ERROR!!!");
				HAL_Delay(1000);
				return 1;
		}
		crc = Utils_crc32(0,random,30);
		random[30] = crc & 0xff;
		random[31] = (crc>>8) & 0xff;
		if(My_DES_Encrypt(random,32,pin,MingWen,&Len_Out))
		{
#ifdef printf_debug			
				printf("My_DES_Encrypt \n");
#endif			
				Asc8_16(176,9,(uint8_t *)"ERROR!!!");
				HAL_Delay(1000);
				return 2;
		}
		if(ATSHA_write_data_slot(0,0,MingWen,32) == 0)
		{
#ifdef printf_debug			
				printf("ATSHA_write_data_slot \n");
#endif			
				Asc8_16(176,9,(uint8_t *)"ERROR!!!");
				HAL_Delay(1000);
				return 3;
		}	
		else
		{
				Asc8_16(176,9,(uint8_t *)"OK");
				HAL_Delay(1000);
				return 0;
		}
}
/************************************************
验证密码
返回值： 
				0  验证成功
				1  加密芯片读取出错  
				2  DES解密出错
				3  DES解密出错
				4	 crc出错
				5  超时
*************************************************/
uint8_t verifyCode(uint8_t state)
{
		uint8_t pin[8];//用户输入的PIN码
		uint8_t MingWen[32];
		uint8_t AnWen[32];
		uint32_t Len_Out = 0;
		uint32_t crc = 0;
		
		if(VerifyCode(pin,state) == 0)		//得到用户输入的有效6位PIN码
				return 5;
		crc = Utils_crc32(0,pin,6);//组合成8位的PIN码
		pin[6] = crc & 0xff;
		pin[7] = (crc>>8) & 0xff;
		
		memset(AnWen,0,32);
		memset(MingWen,0,32);
		if(ATSHA_read_data_slot(0,MingWen) == 0)
		{
#ifdef printf_debug			
				printf("ATSHA_read_data_slot \n");
#endif			
				Asc8_16(176,9,"ERROR!!!");
				HAL_Delay(1000);
				return 1;
		}
		if(My_DES_Decrypt(MingWen,32,pin,AnWen,&Len_Out))
		{
#ifdef printf_debug			
				printf("My_DES_Decrypt \n");
#endif					
				Asc8_16(176,9,"ERROR!!!");
				HAL_Delay(1000);
				return 2;
		}
		if(Len_Out != 32)
		{
#ifdef printf_debug			
				printf("My_DES_Decrypt Len_Out ERROR \n");
#endif					
				Asc8_16(176,9,"ERROR!!!");
				HAL_Delay(1000);
				return 3;
		}
		if(Utils_verifycrc(AnWen,32))
		{
				Asc8_16(176,9,"OK");
				HAL_Delay(1000);
				return 0;
		}
		else
		{
#ifdef printf_debug			
				printf("VerifyCrc ERROR \n");
#endif
				Asc8_16(176,9,"ERROR!!!");
				HAL_Delay(1000);
				return 4;
		}
}

uint8_t verifyCodeGetPin(uint8_t state,uint8_t PinCode[8])
{
		uint8_t pin[8];//用户输入的PIN码
		uint8_t MingWen[32];
		uint8_t AnWen[32];
		uint32_t Len_Out = 0;
		uint32_t crc = 0;
		
		if(VerifyCode(pin,state) == 0)		//得到用户输入的有效6位PIN码
				return 5;
		crc = Utils_crc32(0,pin,6);//组合成8位的PIN码
		pin[6] = crc & 0xff;
		pin[7] = (crc>>8) & 0xff;
		memmove(PinCode,pin,8);		
				
		memset(AnWen,0,32);
		memset(MingWen,0,32);
		if(ATSHA_read_data_slot(0,MingWen) == 0)
		{
#ifdef printf_debug			
				printf("ATSHA_read_data_slot \n");
#endif			
				Asc8_16(176,9,"ERROR!!!");
				HAL_Delay(1000);
				return 1;
		}
		if(My_DES_Decrypt(MingWen,32,pin,AnWen,&Len_Out))
		{
#ifdef printf_debug			
				printf("My_DES_Decrypt \n");
#endif					
				Asc8_16(176,9,"ERROR!!!");
				HAL_Delay(1000);
				return 2;
		}
		if(Len_Out != 32)
		{
#ifdef printf_debug			
				printf("My_DES_Decrypt Len_Out ERROR \n");
#endif					
				Asc8_16(176,9,"ERROR!!!");
				HAL_Delay(1000);
				return 3;
		}
		if(Utils_verifycrc(AnWen,32))
		{
				Asc8_16(176,9,"OK");
				HAL_Delay(1000);
				return 0;
		}
		else
		{
#ifdef printf_debug			
				printf("VerifyCrc ERROR \n");
#endif
				Asc8_16(176,9,"ERROR!!!");
				HAL_Delay(1000);
				return 4;
		}
}
/************************************************
新钱包加密数据
参数：
				data			要加密的私钥
				len				私钥的长度
				num_slot	要写入的slotID
返回值： 
				0  加密数据写入成功
				1  加密芯片mac指令出错
				2  AES加密出错
				3  AES加密出错
				4	 加密芯片写入数据出错
				5  DES加密出错
*************************************************/
uint8_t EncryptDataNew(uint8_t *data,int len,uint8_t num_slot,uint8_t PinCode[8])
{
		uint8_t A[30],B[32],D[32],U[64];
		uint32_t crc = 0;
		uint8_t key[32];
		uint8_t MingWenAB[64];
		uint8_t MingWenKey[32];
		uint32_t Len_Out = 0;
		
		memset(A,0,30);
		memset(B,0,32);
		memset(D,0,32);
		memset(U,0,64);
		memset(MingWenAB,0,64);
		memset(MingWenKey,0,32);
		memset(key,0,32);
		
		RandomArray(A,30);
		memmove(U,A,30);
		RandomArray(B,32);
		if(ATSHA_mac(15,B,D) == 0)
		{
				return 1;
		}
		memmove(U+30,D,32);
		crc = Utils_crc32(0,U,62);
		U[62] = crc & 0xff;
		U[63] = (crc >> 8) & 0xff;
		SHA256_Data(U,64,key,32);
		if(STM32_AES_ECB_Encrypt(data,len,key,MingWenKey,&Len_Out))
		{
				return 2;
		}
		if(len != Len_Out)
				return 3;
		if(ATSHA_write_data_slot(num_slot,0,MingWenKey,Len_Out) == 0)
				return 4;
		
		Len_Out = 0;
		memset(U,0,64);
		memmove(U,A,30);
		memmove(U+30,B,32);
		crc = Utils_crc32(0,U,62);
		U[62] = crc & 0xff;
		U[63] = (crc >> 8) & 0xff;
		if(My_DES_Encrypt(U,64,PinCode,MingWenAB,&Len_Out))
		{
				return 5;
		}
		if(Len_Out != 64)
				return 5;
		if(ATSHA_write_data_slot(1,0,MingWenAB,32) == 0)
				return 4;
		if(ATSHA_write_data_slot(2,0,MingWenAB+32,32) == 0)
				return 4;
		return 0;
}
/************************************************
旧钱包加密数据
参数：
				data			要加密的私钥
				len				私钥的长度
				num_slot	要写入的slotID
返回值： 
				0  加密数据写入成功
				1  加密芯片读取数据出错
				2  DES解密出错
				3  数据CRC出错
				4	 加密芯片mac指令出错
				5  AES加密出错
				6  加密芯片写入数据出错
*************************************************/
uint8_t EncryptData(uint8_t *data,int len,uint8_t num_slot,uint8_t PinCode[8])
{
		uint8_t A[32],B[32],D[32],U[64];
		uint32_t crc = 0;
		uint8_t AnWenAB[64];
		uint8_t MingWenKey[32];
		uint8_t key[32];
		uint32_t Len_Out = 0;

		
		memset(A,0,32);
		memset(B,0,32);
		memset(D,0,32);
		memset(key,0,32);
		memset(U,0,64);
		memset(AnWenAB,0,64);
		memset(MingWenKey,0,32);
		
		if(ATSHA_read_data_slot(1,A) == 0)
				return 1;
		if(ATSHA_read_data_slot(2,B) == 0)
				return 1;
		memmove(U,A,32);
		memmove(U+32,B,32);
		if(My_DES_Decrypt(U,64,PinCode,AnWenAB,&Len_Out))
		{
				return 2;
		}
		if(Len_Out != 64)
				return 2;
		if(Utils_verifycrc(AnWenAB,64) == 0)
				return 3;

		memmove(A,AnWenAB,30);
		memmove(B,AnWenAB+30,32);
		memset(U,0,64);
		memmove(U,A,30);

		if(ATSHA_mac(15,B,D) == 0)
		{
				return 4;
		}		
		memmove(U+30,D,32);
		crc = Utils_crc32(0,U,62);
		U[62] = crc & 0xff;
		U[63] = (crc >> 8) & 0xff;
		SHA256_Data(U,64,key,32);
		
		Len_Out = 0;
		if(STM32_AES_ECB_Encrypt(data,len,key,MingWenKey,&Len_Out))
		{
				return 5;
		}
		if(len != Len_Out)
				return 5;
		if(ATSHA_write_data_slot(num_slot,0,MingWenKey,Len_Out) == 0)
				return 6;
		return 0;
}
/************************************************
钱包解密数据
参数：
				data			要解密的私钥明文
				len				明文的长度
				Output		输出私钥数组
返回值： 
				0  加密数据写入成功
				1  加密芯片读取数据出错
				2  DES解密出错
				3  数据CRC出错
				4	 加密芯片mac指令出错
				5  AES解密出错
*************************************************/
uint8_t DecryptData(uint8_t *data,int len,uint8_t Pin[8],uint8_t *Output)
{
		uint8_t A[32],B[32],D[32],U[64];
		uint32_t crc = 0;
		uint8_t AnWenAB[64];
		uint8_t key[32];
		uint32_t Len_Out = 0;
		
		memset(A,0,32);
		memset(B,0,32);
		memset(D,0,32);
		memset(key,0,32);
		memset(U,0,64);
		memset(AnWenAB,0,64);
		
		if(ATSHA_read_data_slot(1,A) == 0)
				return 1;
		if(ATSHA_read_data_slot(2,B) == 0)
				return 1;
		memmove(U,A,32);
		memmove(U+32,B,32);
		if(My_DES_Decrypt(U,64,Pin,AnWenAB,&Len_Out))
		{
				return 2;
		}
		if(Len_Out != 64)
				return 2;
		if(Utils_verifycrc(AnWenAB,64) == 0)
				return 3;

		memmove(A,AnWenAB,30);
		memmove(B,AnWenAB+30,32);
		memset(U,0,64);
		memmove(U,A,30);

		if(ATSHA_mac(15,B,D) == 0)
		{
				return 4;
		}		
		memmove(U+30,D,32);
		crc = Utils_crc32(0,U,62);
		U[62] = crc & 0xff;
		U[63] = (crc >> 8) & 0xff;
		SHA256_Data(U,64,key,32);
		
		if(STM32_AES_ECB_Decrypt(data,len,key,Output,&Len_Out))
		{
				return 5;
		}
		if(len != Len_Out)
				return 5;		
		return 0;
}


