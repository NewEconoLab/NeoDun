#include "app_command.h"
#include <string.h>
#include "CMD.h"
#include "app_utils.h"
#include "app_hal.h"
#include "app_interface.h"
#include "atsha204a.h"
#include "OledMenu.h"
#include "Algorithm.h"
#include "app_oled.h"
#include "OLED281/oled281.h"
#include "stmflash.h"

extern uint8_t gImage_wait[128];
static uint8_t dataSave[HID_MAX_DATA_LEN];
static uint8_t packIndexRecord[(HID_MAX_DATA_LEN+DATA_PACK_SIZE-1)/DATA_PACK_SIZE ];

static uint8_t isAllPackGot(void)
{
		for (int i = 0; i < HidData.packCount; i++) 
		{
				if (!packIndexRecord[i]) 
				{
						return 0;
				}
		}
		return 1;
}

static void HID_NOTIFY_DATA_REP(uint8_t state,uint16_t serialID)
{
		uint32_t crc = 0;
		HidData.reqSerial = RandomInteger();
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x01;
		if(state)
		{			
				data_rp[1] = 0x10;
				data_rp[2] = HidData.reqSerial & 0xff;
				data_rp[3] = (HidData.reqSerial >> 8) & 0xff;
				data_rp[4] = 0;
				data_rp[5] = 0;
				data_rp[6] = (HidData.packCount - 1)&0xff;
				data_rp[7] = ((HidData.packCount - 1)>>8) & 0xff;
				memmove(data_rp+8,HidData.hashRecord,32);
		}
		else
		{
				data_rp[1] = 0xe0;
				data_rp[2] = serialID & 0xff;
				data_rp[3] = (serialID >> 8) & 0xff;
		}
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);		
}

static void HID_PULL_DATA_REP(uint16_t blockbegin,uint16_t blockend,uint16_t serialID)
{
		uint32_t	crc = 0;
		uint8_t 	data_rp[64];
		uint16_t 	i = 0;
		memset(data_rp,0,64);
		
		for(i=blockbegin;i<(blockend+1);i++)
		{
				data_rp[0] = 0x01;
				data_rp[1] = 0xa2;
				data_rp[2] = serialID & 0xff;
				data_rp[3] = (serialID >> 8) & 0xff;
				data_rp[4] = (uint8_t)(i&0xff); 
				data_rp[5] = (uint8_t)((i>>8)&0xff);
				memmove(data_rp+6,dataSave+i*DATA_PACK_SIZE,50);
				crc = Utils_crc32(0,data_rp,62);
				data_rp[62] = crc & 0xff;
				data_rp[63] = (crc >> 8) & 0xff;
				SendUSBData(data_rp,64);
				memset(data_rp,0,64);
#ifdef HID_Delay
				HAL_Delay(HID_SEND_DELAY);
#endif			
		}
		
		data_rp[0] = 0x01;
		data_rp[1] = 0xa3;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);
}

static void HID_PULL_DATA_REEQ_OVER_REP(uint8_t state,uint16_t packIndex,uint8_t *data)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x01;
		if(state)
				data_rp[1] = 0x11;
		else
				data_rp[1] = 0x12;
		data_rp[2] = HidData.notifySerial & 0xff;
		data_rp[3] = (HidData.notifySerial >> 8) & 0xff;
		
		data_rp[4] = (uint8_t)(packIndex & 0xff);
		data_rp[5] = (uint8_t)((packIndex >> 8) & 0xff);
		data_rp[6] = (uint8_t)(system_base_time&0xff);
		data_rp[7] = (uint8_t)((system_base_time>>8)&0xff);
		memmove(data_rp+8,data,32);
		
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);			
}

static void HID_PULL_LOST_DATA(uint16_t blockbegin,uint16_t blockend,uint16_t serialID)
{
		uint32_t	crc = 0;
		uint8_t 	data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x01;
		data_rp[1] = 0x10;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		data_rp[4] = (uint8_t)(blockbegin&0xff); 
		data_rp[5] = (uint8_t)((blockbegin>>8)&0xff);
		data_rp[6] = (uint8_t)(blockend&0xff); 
		data_rp[7] = (uint8_t)((blockend>>8)&0xff);
		memmove(data_rp+8,HidData.hashRecord,32);
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);
}

