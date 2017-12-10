#ifndef _CMD_H
#define _CMD_H

#include "sys.h"

#define CMD_DATA_PACK_LEN      			64
#define CMD_CRC_FAILED						0x00e0		//CRC校验出错
#define	CMD_NOTIFY_DATA						0x0101		//
#define	CMD_PULL_DATA							0x0110		//
#define	CMD_PULL_DATA_REQ					0x01a2		//
#define	CMD_PULL_DATA_REQ_OVER		0x01a3		//
#define CMD_ERR_DATA_TOO_BIG			0x01e0		//
#define CMD_ERR_HASH							0x0112		//
#define CMD_ACK_HASH_OK						0x0111		//
#define CMD_DEL_ADDRESS						0x0203		
#define CMD_DEL_ADDRESS_OK				0x02c1
#define CMD_DEL_ADDRESS_FAILED		0x02c2
#define CMD_ADD_ADDRESS						0x0204		//
#define CMD_ADDR_ADD_OK						0x02b1		//
#define CMD_ADDR_ADD_FAILED				0x02b2		//
#define CMD_GET_PRIKEY						0x0206
#define CMD_RETURN_MESSAGE				0x02a4
#define CMD_QUERY_ADDRESS					0x0201		//查询设备地址情况
#define CMD_RETURN_ADDRESS				0x02a0		//返回一条地址
#define CMD_RETURN_ADDRESS_OK			0x02a1		//返回地址结束
//add by hkh
#define CMD_SIGN_DATA							0x020a
#define CMD_SIGN_OK								0x02a8
#define CMD_SIGN_FAILED						0x02e0
#define CMD_GET_ARRAY							0x0301
#define CMD_SEND_ARRAY						0x03a1
#define CMD_SET_FIRSTPASSPORT			0x0302
#define CMD_SET_OK								0x03b1
#define CMD_VERIFY_PASSPORT				0x0303
#define CMD_VERIFY_OK							0x03c1
#define CMD_VERIFY_ERROR					0x03c2


void CreateDataQuest(u16 cmd, u16 serial,u8* data);
void AppendU16(u16 data);
void AppendU32(u16 data);
void AppendBytes(u8 *data , int len);
void SendToPc(u8 *data);

void make_table(void);
u32 Crc32(u32 crc, u8* buff, int len);   
void  PrintArray(u8 data[], int len);
void WriteU16(u16 data, u8* des);
void WriteU32(u32 data, u8* des);
u16 ReadU16(u8* des);
u16 ReverseU16(u16 data);
u32 ReadU32(u8* des);
void SendUSBData(u8 *data , int len);
int crc_frame(u8 data[], int len);
int MemoryCompare(void* left, void* right, int size);

#endif


