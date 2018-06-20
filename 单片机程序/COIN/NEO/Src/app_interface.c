#include "app_interface.h"
#include <string.h>
#include <stdio.h>
#include "atsha204a.h"
#include "app_des.h"
#include "app_aes.h"
#include "ecdsa.h"

static const uint32_t  POLYNOMIAL = 0xEDB88320;
static uint8_t have_table = 0;
static uint32_t table[256];

static void make_table()    //表是全局的
{
		int i, j;
		have_table = 1;
		for (i = 0; i < 256; i++)
				for (j = 0, table[i] = i; j < 8; j++)
						table[i] = (table[i] >> 1) ^ ((table[i] & 1) ? POLYNOMIAL : 0);
}

uint32_t Utils_crc32(uint32_t crc, uint8_t* buff, int len)
{
		if (!have_table) make_table();
				crc = ~crc;
		for (int i = 0; i < len; i++)
				crc = (crc >> 8) ^ table[(crc ^ buff[i]) & 0xff];
		return ~crc;		
}

uint8_t	Utils_verifycrc(uint8_t data[],int len)
{
		uint32_t res = Utils_crc32(0, data, len - 2);	
		if ((data[len - 2] == (res & 0xff)) && (data[len - 1] == ((res >> 8) & 0xff))) 
		{
				return 1;
		} 
		else
		{
				return 0;	//校验失败
		}
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
		int sha_len = 0;
		
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
		Alg_HashData(U,64,key,&sha_len);
		
		if(STM32_AES_ECB_Decrypt(data,len,key,Output,&Len_Out))
		{
				return 5;
		}
		if(len != Len_Out)
				return 5;		
		return 0;
}

/***************************************************
功能：
		SlotID为地址所对应的槽号
		得到地址所对应的私钥
***************************************************/
uint8_t	Get_Privekey(uint8_t SlotID,uint8_t pin[8],uint8_t Privekey[32])
{
		uint8_t slot_data[32];
		uint8_t slot_ID_read = SlotID - 5;
		ATSHA_read_data_slot(slot_ID_read,slot_data);
		return (DecryptData(slot_data,32,pin,Privekey));
}