static void HID_QUERY_ADDRESS_ALL_REP(uint16_t serialID)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
				
		for(uint8_t i = 0;i < 5;i++)
		{
				if(showaddress[i].content[0] == 0)
						continue;
				if(showaddress[i].hide)
						continue;
				memset(data_rp,0,64);
				data_rp[0] = 0x02;
				data_rp[1] = 0xa0;
				data_rp[2] = serialID & 0xff;
				data_rp[3] = (serialID >> 8) & 0xff;
				data_rp[4] = i;
				data_rp[5] = 0;
				data_rp[6] = Neo_System.count;
				data_rp[7] = 0;
				data_rp[8] = (uint8_t)(ADDR_XiaoYi & 0xff);
				data_rp[9] = (uint8_t)((ADDR_XiaoYi >> 8) & 0xff);
				memmove(data_rp+10,showaddress[i].content,25);
				memmove(data_rp+36,showaddress[i].name,showaddress[i].len_name);
				
				crc = Utils_crc32(0,data_rp,62);
				data_rp[62] = crc & 0xff;
				data_rp[63] = (crc >> 8) & 0xff;
				SendUSBData(data_rp,64);
#ifdef HID_Delay
				HAL_Delay(HID_SEND_DELAY);
#endif				
		}
}

static void HID_QUERY_ADDRESS_REP(uint16_t serialID)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		data_rp[1] = 0xa1;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		data_rp[4] = Neo_System.count;
	
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);
}

static void HID_SET_ADDRESS_NAME_REP(uint8_t state,uint16_t serialID,uint16_t errCode)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		if(state)
				data_rp[1] = 0xa2;
		else
				data_rp[1] = 0xe2;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		data_rp[4] = (errCode >> 8) & 0xff;		
		data_rp[5] = errCode & 0xff;

		
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);
}

static void HID_DEL_ADDRESS_REP(uint16_t state,uint16_t serialID,uint16_t addressType)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		if(state == 0)
				data_rp[1] = 0xa3;
		else
		{
				data_rp[1] = 0xe3;
				data_rp[4] = (state >> 8)& 0xff;
				data_rp[5] = state & 0xff;			
		}
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);			
}

static void HID_ADD_ADDRESS_REP(uint16_t state,uint16_t serialID,uint16_t addressType,uint8_t *data)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		if(state == 1)
		{
				data_rp[1] = 0xa4;
		}
		else
		{
				data_rp[1] = 0xe4;
				data_rp[4] = (state >> 8)& 0xff;
				data_rp[5] = state & 0xff;
		}
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);		
}

static void HID_SIGN_DATA_FAILED_REP(uint16_t serialID)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		data_rp[1] = 0xe5;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);	
}

static void HID_SIGN_DATA_REP(uint16_t cmd,uint16_t serialID,uint32_t len_data,uint8_t *hash,uint8_t len_hash)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = (cmd >> 8) & 0xff;;
		data_rp[1] = cmd & 0xff;;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		data_rp[4] = len_data & 0xff;
		data_rp[5] = (len_data >> 8) & 0xff;
		data_rp[7] = (len_data >> 16) & 0xff;	
		data_rp[7] = (len_data >> 24) & 0xff;
		memmove(data_rp+8,hash,len_hash);

		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);
}

static void HID_SET_INFO_REP(uint16_t serialID)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		data_rp[1] = 0xd3;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);			
}

static void HID_CRC_ERROR_REP(uint16_t serialID)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x00;
		data_rp[1] = 0x10;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);
}

