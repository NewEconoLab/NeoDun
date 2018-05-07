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

static uint8_t dataSave[HID_MAX_DATA_LEN];
static uint8_t packIndexRecord[(HID_MAX_DATA_LEN+DATA_PACK_SIZE-1)/DATA_PACK_SIZE ];
static uint8_t resultsignRecord[98];

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
		data_rp[6] = 0;
		data_rp[7] = 0;
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

static void HID_QUERY_ADDRESS_ALL_REP(uint16_t serialID)//TBD
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		uint8_t slot_buff[32];
				
		for(uint8_t i = 0;i < 5;i++)
		{
				memset(slot_buff,0,32);
				ATSHA_read_data_slot(i + 8,slot_buff);
				if(slot_buff[0] == 0)
						continue;
				memset(data_rp,0,64);
				data_rp[0] = 0x02;
				data_rp[1] = 0xa0;
				data_rp[2] = serialID & 0xff;
				data_rp[3] = (serialID >> 8) & 0xff;
				data_rp[4] = i;
				data_rp[5] = 0;
				data_rp[6] = System_Flag.count;
				data_rp[7] = 0;
				data_rp[8] = (uint8_t)(ADDR_XiaoYi & 0xff);
				data_rp[9] = (uint8_t)((ADDR_XiaoYi >> 8) & 0xff);
				memmove(data_rp+10,slot_buff,32);
				
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
		data_rp[4] = System_Flag.count;
		
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);
}

static void HID_DEL_ADDRESS_REP(uint8_t state,uint16_t serialID,uint16_t addressType)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		if(state)
				data_rp[1] = 0xc1;
		else
				data_rp[1] = 0xc2;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		data_rp[4] = addressType & 0xff;
		data_rp[5] = (addressType >> 8) & 0xff;
		
		crc = Utils_crc32(0,data_rp,62);
		data_rp[62] = crc & 0xff;
		data_rp[63] = (crc >> 8) & 0xff;
		SendUSBData(data_rp,64);			
}

static void HID_ADD_ADDRESS_REP(uint8_t state,uint16_t serialID,uint16_t addressType,uint8_t *data)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		if(state)
				data_rp[1] = 0xb1;
		else
				data_rp[1] = 0xb2;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		data_rp[4] = addressType & 0xff;
		data_rp[5] = (addressType >> 8) & 0xff;
		memmove(data_rp+6,data,42);
		
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
		data_rp[1] = 0xe3;
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

