#include "cmd.h"
#include <string.h>
#include "usbd_customhid.h"
#include "usb_device.h"

static const u32  POLYNOMIAL = 0xEDB88320;
int have_table = 0;
u32 table[256];

//Commands::Commands(u16 cmd, u16 serial) 
//{
//	index = 0 ;
//	memset(data,0,sizeof(data));
//	this->AppendU16(ReverseU16(cmd));
//	this->AppendU16(serial );
//}

//void CreateDataQuest(u16 cmd, u16 serial, u16 firstIndex, u16 lastIndex, u8* hash, u32 hashLen) 
//{
//	memset(data,0,sizeof(data));
//	WriteU16( Utils::ReverseU16(cmd)  , this->data + index);index +=2;
//	WriteU16( serial , this->data + index);index +=2;
//	WriteU16( firstIndex , this->data + index);index +=2;
//	WriteU16( lastIndex , this->data + index);index +=2;
//	memmove(data+index , hash , hashLen ); index += hashLen;
//}

//void AppendU16(u16 data) 
//{
//	WriteU16( data , data + index);
//	index +=2;
//}

//void AppendU32(u16 data) 
//{
//	WriteU32( data , data + index);
//	index +=4;
//}

//void AppendBytes(u8* data, int len) 
//{
//	memmove(data + index,data,len);
//	index += len;
//}

void CreateDataQuest(u16 cmd, u16 serial,u8* data) 
{
		int index_cmd = 0;
		memset(data,0,64);
		WriteU16( ReverseU16(cmd) , data + index_cmd);
		index_cmd +=2;
		WriteU16( serial , data + index_cmd);
		index_cmd +=2;
		
}

void SendToPc(u8* data) 
{
	//发送之前crc一下
	u32 crc = Crc32(0,data,62);
	data[62] = crc & 0xff;
	data[63] = (crc>>8) & 0xff;
	//printf("ack crc = 0x%x\r\n",crc);
	//Utils::PrintArray(data,CMD_DATA_PACK_LEN);
	SendUSBData( data,64 );
}

void make_table(void)    
{
	int i, j;
	have_table = 1;
	for (i = 0; i < 256; i++)
			for (j = 0, table[i] = i; j < 8; j++)
					table[i] = (table[i] >> 1) ^ ((table[i] & 1) ? POLYNOMIAL : 0);
}


u32 Crc32(u32 crc, u8* buff, int len) 
{
	int i=0;
	if (!have_table) make_table();
			crc = ~crc;
	for (i = 0; i < len; i++)
			crc = (crc >> 8) ^ table[(crc ^ buff[i]) & 0xff];
	return ~crc;
}

void  PrintArray(u8 data[], int len) 
{
	int i = 0;
	for(i = 0 ; i<len ; i ++)
	{
		printf("%02x ",data[i]);
	}
	printf("\r\n");
}

void WriteU16(u16 data, u8* des) 
{
	des[0] = data & 0xff;
	des[1] = (data>>8) & 0xff;
}

void WriteU32(u32 data, u8* des) 
{
	des[0] = data & 0xff;
	des[1] = (data>>8) & 0xff;
	des[2] = (data>>16) & 0xff;
	des[3] = (data>>24) & 0xff;
}

u16 ReadU16(u8* des) 
{
	return	des[0] | (des[1]<<8);
}

u16 ReverseU16(u16 data) 
{
	return  ((data&0xff )<<8)|((data>>8 )& 0xff);
}


u32 ReadU32(u8* des) 
{
	return	des[0] | (des[1]<<8)| (des[2]<<16)| (des[3]<<24);
}

//USB HID发送
void SendUSBData(u8 *data , int len)
{
		USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,data,len);
		USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)&hUsbDeviceFS;
		while(hhid->state == CUSTOM_HID_BUSY);
}

int crc_frame(u8 data[], int len) 
{
		if (len != 64)
				return 0;
		u32 res = Crc32(0, data, len - 2);		
		if ((data[len - 2] == (res & 0xff)) && (data[len - 1] == ((res >> 8) & 0xff))) 
		{
				return 1;
		} 
		else 
		{
				return 0;	//校验失败
		}
}

int MemoryCompare(void* left, void* right, int size) 
{
		int i = 0;
		for(i = 0 ; i < size ; i ++)
		{
				if( ((u8*)left)[i]  != ((u8*)right)[i] )
				{
						return 0;
				}
		}
		return 1;
}