void Deal_Sign_Data_Restart(void)
{
		uint16_t serialId = 0;	
		uint8_t AddressID = 0;
		uint8_t	sign_cmd_data[32];
		uint8_t	SIGN_Out_Para_data[136];
		SIGN_Out_Para Sign;	
		uint8_t resultsignRecord[98];
		uint8_t hash_sign[32];
		memset(&Sign,0,sizeof(Sign));
		if(STMFLASH_ReadWord(FLASH_ADDRESS_SIGN_DATA) != 0)
		{
				//恢复现场
				serialId = (uint16_t)STMFLASH_ReadWord(FLASH_ADDRESS_SCENE);
				AddressID = (uint8_t)STMFLASH_ReadWord(FLASH_ADDRESS_SCENE+4);
				STMFLASH_Read_ByteArray(FLASH_ADDRESS_SCENE+8,sign_cmd_data,32);
				//从FLASH中得到数据
				STMFLASH_Read_ByteArray(FLASH_ADDRESS_SIGN_DATA,SIGN_Out_Para_data,136);
				STMFLASH_Read_ByteArray(FLASH_ADDRESS_SIGN_DATA+0x800,resultsignRecord,98);
				STMFLASH_Erase_Sectors(FLASH_DATA_SECTOR);
				//组合数据
				{
						Sign.type							= SIGN_Out_Para_data[0];
						Sign.version 					= SIGN_Out_Para_data[1];
						Sign.coin							= SIGN_Out_Para_data[2];
						Sign.countAttributes  = SIGN_Out_Para_data[3];
						Sign.countInputs			= SIGN_Out_Para_data[4];
						Sign.countOutputs			= SIGN_Out_Para_data[5];
						memmove(Sign.address[0],SIGN_Out_Para_data+6,25);
						memmove(Sign.address[1],SIGN_Out_Para_data+31,25);
						memmove(Sign.assetid[0],SIGN_Out_Para_data+56,32);
						memmove(Sign.assetid[1],SIGN_Out_Para_data+88,32);
						Sign.money[0] = (uint64_t)SIGN_Out_Para_data[120]|((uint64_t)SIGN_Out_Para_data[121]<<8)
														|((uint64_t)SIGN_Out_Para_data[122]<<16)|((uint64_t)SIGN_Out_Para_data[123]<<24)
														|((uint64_t)SIGN_Out_Para_data[124]<<32)|((uint64_t)SIGN_Out_Para_data[125]<<40)
														|((uint64_t)SIGN_Out_Para_data[126]<<48)|((uint64_t)SIGN_Out_Para_data[127]<<56);
						Sign.money[1] = (uint64_t)SIGN_Out_Para_data[128]|((uint64_t)SIGN_Out_Para_data[129]<<8)
														|((uint64_t)SIGN_Out_Para_data[130]<<16)|((uint64_t)SIGN_Out_Para_data[131]<<24)
														|((uint64_t)SIGN_Out_Para_data[132]<<32)|((uint64_t)SIGN_Out_Para_data[133]<<40)
														|((uint64_t)SIGN_Out_Para_data[134]<<48)|((uint64_t)SIGN_Out_Para_data[135]<<56);
				}
				SHA256_Data(resultsignRecord,98,hash_sign,32);
				
				if(Sign.type != 0x80)//不是合约交易
				{
						if(Display_Sign_Data_Type_Identify() == 0)//用户拒绝签名或者操作超时
						{
								HID_SIGN_DATA_FAILED_REP(serialId);
								return;
						}
				}
				uint8_t index_add;
				char Sign_address[40] = "";
				int len_add = 0;

				if(Sign.countOutputs == 0)		 //输出端地址为0个
				{
						HID_SIGN_DATA_FAILED_REP(serialId);
						return;														
				}
				else if(Sign.countOutputs == 1)//输出端地址为1个
				{
						index_add = 0;
				}
				else if(Sign.countOutputs == 2)//输出端地址为2个
				{
						if(ArrayCompare(sign_cmd_data,Sign.address[0],25))
								index_add = 1;
						else if(ArrayCompare(sign_cmd_data,Sign.address[1],25))
								index_add = 0;
						else											 //本机没有存有该地址
						{
								HID_SIGN_DATA_FAILED_REP(serialId);
								return;
						}
				}
				else//多个签名的情况
				{
						index_add = 0xff;
				}

				Alg_Base58Encode(Sign.address[index_add] , 25 ,Sign_address,&len_add);
				if(Display_SignData(&Sign,&showaddress[AddressID-1],index_add))
				{
						memset(dataSave,0,HID_MAX_DATA_LEN);
						memmove(dataSave,resultsignRecord,98);
						HidData.reqSerial = RandomInteger();
						HID_SIGN_DATA_REP(CMD_NOTIFY_DATA,HidData.reqSerial,98,hash_sign,32);
#ifdef HID_Delay
						HAL_Delay(HID_SEND_DELAY);
#endif
						HID_SIGN_DATA_REP(CMD_SIGN_OK,serialId,98,hash_sign,32);
				}
				else//拒绝签名
				{
						HID_SIGN_DATA_FAILED_REP(serialId);	
				}
		}
		else
		{
				HID_SIGN_DATA_FAILED_REP(serialId);
		}
}

