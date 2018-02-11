/*
 * Utils.h
 *
 *  Created on: 2017Äê7ÔÂ30ÈÕ
 *      Author: Administrator
 */

#ifndef CPP_UTILS_H_
#define CPP_UTILS_H_

#include "myType.h"


class Utils 
{
private:
		Utils();
public:
		static void WriteU16(u16 data , u8 *des);
		static void WriteU32(u32 data , u8 *des);
		static u16 ReadU16( u8 *des);
		static u32 ReadU32( u8 *des);
		static u16 ReverseU16( u16 data);
		static u32 Crc32(u32 crc,u8 *data,int len);
		static u32 RandomInteger( );
		static void Sha256(u8 * input , u32 inLen , u8 *outPut , u32 outLen );
		static void PrintArray(u8 data[] , int len );
		static void PrintString(char * data , int len );
		static bool MemoryCompare(void *left , void *right , int size);
		static bool PassportMemoryCompare(u32 *left , u32 *right , int size);
		static void HexToNum(u8 *src,int len, char *dec);
};


#endif /* CPP_UTILS_H_ */
