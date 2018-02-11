/*
 * Utils.cpp
 *
 *  Created on: 2017年7月30日
 *      Author: Administrator
 */

#include "Utils.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "crypto.h"
#include "HASH/SHA256/sha256.h"

Utils::Utils() {
}

void Utils::WriteU16(u16 data, u8* des) 
{
		des[0] = data & 0xff;
		des[1] = (data>>8) & 0xff;
}

void Utils::WriteU32(u32 data, u8* des) 
{
		des[0] = data & 0xff;
		des[1] = (data>>8) & 0xff;
		des[2] = (data>>16) & 0xff;
		des[3] = (data>>24) & 0xff;
}

u16 Utils::ReadU16(u8* des) 
{
		return	des[0] | (des[1]<<8);
}

u16  Utils::ReverseU16(u16 data) 
{
		return  ((data&0xff )<<8)|((data>>8 )& 0xff);
}


u32 Utils::ReadU32(u8* des) 
{
		return	des[0] | (des[1]<<8)| (des[2]<<16)| (des[3]<<24);
}

static const u32  POLYNOMIAL = 0xEDB88320;
bool have_table = false;
u32 table[256];


static void make_table()    //表是全局的
{
		int i, j;
		have_table = true;
		for (i = 0; i < 256; i++)
				for (j = 0, table[i] = i; j < 8; j++)
						table[i] = (table[i] >> 1) ^ ((table[i] & 1) ? POLYNOMIAL : 0);
}


u32 Utils::Crc32(u32 crc, u8* buff, int len) 
{
		if (!have_table) make_table();
				crc = ~crc;
		for (int i = 0; i < len; i++)
				crc = (crc >> 8) ^ table[(crc ^ buff[i]) & 0xff];
		return ~crc;
}

u32  Utils::RandomInteger() 
{
		return HAL_GetTick() ;
}

void  Utils::PrintArray(u8 data[], int len) 
{
		for(int i = 0 ; i<len ; i ++)
		{
				printf("%02x ",data[i]);
		}
		printf("\r\n");
}

void Utils::Sha256(u8* input, u32 inLen, u8* outPut, u32 outLen) 
{
		//uint8_t *message  = input;
		//uint8_t *digest = outPut ; //[CRL_SHA256_SIZE]; // 32 byte buffer
		// outSize is for digest output size, retval is for return value
		int32_t outSize, retval;
		SHA256ctx_stt context_st; // The SHA256 context

		// Initialize Context Flag with default value
		context_st.mFlags = E_HASH_DEFAULT;
		// Set the required digest size to be of 32 byte
		context_st.mTagSize = CRL_SHA256_SIZE;

		// call init function
		retval = SHA256_Init(&context_st);
		if (retval != HASH_SUCCESS) 
		{
				printf("256init failed\r\n");
		}

		retval = SHA256_Append(&context_st, input   , inLen);
		if (retval != HASH_SUCCESS) 
		{
				printf("256 apd failed\r\n");
		}

		//Generate the message digest
		retval = SHA256_Finish(&context_st, outPut, &outSize);
		if (retval != HASH_SUCCESS) 
		{
				printf("256 fnsh failed\r\n");
		}
}

void Utils::PrintString(char* data, int len) 
{
		for(int i = 0 ; i< len ; i++)
		{
				printf("%c",data[i]);
		}
		printf("\r\n" );
}

bool Utils::MemoryCompare(void* left, void* right, int size) 
{
		for(int i = 0 ; i < size ; i ++)
		{
				if( ((u8*)left)[i]  != ((u8*)right)[i] )
				{
						return false;
				}
		}
		return true;
}

bool Utils::PassportMemoryCompare(u32 *left, u32 *right, int size) 
{
		for(int i = 0 ; i < size ; i ++)
		{
				if( left[i]  != right[i] )
				{
						return false;
				}
		}
		return true;
}

void Utils::HexToNum(u8 *src,int len, char *dec)
{
		for(int i=0;i<len;i++)
		{
				dec[2*i] = (src[i])/16 + 0x30;
				dec[2*i+1] = (src[i])%16 + 0x30;
		}
}