static void HID_INSTALL_PACK_REP(uint16_t state,uint16_t serialID,uint16_t install_type,uint16_t install_cont,uint16_t version)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x03;
		if(state == 0)
		{			
				data_rp[1] = 0xa1;
		}
		else
		{
				data_rp[1] = 0xe1;
				data_rp[4] = (state >> 8) & 0xff;
				data_rp[5] = state & 0xff;
		}

		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);
}	

static void HID_INSTALL_REQUEST_REP(uint8_t state,uint16_t serialID)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x03;
		if(state)	
				data_rp[1] = 0xa2;
		else
				data_rp[1] = 0xe2;

		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;

		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);
}

static void HID_QUERY_PACK_INFO_REP(uint16_t serialID)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x03;	
		data_rp[1] = 0xa4;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		data_rp[4] = VERSION_NEODUN & 0xff;
		data_rp[5] = (VERSION_NEODUN >> 8) & 0xff;		
		
		data_rp[6] = coinrecord.coin1;
		data_rp[7] = coinrecord.version1;
		data_rp[8] = coinrecord.coin2;
		data_rp[9] = coinrecord.version2;
		data_rp[10] = coinrecord.coin3;
		data_rp[11] = coinrecord.version3;
		data_rp[12] = coinrecord.coin4;
		data_rp[13] = coinrecord.version4;
		data_rp[14] = coinrecord.coin5;
		data_rp[15] = coinrecord.version5;
	
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);
}

static void HID_UNINSTALL_PACK_REP(uint16_t state,uint16_t serialID,uint16_t install_type)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x03;
		if(state == 0)
				data_rp[1] = 0xa3;
		else
		{
				data_rp[1] = 0xe3;
				data_rp[4] = (state >> 8) & 0xff;				
				data_rp[5] = state & 0xff;
		}
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
	
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);			
}