static void HID_GET_INFO_REP(uint16_t serialID)
{
		uint32_t crc = 0;
		uint8_t data_rp[64];
		memset(data_rp,0,64);
		
		data_rp[0] = 0x02;
		data_rp[1] = 0xd1;
		data_rp[2] = serialID & 0xff;
		data_rp[3] = (serialID >> 8) & 0xff;
		
		data_rp[4] = System_Flag.new_wallet;
		data_rp[5] = 0;
		data_rp[6] = Set_Flag.flag.auto_show;
		data_rp[7] = 0;
		data_rp[8] = Set_Flag.flag.auto_update;
		data_rp[9] = 0;
		data_rp[10] = Set_Flag.flag.add_address;
		data_rp[11] = 0;
		data_rp[12] = Set_Flag.flag.del_address;
		data_rp[13] = 0;
		data_rp[14] = Set_Flag.flag.backup_address;
		data_rp[15] = 0;
		data_rp[16] = Set_Flag.flag.backup_address_encrypt;
		data_rp[17] = 0;
		
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
								case CMD_DEL_ADDRESS:					//0x0203
								{
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
												uint16_t addressType = data[4] | (data[5] << 8);
												if(Display_DelAdd() == 0)//超时或者拒绝删除
												{
														HID_DEL_ADDRESS_REP(0,serialId,addressType);
												}												
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
																//将该地址ID对应的私钥和地址槽，都写入0
																ATSHA_write_data_slot(addressID+2,0,data_write,32);
																ATSHA_write_data_slot(addressID+7,0,data_write,32);
																System_Flag.count--;
																Updata_SYS_Count(System_Flag.count);				//更新计数的值
															
																if(Set_Flag.flag.del_address)//是否重复进行删除地址时的密码验证，1为需要，0为不需要
																		Passport_Flag.flag.del_address = 0;
																else
																		Passport_Flag.flag.del_address = 1;		
																Display_MainPage();																
																HID_DEL_ADDRESS_REP(1,serialId,addressType);
																break;
														}
														else//没有存储此地址
														{
																if(Set_Flag.flag.del_address)//是否重复进行删除地址时的密码验证，1为需要，0为不需要
																		Passport_Flag.flag.del_address = 0;
																else
																		Passport_Flag.flag.del_address = 1;		
																Display_MainPage();
																HID_DEL_ADDRESS_REP(0,serialId,addressType);
																break;
														}
												}
												else
												{
														if(Set_Flag.flag.del_address)//是否重复进行删除地址时的密码验证，1为需要，0为不需要
																Passport_Flag.flag.del_address = 0;
														else
																Passport_Flag.flag.del_address = 1;		
														Display_MainPage();
														HID_DEL_ADDRESS_REP(0,serialId,addressType);
														break;
												}
										}
										Display_MainPage();
										break;
								}
								case CMD_ADD_ADDRESS:					//0x0204
								{
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
												Fill_RAM(0x00);
												Asc8_16(72,26,"Add Address ...");
												uint16_t addressType = data[4] | (data[5] << 8);
												if(System_Flag.count == 5) //地址已满
												{
														if(Set_Flag.flag.add_address)
																Passport_Flag.flag.add_address = 0;
														else
																Passport_Flag.flag.add_address = 1;
														Display_MainPage();
														HID_ADD_ADDRESS_REP(0,serialId,addressType,data+6);
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
																Display_MainPage();
																HID_ADD_ADDRESS_REP(0,serialId,addressType,data+6);
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
																		Display_MainPage();
																		HID_ADD_ADDRESS_REP(0,serialId,addressType,data+6);
																		break;
																}
																uint8_t slotID = Get_Empty_SlotID();
																uint8_t slot_data[32];
#ifdef printf_debug
																printf("slotID: %d\r\n",slotID);
#endif
																if(System_Flag.count == 0)
																		EncryptDataNew(PrivateKey,32,slotID+2);
																else
																		EncryptData(PrivateKey,32,slotID+2);
																memset(slot_data,0,32);
																memmove(slot_data,data+6,25);
																ATSHA_write_data_slot(slotID+7,0,slot_data,32);
																System_Flag.count++;
																Updata_SYS_Count(System_Flag.count);			 //更新计数的值
																if(Set_Flag.flag.add_address)//是否重复进行添加地址时的密码验证，1为需要，0为不需要
																		Passport_Flag.flag.add_address = 0;
																else
																		Passport_Flag.flag.add_address = 1;															
																Display_MainPage();
																HID_ADD_ADDRESS_REP(1,serialId,addressType,data+6);
																break;
														}
														else//地址匹配失败
														{
																if(Set_Flag.flag.add_address)//是否重复进行添加地址时的密码验证，1为需要，0为不需要
																		Passport_Flag.flag.add_address = 0;
																else
																		Passport_Flag.flag.add_address = 1;														
																Display_MainPage();
																HID_ADD_ADDRESS_REP(0,serialId,addressType,data+6);
																break;
														}
												}
												else
												{
														if(Set_Flag.flag.add_address)//是否重复进行添加地址时的密码验证，1为需要，0为不需要
																Passport_Flag.flag.add_address = 0;
														else
																Passport_Flag.flag.add_address = 1;
														Display_MainPage();
														HID_ADD_ADDRESS_REP(0,serialId,addressType,data+6);
														break;
												}
										}
										Display_MainPage();
										break;
								}
								case CMD_SIGN_DATA:						//0x020a
								{
										if(Display_VerifyCode() == 0)
										{
												Passport_Flag.flag.sign_data = 1;
										}									
										if(Passport_Flag.flag.sign_data)
										{
												Fill_RAM(0x00);
												Asc8_16(76,26,"Sign Data ...");
//												uint16_t addressType 	= data[4] | (data[5] << 8);
												uint8_t  AddressID 		= Get_Address_ID(data+6);
												if(AddressID)
												{
														uint8_t slot_data[32];
														uint8_t privateKey[32];
														SIGN_Out_Para Sign;												
														ATSHA_read_data_slot(AddressID+2,slot_data);
														DecryptData(slot_data,32,privateKey);
														memset(&Sign,0,sizeof(Sign));
														if(Alg_ShowSignData(dataSave,HidData.dataLen,&Sign))//数据包解析出错
														{
																Display_MainPage();
																HID_SIGN_DATA_FAILED_REP(serialId);
																break;
														}
														else
														{
																if(Sign.type != 0x80)//不是合约交易
																{
																		if(Display_Sign_Data_Type_Identify() == 0)//用户拒绝签名或者操作超时
																		{
																				Display_MainPage();
																				HID_SIGN_DATA_FAILED_REP(serialId);
																				break;
																		}
																		Fill_RAM(0x00);
																		Asc8_16(76,26,"Sign Data ...");
																}
																uint8_t index_add;
																uint8_t addessID = 0;
																char Sign_address[40] = "";
																int len_add = 0;
																
																if(Sign.countOutputs == 0)		 //输出端地址为0个
																{
																		Display_MainPage();
																		HID_SIGN_DATA_FAILED_REP(serialId);
																		break;														
																}
																else if(Sign.countOutputs == 1)//输出端地址为1个
																{
																		index_add = 0;
																}
																else if(Sign.countOutputs == 2)//输出端地址为2个
																{
																		if(ArrayCompare(data+6,Sign.address[0],25))
																				index_add = 0;
																		else if(ArrayCompare(data+6,Sign.address[1],25))
																				index_add = 1;
																		else											 //本机没有存有该地址
																		{
																				Display_MainPage();
																				HID_SIGN_DATA_FAILED_REP(serialId);
																				break;																
																		}
																}
																else//多个签名的情况
																{
																		index_add = 0xff;
																}
																
																Alg_Base58Encode(Sign.address[index_add] , 25 ,Sign_address,&len_add);
																if(Display_SignData(&Sign,Sign_address,addessID,index_add))
																{
																		uint8_t hash_all[32];
																		uint8_t hash_sign[32];
																		int len;
																		uint8_t resultsign[64];
																		uint8_t PublicKey[65];
																		uint8_t PubKey[33];
																		memset(hash_all,0,32);
																		memset(hash_sign,0,32);
																		memset(resultsign,0,64);
																		memset(resultsignRecord,0,98);
																		//签名的结果保存
																		Alg_ECDSASignData(dataSave,HidData.dataLen,resultsign,&len,privateKey);														
																		//组合成最终的数据，长度1字节+公钥33字节+签名结果64字节													
																		Alg_GetPublicFromPrivate(privateKey,PublicKey,1);
																		memmove(PubKey,PublicKey,33);
																		//需要确定上位机具体是要哪个数据，是长度为98的，还是长度为64的？？？？
																		resultsignRecord[0] = 33;
																		memmove(resultsignRecord+1,PubKey,33);
																		memmove(resultsignRecord+34,resultsign,64);
																		SHA256_Data(resultsignRecord, 98, hash_all, 32);										
//																	memmove(this->dataSave, resultsign,DATA_PACK_SIZE);
//																	memmove(this->dataSave + DATA_PACK_SIZE, resultsign + DATA_PACK_SIZE,64-DATA_PACK_SIZE);
																		memmove(dataSave, resultsignRecord,DATA_PACK_SIZE);
																		memmove(dataSave + DATA_PACK_SIZE, resultsignRecord + DATA_PACK_SIZE,98-DATA_PACK_SIZE);
																		SHA256_Data(resultsign, 64, hash_sign, 32);			

																		Display_MainPage();
																		
																		HidData.reqSerial = RandomInteger();
																		HID_SIGN_DATA_REP(CMD_NOTIFY_DATA,HidData.reqSerial,98,hash_all,32);
#ifdef HID_Delay
																		HAL_Delay(delay_hid);
#endif
																		HID_SIGN_DATA_REP(CMD_SIGN_OK,serialId,98,hash_all,32);
																		break;																			
																}
																else//决绝签名
																{
																		Display_MainPage();
																		HID_SIGN_DATA_FAILED_REP(serialId);
																		break;																		
																}
														}
												}
												else
												{
														Display_MainPage();
														HID_SIGN_DATA_FAILED_REP(serialId);
														break;
												}
										}
										Display_MainPage();
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
				if(cmd == CMD_GET_INFO)								//0x021b
				{
						HID_GET_INFO_REP(serialId);
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


	
