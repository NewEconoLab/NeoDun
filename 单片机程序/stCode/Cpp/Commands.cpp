/*
 * Commands.cpp
 *
 *  Created on: 2017年7月15日
 *      Author: Administrator
 */

#include "Commands.h"
#include "usbd_customhid.h"
#include "Utils.h"
#include "Hal.h"
#include <string.h>

u8 Commands::data[CMD_DATA_PACK_LEN];

Commands::Commands(  ) 
{
		memset(data,0,sizeof(data));
		index = 0;
}

void Commands::CreateDataQuest(u16 cmd, u16 serial, u16 firstIndex, u16 lastIndex, u8* hash, u32 hashLen) 
{
		memset(data,0,sizeof(data));
		Utils::WriteU16( Utils::ReverseU16(cmd)  , this->data + index);index +=2;
		Utils::WriteU16( serial , this->data + index);index +=2;
		Utils::WriteU16( firstIndex , this->data + index);index +=2;
		Utils::WriteU16( lastIndex , this->data + index);index +=2;
		memmove(data+index , hash , hashLen ); index += hashLen;
}

void Commands::AppendU16(u16 data) 
{
		Utils::WriteU16( data , this->data + index);
		index +=2;
}

void Commands::AppendU32(u16 data) 
{
		Utils::WriteU32( data , this->data + index);
		index +=4;
}

void Commands::AppendBytes(u8* data, int len) 
{
		memmove(this->data + index,data,len);
		index += len;
}

Commands::Commands(u16 cmd, u16 serial) 
{
		index = 0 ;
		memset(data,0,sizeof(data));
		this->AppendU16(Utils::ReverseU16(cmd)  );
		this->AppendU16(serial );
}

void Commands::SendToPc() 
{
		//发送之前crc一下
		u32 crc = Utils::Crc32(0,this->data,62);
		data[62] = crc & 0xff;
		data[63] = (crc>>8) & 0xff;
		//printf("ack crc = 0x%x\r\n",crc);
		//Utils::PrintArray(data,CMD_DATA_PACK_LEN);
		Hal::getInstance().SendUSBData( data,64 );
}