void Hid_Data_Analysis(uint8_t data[],int len)
{
		uint16_t cmd 			= (data[0] << 8) | data[1];
		uint16_t serialId = data[2] | (data[3] << 8);
		if(Utils_verifycrc(data,len))
		{
				if(Passport_Flag.flag.poweron)
				{
						switch(cmd)
						{
								case CMD_NOTIFY_DATA:					//0x0101
								{
										memset(dataSave,0,sizeof(dataSave));
										memset(packIndexRecord,0,sizeof(packIndexRecord));
										HidData.dataLen = data[4] | (data[5]<<8) | (data[6]<<16) | (data[7]<<24);
										HidData.notifySerial = serialId;
										if(HidData.dataLen < HID_MAX_DATA_LEN)
										{
												HidData.packCount = (HidData.dataLen + DATA_PACK_SIZE - 1) / DATA_PACK_SIZE;
												HidData.packIndex = 0;
												memmove(HidData.hashRecord, data+8, 32);
												HID_NOTIFY_DATA_REP(1,serialId);
										}
										else
										{
												HID_NOTIFY_DATA_REP(0,serialId);
										}
										break;
								}
								case CMD_PULL_DATA_REQ:				//0x01a2
								{
										if(serialId == HidData.reqSerial)
										{
												memmove(dataSave + HidData.packIndex * DATA_PACK_SIZE, data + 6,DATA_PACK_SIZE);
												packIndexRecord[HidData.packIndex] = 1;
												HidData.packIndex++;
										}
										break;
								}
								case CMD_PULL_DATA:						//0x0110
								{
										uint16_t blockbegin = data[4] | (data[5]<<8);
										uint16_t blockend   = data[6] | (data[7]<<8);
										HID_PULL_DATA_REP(blockbegin,blockend,serialId);
										break;
								}
								case CMD_PULL_DATA_REQ_OVER:	//0x01a3
								{
										if(isAllPackGot())
										{
												uint8_t res[32];
												SHA256_Data(dataSave, HidData.dataLen , res, 32);									
												if(ArrayCompare(res,HidData.hashRecord,32))
												{
														HID_PULL_DATA_REEQ_OVER_REP(1,HidData.packIndex,res);
												}
												else
												{
														HID_PULL_DATA_REEQ_OVER_REP(0,HidData.packIndex,res);
												}
										}
										else
										{
												for (int i = 0; i < HidData.packCount; i++) 
												{
														if (packIndexRecord[i] == 0)//把这一包拉下来
														{
																HID_PULL_LOST_DATA(i,i,HidData.reqSerial);
														}
												}
										}
										break;
								}
								case CMD_QUERY_ADDRESS:				//0x0201
								{
										HID_QUERY_ADDRESS_ALL_REP(serialId);
										HID_QUERY_ADDRESS_REP(serialId);
										break;
								}
								case CMD_SET_ADDRESS_NAME:		//0x0202
								{
										uint8_t i = 0;
										uint8_t slot_data[32];
										uint8_t	temp_name[6];
										uint8_t  AddressID = Get_Address_ID(data+4);
										uint16_t len_name  = data[30] | (data[31] << 8);
										memset(temp_name,0,6);
										if((AddressID == 0)||(len_name > 6)||(len_name == 0))
										{
												HID_SET_ADDRESS_NAME_REP(0,serialId,0x0206);
												break;
										}
										//地址重名判断
										for(i = 0; i<5; i++)
										{
												if(showaddress[i].len_name == 0)
														continue;
												if(ArrayCompare(data+32,(uint8_t*)showaddress[i].name,len_name))
												{
														HID_SET_ADDRESS_NAME_REP(0,serialId,0x0203);
														return;
												}
										}
										//更新内存中的地址属性
										memmove(temp_name,data+32,len_name);
										memmove(showaddress[AddressID-1].name,temp_name,6);
										showaddress[AddressID-1].len_name = len_name;
										//更新到加密芯片中
										ATSHA_read_data_slot(AddressID+7,slot_data);
										memmove(slot_data+25,temp_name,6);
										slot_data[31] |= (len_name<<4);
										ATSHA_write_data_slot(AddressID+7,0,slot_data,32);
										HID_SET_ADDRESS_NAME_REP(1,serialId,0);
										break;
								}
								case CMD_DEL_ADDRESS:					//0x0203
								{
										uint16_t addressType = data[4] | (data[5] << 8);
										if(addressType == ADDR_XiaoYi)
										{
												uint8_t addressID = Get_Address_ID(data+6);
												uint8_t data_write[32];
												memset(data_write,0,32);
#ifdef printf_debug
												printf("addressID: %d\r\n",addressID);
#endif
												if(addressID)
												{
														if(Display_DelAdd(addressID-1) == 0)//超时或者拒绝删除
														{
																HID_DEL_ADDRESS_REP(0x0401,serialId,addressType);
																Display_Usb();
																break;
														}
														if(Set_Flag.flag.del_address)
														{
																if(Display_VerifyCode() == 0)
																{
																		Passport_Flag.flag.del_address = 1;
																}
														}
														else
														{
																if(Passport_Flag.flag.del_address == 0)
																{
																		if(Display_VerifyCode() == 0)
																		{
																				Passport_Flag.flag.del_address = 1;
																		}
																}
														}
														if(Passport_Flag.flag.del_address)
														{
																//将该地址ID对应的私钥和地址槽，都写入0
																ATSHA_write_data_slot(addressID+2,0,data_write,32);
																ATSHA_write_data_slot(addressID+7,0,data_write,32);
																Neo_System.count--;
																Updata_SYS_Count(Neo_System.count);				//更新计数的值
																memset(&showaddress[addressID-1],0,sizeof(ADDRESS));
															
																if(Set_Flag.flag.del_address)//是否重复进行删除地址时的密码验证，1为需要，0为不需要
																		Passport_Flag.flag.del_address = 0;
																else
																		Passport_Flag.flag.del_address = 1;		
																Display_Usb();															
																HID_DEL_ADDRESS_REP(0,serialId,addressType);
														}
														else//密码错误
														{
																if(Set_Flag.flag.del_address)//是否重复进行删除地址时的密码验证，1为需要，0为不需要
																		Passport_Flag.flag.del_address = 0;
																else
																		Passport_Flag.flag.del_address = 1;															
																Display_Usb();
																HID_DEL_ADDRESS_REP(0x0401,serialId,addressType);
														}
												}
												else//没有存储此地址
												{
														if(Set_Flag.flag.del_address)//是否重复进行删除地址时的密码验证，1为需要，0为不需要
																Passport_Flag.flag.del_address = 0;
														else
																Passport_Flag.flag.del_address = 1;		
														Display_Usb();
														HID_DEL_ADDRESS_REP(0x0206,serialId,addressType);
														break;
												}
										}
										else
										{
												if(Set_Flag.flag.del_address)//是否重复进行删除地址时的密码验证，1为需要，0为不需要
														Passport_Flag.flag.del_address = 0;
												else
														Passport_Flag.flag.del_address = 1;		
												Display_Usb();
												HID_DEL_ADDRESS_REP(ERR_UNKNOW_COIN,serialId,addressType);
												break;
										}
										Display_Usb();
										break;
								}
								case CMD_ADD_ADDRESS:					//0x0204
								{
										uint16_t addressType = data[4] | (data[5] << 8);
										if(Neo_System.count == 5) //地址已满
										{
												if(Set_Flag.flag.add_address)
														Passport_Flag.flag.add_address = 0;
												else
														Passport_Flag.flag.add_address = 1;
												Display_Usb();
												HID_ADD_ADDRESS_REP(ERR_KEY_MAX_COUNT,serialId,addressType,data+6);
												break;
										}
										if(addressType == ADDR_XiaoYi)
										{
												if(Get_Address_ID(data+6))//已经存在了这个地址
												{
														if(Set_Flag.flag.add_address)
																Passport_Flag.flag.add_address = 0;
														else
																Passport_Flag.flag.add_address = 1;
														Display_Usb();
														HID_ADD_ADDRESS_REP(ERR_EXIST_KEY,serialId,addressType,data+6);
														break;
												}
												uint8_t	PrivateKey[32];
												uint8_t PublicKey[65];
												uint8_t	PubKey[33];
												int tempLen = 0;
												char temp[40] = "";
												char addrCacl[40] = "";
												//得到传入的地址
												Alg_Base58Encode(data+6 , 25 ,temp,&tempLen);
												memmove(PrivateKey,dataSave,32);
												Alg_GetPublicFromPrivate(PrivateKey,PublicKey,1);
												memmove(PubKey,PublicKey,33);
												//得到数据包计算出的地址
												Alg_GetAddressFromPublic(PubKey,addrCacl,33);
#ifdef printf_debug
												printf("Address Prikey:%s\r\n",addrCacl);
#endif
												if(ArrayCompare((uint8_t*)temp,(uint8_t*)addrCacl,tempLen))
												{
														if(Display_AddAdd(addrCacl) == 0)//用户添加地址失败
														{
																Display_Usb();
																HID_ADD_ADDRESS_REP(0x0401,serialId,addressType,data+6);
																break;
														}
														if(Set_Flag.flag.add_address)
														{
																if(Display_VerifyCode() == 0)
																{
																		Passport_Flag.flag.add_address = 1;
																}
														}
														else
														{
																if(Passport_Flag.flag.add_address == 0)
																{
																		if(Display_VerifyCode() == 0)
																		{
																				Passport_Flag.flag.add_address = 1;
																		}
																}
														}
														if(Passport_Flag.flag.add_address)
														{
																uint8_t slotID = Get_Empty_SlotID();
																uint8_t slot_data[32];
#ifdef printf_debug
																printf("slotID: %d\r\n",slotID);
#endif
																if(Neo_System.count == 0)
																		EncryptDataNew(PrivateKey,32,slotID+2,Neo_System.pin);
																else
																		EncryptData(PrivateKey,32,slotID+2,Neo_System.pin);
																memset(slot_data,0,32);
																memmove(slot_data,data+6,25);
																ATSHA_write_data_slot(slotID+7,0,slot_data,32);
																Neo_System.count++;
																Updata_SYS_Count(Neo_System.count);			 //更新计数的值
																memmove(showaddress[slotID-1].content,data+6,25);
																memmove(showaddress[slotID-1].address,addrCacl,tempLen);
																
																if(Set_Flag.flag.add_address)//是否重复进行添加地址时的密码验证，1为需要，0为不需要
																		Passport_Flag.flag.add_address = 0;
																else
																		Passport_Flag.flag.add_address = 1;															
																Display_Usb();
																HID_ADD_ADDRESS_REP(1,serialId,addressType,data+6);
														}
														else
														{
																if(Set_Flag.flag.add_address)//是否重复进行添加地址时的密码验证，1为需要，0为不需要
																		Passport_Flag.flag.add_address = 0;
																else
																		Passport_Flag.flag.add_address = 1;																
																Display_Usb();
																HID_ADD_ADDRESS_REP(ERR_SAME_ADD_NAME,serialId,addressType,data+6);															
														}
												}
												else//地址匹配失败
												{
														if(Set_Flag.flag.add_address)//是否重复进行添加地址时的密码验证，1为需要，0为不需要
																Passport_Flag.flag.add_address = 0;
														else
																Passport_Flag.flag.add_address = 1;														
														Display_Usb();
														HID_ADD_ADDRESS_REP(ERR_KEY_FORMAT,serialId,addressType,data+6);
														break;
												}
										}
										else
										{
												if(Set_Flag.flag.add_address)//是否重复进行添加地址时的密码验证，1为需要，0为不需要
														Passport_Flag.flag.add_address = 0;
												else
														Passport_Flag.flag.add_address = 1;
												Display_Usb();
												HID_ADD_ADDRESS_REP(ERR_KEY_FORMAT,serialId,addressType,data+6);
												break;
										}
										Display_Usb();
										break;
								}
								case CMD_SIGN_DATA:						//0x0205
								{
										if(Display_VerifyCode_GetPin(Neo_System.pin) == 0)
										{
												Passport_Flag.flag.sign_data = 1;
										}									
										if(Passport_Flag.flag.sign_data)
										{
												uint8_t  AddressID 		= Get_Address_ID(data+6);
												if(AddressID)
												{
														uint8_t Privekey[32];
														STMFLASH_Erase_Sectors(FLASH_DATA_SECTOR);
														if(Get_Privekey(AddressID+7,Neo_System.pin,Privekey) != 0)//出错
														{
																Display_Usb();
																HID_SIGN_DATA_FAILED_REP(serialId);
																return;
														}
														//写入现场数据
														STMFLASH_WriteWord(FLASH_ADDRESS_SCENE,(uint32_t)serialId);
														STMFLASH_WriteWord(FLASH_ADDRESS_SCENE+4,(uint32_t)AddressID);
														STMFLASH_Write_ByteArray(FLASH_ADDRESS_SCENE+8,data+6,32);
														//写入数据包及相关数据
														STMFLASH_WriteWord(FLASH_ADDRESS_PACK,HidData.dataLen);
														STMFLASH_Write_ByteArray(FLASH_ADDRESS_PACK+4,Privekey,32);
														STMFLASH_Write_ByteArray(FLASH_ADDRESS_PACK+36,dataSave,((HidData.dataLen)/4)*4+4);
														//显示等待图标
														Fill_RAM(0x00);
														Show_Pattern(&gImage_wait[0],30,33,24,40);
														clearArea(120,40,16,1);
														jump_to_app(FLASH_ADDRESS_APP1);
												}
												else
												{
														Display_Usb();
														HID_SIGN_DATA_FAILED_REP(serialId);
														break;
												}
										}
										Display_Usb();
										break;
								}
								case CMD_SET_INFO:						//0x021a
								{
										Set_Flag.flag.auto_show 								= data[6] | (data[7] << 8);
										Set_Flag.flag.auto_update 							= data[8] | (data[9] << 8);
										Set_Flag.flag.add_address 							= data[10] | (data[11] << 8);
										Set_Flag.flag.del_address 							= data[12] | (data[13] << 8);
										Set_Flag.flag.backup_address 					= data[14] | (data[15] << 8);
										Set_Flag.flag.backup_address_encrypt 	= data[16] | (data[17] << 8);
										Updata_Set_Flag(&Set_Flag);
										HID_SET_INFO_REP(serialId);
										break;
								}
								case CMD_INSTALL_PACK:				//0x0301
								{
										uint16_t install_type = data[4] | (data[5] << 8);
										uint16_t install_cont = data[6] | (data[7] << 8);
										uint16_t version_coin = data[40] | (data[41] << 8);
										
										if(install_type != 0x0001)
												HID_INSTALL_PACK_REP(ERR_COIN_TYPE,serialId,install_type,install_cont,version_coin);
										if(coinrecord.count>=5)
												HID_INSTALL_PACK_REP(ERR_NO_SPACE_INSTALL,serialId,install_type,install_cont,version_coin);

										uint8_t hash_file[32];
										SHA256_Data(dataSave,HidData.dataLen,hash_file,32);
										if(ArrayCompare(data+8,hash_file,32))
										{
												uint32_t Flash_Sector_Address = Get_Empty_Sector();
												if(Flash_Sector_Address == FLASH_ADDRESS_APP1)
												{
														STMFLASH_Erase_Sectors(FLASH_SECTOR_APP1);
														coinrecord.coin1 = install_cont;
														coinrecord.version1 = version_coin;
												}
												else if(Flash_Sector_Address == FLASH_ADDRESS_APP2)
												{
														STMFLASH_Erase_Sectors(FLASH_SECTOR_APP2);
														coinrecord.coin2 = install_cont;
														coinrecord.version2 = version_coin;														
												}
												else if(Flash_Sector_Address == FLASH_ADDRESS_APP3)
												{
														STMFLASH_Erase_Sectors(FLASH_SECTOR_APP3);
														coinrecord.coin3 = install_cont;
														coinrecord.version3 = version_coin;														
												}
												else if(Flash_Sector_Address == FLASH_ADDRESS_APP4)
												{
														STMFLASH_Erase_Sectors(FLASH_SECTOR_APP4);
														coinrecord.coin4 = install_cont;
														coinrecord.version4 = version_coin;														
												}
												else if(Flash_Sector_Address == FLASH_ADDRESS_APP5)
												{
														STMFLASH_Erase_Sectors(FLASH_SECTOR_APP5);
														coinrecord.coin5 = install_cont;
														coinrecord.version5 = version_coin;													
												}
												else
												{
														HID_INSTALL_PACK_REP(ERR_NO_SPACE_INSTALL,serialId,install_type,install_cont,version_coin);
														break;
												}
												STMFLASH_Write_ByteArray(Flash_Sector_Address,dataSave,HidData.dataLen);
												STMFLASH_WriteHalfWord(Flash_Sector_Address+COIN_TYPE_OFFSET,install_cont);
												STMFLASH_WriteHalfWord(Flash_Sector_Address+COIN_VERSION_OFFSET,version_coin);
												PowerOn_PACKInfo_To_Ram();
												HID_INSTALL_PACK_REP(0,serialId,install_type,install_cont,version_coin);
										}
										else
										{
												HID_INSTALL_PACK_REP(ERR_DATA_HASH,serialId,install_type,install_cont,version_coin);	
										}								
										break;
								}
								case CMD_INSTALL_REQUEST:			//0x0302
								{
										if(Display_Updata_Wallet())
										{
												//更改更新标识
												uint8_t slot_data[32];
												ATSHA_read_data_slot(14,slot_data);
												slot_data[1] = 1;
												ATSHA_write_data_slot(14,0,slot_data,32);
												HID_INSTALL_REQUEST_REP(1,serialId);
												HAL_Delay(500);
												Deal_USB_ERROR();//重新配置下USB
												//重启回跳到BootLoader
												System_Reset();
										}
										else
										{
												HID_INSTALL_REQUEST_REP(0,serialId);
										}
										Display_Usb();
										break;
								}
								case CMD_UNINSTALL_PACK:			//0x0303
								{
										uint16_t install_type = data[4] | (data[5] << 8);
										if(install_type == coinrecord.coin1)
										{
												coinrecord.count--;
												coinrecord.coin1 = 0;
												coinrecord.version1 = 0;
												STMFLASH_Erase_Sectors(FLASH_SECTOR_APP1);
										}
										else if(install_type == coinrecord.coin2)
										{
												coinrecord.count--;
												coinrecord.coin2 = 0;
												coinrecord.version2 = 0;											
												STMFLASH_Erase_Sectors(FLASH_SECTOR_APP2);
										}
										else if(install_type == coinrecord.coin3)
										{
												coinrecord.count--;
												coinrecord.coin3 = 0;
												coinrecord.version3 = 0;											
												STMFLASH_Erase_Sectors(FLASH_SECTOR_APP3);
										}
										else if(install_type == coinrecord.coin4)
										{
												coinrecord.count--;
												coinrecord.coin4 = 0;
												coinrecord.version4 = 0;											
												STMFLASH_Erase_Sectors(FLASH_SECTOR_APP4);
										}
										else if(install_type == coinrecord.coin5)
										{
												coinrecord.count--;
												coinrecord.coin5 = 0;
												coinrecord.version5 = 0;											
												STMFLASH_Erase_Sectors(FLASH_SECTOR_APP5);
										}
										else
										{
												HID_UNINSTALL_PACK_REP(ERR_COIN_TYPE,serialId,install_type);
										}										
										HID_UNINSTALL_PACK_REP(0,serialId,install_type);
										break;
								}
								case CMD_SECU_PIPE:						//0x0401
								{
										break;
								}
								default:
								break;
						}
				}
				if(cmd == CMD_QUERY_PACK_INFO)								//0x0304
				{
						HID_QUERY_PACK_INFO_REP(serialId);
				}
		}
		else
		{
				HID_CRC_ERROR_REP(serialId);
#ifdef printf_debug				
				printf("crc error\r\n");
#endif			
		}
}


	
