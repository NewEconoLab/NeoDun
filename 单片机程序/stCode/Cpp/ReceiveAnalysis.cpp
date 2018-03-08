/*
 * ReceiveAnalysis.cpp
 *
 *  Created on: 2017��7��30��
 *      Author: Administrator
 */

#include "ReceiveAnalysis.h"
#include "Utils.h"
#include "CMD.h"
#include "Commands.h"
#include "PrivateKey.h"
#include "PublicKey.h"
#include "Hal.h"
#include <string.h>
#include <stdio.h>
#include "DisplayMem.h"
#include "triangle.h"
#include "main_define.h"
#include "stmflash.h"

extern "C"
{
#include "Algorithm.h"
#include "encrypt.h"
}

#define 	delay_hid 	15		

//add by hkh
volatile int hid_flag = 0;
u8 hid_data[64];
int len_hid;
SIGN_KEY_FLAG Key_Flag;

extern int passport_num[9];
extern volatile int passport_flag_poweron;			
extern volatile int passport_flag_sign;		
extern volatile int passport_flag_set;
extern volatile int passport_flag_add;
extern volatile int passport_flag_del;
extern volatile int passport_flag_bek;
extern SIGN_SET_FLAG Set_Flag;

extern "C" void USB_DataReceiveHander(u8 * data, int len) //USB HID ���մ�����
{		
		len_hid = len;
		memmove(hid_data,data,len_hid);
		hid_flag = 1;
//		ReceiveAnalysis::GetInstance().PackDataFromPcCallback(data, len);
}

ReceiveAnalysis::ReceiveAnalysis() 
{
		this->state = CON_STATE_IDLE;
		this->packIndex = 0;
		this->packCount = 0;
		this->dataLen = 0;
		this->reqSerial = 0;
		clearData();
}

ReceiveAnalysis::ReceiveAnalysis(const ReceiveAnalysis& copy) 
{
		this->state = CON_STATE_IDLE;
		this->packIndex = 0;
		this->packCount = 0;
		this->dataLen = 0;
		this->reqSerial = 0;
		clearData();
}

ReceiveAnalysis& ReceiveAnalysis::GetInstance() 
{
		static ReceiveAnalysis ins;
		return ins;
}

bool ReceiveAnalysis::crc_frame(u8 data[], int len) 
{
		if (len != 64)
				return false;
		u32 res = Utils::Crc32(0, data, len - 2);		
		if ((data[len - 2] == (res & 0xff)) && (data[len - 1] == ((res >> 8) & 0xff))) 
		{
				return true;
		} 
		else 
		{
				return false;	//У��ʧ��
		}
}

void ReceiveAnalysis::PackDataFromPcCallback(u8 data[], int len) 
{
#ifdef printf_debug	
		printf("HID receive data:\r\n");
		Utils::PrintArray(data,len);
#endif	
		if (this->crc_frame(data, len)) 
		{
				int cmd = Utils::ReverseU16(Utils::ReadU16(data));
				int serialId = Utils::ReadU16(data + 2);
#ifdef printf_debug				
				printf("cmd = 0x%x\r\n",cmd);
				printf("serialId = 0x%x\r\n",serialId);
#endif		
				if(passport_flag_poweron)
				{
						switch (cmd) 
						{
								case CMD_NOTIFY_DATA://0x0101  ��ǩ�����������ݰ�
								{
										clearData();
										this->dataLen = Utils::ReadU32(data + 4);
										this->notifySerial = serialId;
#ifdef printf_debug								
										printf("ntf\r\n");								
										printf("dataLen = %d \r\n", dataLen);
#endif							
										if (dataLen < 64 * 1024) 
										{
												packCount = (dataLen + DATA_PACK_SIZE - 1) / DATA_PACK_SIZE;
												packIndex = 0;
												u8* hash = data + 8;
												memmove(this->hashRecord, hash, 32);
#ifdef printf_debug						
												printf("packIndex = %d \r\n", packIndex);	
												printf("packCount = %d \r\n", packCount);									
												//Utils::PrintArray(hash, 32);
#endif										
												Commands command;
												command.CreateDataQuest(CMD_PULL_DATA, this->reqSerial = Utils::RandomInteger(), 0, packCount - 1, hash, 32);
												command.SendToPc();
										} 
										else 
										{
												//��������̫�� �޷�����
												Commands command( CMD_ERR_DATA_TOO_BIG, serialId);
												command.SendToPc();
										}
										break;
								}
								case CMD_PULL_DATA_REQ://0x01a2 �յ�һ����Ƭ
								{
										u16 serial = Utils::ReadU16(data + 2);
										u16 Index = Utils::ReadU16(data + 4);										
#ifdef printf_debug						
										printf("Index = %d\r\n", Index);
#endif							
										if (serial == this->reqSerial) 
										{
												memmove(this->dataSave + packIndex * DATA_PACK_SIZE, data + 6,DATA_PACK_SIZE);
												packIndexRecord[packIndex] = true;
												packIndex++;
										} 
										else 
										{
#ifdef printf_debug									
												printf("not this serial");
#endif									
										}
#ifdef printf_debug						
										printf("packIndex = %d\r\n", packIndex);
#endif											
										break;
								}
								case CMD_PULL_DATA://0x0110  ��λ����������
								{													
										int blockbegin = Utils::ReadU16(data + 4);
										int blockend = Utils::ReadU16(data + 6);								
										
										for(int i=blockbegin;i<(blockend+1);i++)
										{
												u8 pd[2] = {0,0};
												pd[0] = (u8)(i&0xff); 
												pd[1] = (u8)((i>>8)&0xff);
												
												Commands command( CMD_PULL_DATA_REQ, serialId);
												command.AppendBytes(&pd[0],1);
												command.AppendBytes(&pd[1],1);												
												command.AppendBytes(this->dataSave+i*DATA_PACK_SIZE,50);
												command.SendToPc();
#ifdef HID_Delay													
												HAL_Delay(delay_hid);
#endif												
										}
										
										Commands command( CMD_PULL_DATA_REQ_OVER, serialId);
										command.SendToPc();
										break;
								}
								case CMD_PULL_DATA_REQ_OVER://0x01a3  �������
								{
#ifdef printf_debug							
										printf("Receive data over\r\n");
#endif							
										if (isAllPackGot()) 
										{
												u8 res[32];
												Utils::Sha256(dataSave, this->dataLen , res, 32);
#ifdef printf_debug										
												printf("All pack got \r\n");
												Utils::PrintArray(res, 32);
#endif
												if (hashCompare(res, hashRecord))//hash ��ȷ
												{

#ifdef printf_debug											
														printf("hash ok \r\n");
#endif											
														Commands command( CMD_ACK_HASH_OK, this->notifySerial);												
														command.AppendU32(packIndex);//������λ�������ݷ���ID����Ӳ��Ǯ�����˼�Ƭ����			
														command.AppendBytes(res, 32);
														command.SendToPc();
												} 
												else//hash ����
												{
#ifdef printf_debug												
														printf("hash error \r\n");
#endif											
														Commands command( CMD_ERR_HASH, this->notifySerial);
														command.AppendU32(packIndex);//������λ�������ݷ���ID		
														command.AppendBytes(res, 32);
														command.SendToPc();
												}
										} 
										else 
										{
												for (int i = 0; i < packCount; i++) 
												{
														if (packIndexRecord[i] == false)//����һ��������
														{
#ifdef printf_debug														
																printf("repull %d\r\n", i);
#endif													
																Commands command;
																u8 hash[32];
																command.CreateDataQuest(CMD_PULL_DATA, this->reqSerial, i, i,hash, 32);
																command.SendToPc();
														}
												}
										}
										break;
								}
								case CMD_QUERY_ADDRESS ://0x0201  ��ѯ��ַ
								{
										int i=0;
										u32 count = Get_Count_Num();
										u32 address_flash = 0x08060000;
										int serialId_add = Utils::ReadU16(data + 2);
		#ifdef printf_debug							
										printf("count=0x%x\r\n",count);
		#endif							
										for(i = 0 ; i < count ; i++ )
										{
												char buff [40] = "";
												u8 result[52];
												int lenout = 0;
												memset(result,0,52);
												while(STMFLASH_ReadWord(address_flash) == 0)//Ϊ0��ʾд���ĵ�ַ�����ұ�������
												{
														address_flash += 80;
														if(address_flash > 0x08100000)
																break;
												}
		#ifdef printf_debug													
												printf("address_flash = 0x%x\r\n",address_flash);
		#endif													
												STMFLASH_Read(address_flash,(u32 *)buff,10);												
												int len  = strlen(buff);												
												Alg_Base58Decode(buff,len,result,&lenout);
		#ifdef printf_debug									
												printf("lenout:%d\r\n",lenout);
												Utils::PrintArray(result,lenout);
												printf("i:%d\r\n",i);												
		#endif									
												Commands command( CMD_RETURN_ADDRESS, serialId_add);
												command.AppendU16(i);
												command.AppendU16((int)count);
												command.AppendU16(ADDR_XiaoYi );										
												command.AppendBytes((u8*)result,lenout);
												command.SendToPc();
		#ifdef printf_debug									
												printf("count = 0x%x \r\n", count);
		#endif										
		#ifdef HID_Delay													
												HAL_Delay(delay_hid);
		#endif									
												address_flash += 80;
										}										
										Commands command( CMD_RETURN_ADDRESS_OK, serialId_add);
										command.AppendU16((int)count);
										command.SendToPc();
										break;
								}								
								case CMD_DEL_ADDRESS ://0x0203  ɾ����ַ
								{
										if(passport_flag_del)
										{
												u16 addressType = Utils::ReadU16(data + 4);
#ifdef printf_debug
												printf("delete address\r\n");
												printf("addressType = 0x%04x\r\n",addressType);
#endif
												if(addressType == ADDR_XiaoYi)
												{
														u8 * content = data+6;
														char temp[40] = "";
														int tempLen = 0;
														//Utils::PrintArray(content,25);
														Alg_Base58Encode(content , 25 ,temp,&tempLen);									
														
														//ɾ����Ӧ�ĵ�ַ��˽Կ
														u32 count = Get_Count_Num();
														u32 address = Get_Flash_Address(temp,count);
#ifdef printf_debug									
														printf("count = %d\r\n",count);
														printf("address = 0x%x\r\n",address);
#endif
														if(address)
														{
																for(int i =0;i<20;i++)		//���õ�ַ������80�ֽ�д��0
																		STMFLASH_WriteWord(address+4*i,0);
																Update_Count_Num(count-1);//���¼�����ֵ													
																Commands command( CMD_DEL_ADDRESS_OK, serialId);
																command.AppendU16(addressType);
																command.SendToPc();
														}
														else//û�д洢�˵�ַ
														{
																Commands command( CMD_DEL_ADDRESS_FAILED, serialId);
																command.AppendU16(addressType);
																command.SendToPc();										
														}
												}
												else//��ַ���Ͳ���С��
												{
														Commands command( CMD_DEL_ADDRESS_FAILED, serialId);
														command.AppendU16(addressType);
														command.SendToPc();
												}
												if(Set_Flag.Del_Address_Flag)
														passport_flag_del = 0;
												else
														passport_flag_del = 1;
												break;
										}
								}
								case CMD_ADD_ADDRESS ://0x0204  ����һ����ַ
								{
#ifdef printf_debug									
										printf("passport_flag_add:%d\r\n",passport_flag_add);
#endif
										if(passport_flag_add)
										{
												u16 addressType = Utils::ReadU16(data + 4);
#ifdef printf_debug								
												printf("add address\r\n");
												printf("addressType = 0x%04x\r\n",addressType);	
#endif							
												if(addressType == ADDR_XiaoYi) //neo�ĵ�ַ����
												{
														u8 * content = data+6;
														char temp[40] = "";
														int tempLen = 0;

														Alg_Base58Encode(content , 25 ,temp,&tempLen);																														
														u32 dataId0 = Utils::ReadU32(data +46);				
#ifdef printf_debug									
														printf("Base58 content address:%s\r\n",temp);
														printf("dataId0=%d\r\n",dataId0);	
#endif									
														PrivateKey prkey(this->dataSave);//˽Կ�����Ե�һ��(0x01a2)�����ݣ����ʣ���Զ�ǵ�һ��������ȥ��dataId0	 																
														PublicKey pubK = prkey.GetPublicKey(true);
														uint8_t pubKEY[33];
														memset(pubKEY,0,33);
														memmove(pubKEY,pubK.getData(),33);								
														char addrCacl[40] = "";
														Alg_GetAddressFromPublic(pubKEY,addrCacl,33);
#ifdef printf_debug									
														printf("Get address from Prikey:%s\r\n",addrCacl);																												
#endif										
													
														int m=0;
														int address_flag_error = 0;
														u32 count_temp = Get_Count_Num();
														u32 address_flash_temp = 0x08060000;													
														char buff_temp [40] = "";
														u8 result_temp[52];													
													
														for(m = 0 ; m < count_temp ; m++ )
														{
																memset(buff_temp,0,40);
																memset(result_temp,0,52);
																while(STMFLASH_ReadWord(address_flash_temp) == 0)//Ϊ0��ʾд���ĵ�ַ�����ұ�������
																{
																		address_flash_temp += 80;
																		if(address_flash_temp > 0x08100000)
																				break;
																}
#ifdef printf_debug													
																printf("address_flash = 0x%x\r\n",address_flash_temp);
#endif													
																STMFLASH_Read(address_flash_temp,(u32 *)buff_temp,10);		
																
																if(Utils::MemoryCompare(temp,buff_temp,tempLen))
																{
																		address_flag_error = 1;
																}
														}	
														if(address_flag_error)		
														{
																Commands command( CMD_ADDR_ADD_FAILED, serialId);
																command.AppendU16(addressType);
																command.AppendBytes(data+6,40+2);
																command.SendToPc();
																if(Set_Flag.Add_Address_Flag)
																		passport_flag_add = 0;
																else
																		passport_flag_add = 1;
																break;															
														}
														
														bool cmp = Utils::MemoryCompare(temp,addrCacl,tempLen);																			
														if(cmp) //��ַƥ��ɹ�
														{
#ifdef printf_debug											
																printf("cmp address sucess\r\n");
#endif											
																u32 dataId1 = Utils::ReadU32(data +50);
																u32 dataId2 = Utils::ReadU32(data +54);
																u32 dataId3 = Utils::ReadU32(data +58);																								
																//�ַ������͵ĵ�ַ��˽Կ����ַ�����¼���������浽������
//														Address addr(temp,prkey.getData(),addressType,dataId0,dataId1,dataId2,dataId3 );
//														this->addreses.add(addr);
																//printf("addr = %s \r\n", content );
															
																u32 address_flash = Get_Flash_EMPTY();
#ifdef printf_debug													
																printf("address_flash = 0x%x\r\n",address_flash);
#endif													
																if(address_flash)
																{
																		STMFLASH_Write(address_flash,(u32 *)addrCacl,10);
																		STMFLASH_Write(address_flash+40,(u32 *)prkey.getData(),8);
																		u32 count = Get_Count_Num();
#ifdef printf_debug
																		printf("count = 0x%x\r\n",count);
#endif															
																		Update_Count_Num(count+1);//���¼�����ֵ
																		Commands command( CMD_ADDR_ADD_OK, serialId);
																		command.AppendU16(addressType);
																		command.AppendBytes(data+6,ADDR_SIZE);
																		command.SendToPc();
																}
																else//�洢��ַ���˵����
																{
																		Commands command( CMD_ADDR_ADD_FAILED, serialId);
																		command.AppendU16(addressType);
																		command.AppendBytes(data+6,40+2);
																		command.SendToPc();																				
																}
														}
														else		 //��ַƥ��ʧ��
														{
																Commands command( CMD_ADDR_ADD_FAILED, serialId);
																command.AppendU16(addressType);
																command.AppendBytes(data+6,40+2);
																command.SendToPc();						
														}
												}
												else  //����neo�ĵ�ַ���ֱ࣬��ʧ��
												{										
														Commands command( CMD_ADDR_ADD_FAILED, serialId);
														command.AppendU16(addressType);
														command.AppendBytes(data+6,40+2);
														command.SendToPc();
												}
												if(Set_Flag.Add_Address_Flag)
														passport_flag_add = 0;
												else
														passport_flag_add = 1;
												break;
										}
								}
								case CMD_GET_PRIKEY://0x0206  ��ȡ��ַ��ȫ��Ϣ
								{		
										if(passport_flag_bek)
										{
												int serialId_getprikey = Utils::ReadU16(data + 2);
												u16 addressType = Utils::ReadU16(data+8);
#ifdef printf_debug							
												printf("Get security message\r\n");
												printf("addressType = 0x%04x\r\n",addressType);
#endif									
/*********************************************************************************************************************
		��ȡ��ȫ��Ϣʱ����ʾҳ�棺
															�Ƿ�ͬ���ȡ��ȫ��Ϣ��												
												
												ȡ��          ȷ��          ȡ��		
*********************************************************************************************************************/										
												view::DisplayMem::getInstance().clearAll();//����
												//��ʾ����		�Ƿ�ͬ���ȡ��ȫ��Ϣ��										
												view::DisplayMem::getInstance().drawHZString(62,18,11,21);
												view::DisplayMem::getInstance().drawHZString(36,48,9,10);
												view::DisplayMem::getInstance().drawHZString(124,48,4,5);	
												view::DisplayMem::getInstance().drawHZString(208,48,9,10);		
												
												view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],5,7,48,60);//��������
												view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],27,29,48,60);//��������
												view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],48,50,48,60);//��������	
												view::DisplayMem::getInstance().clearArea(0,60,256,1);//������һ�еİ׵�																		
												
												Key_Flag.Sign_Key_Flag = 1;//������Ч
												while(1)
												{
														if(Key_Flag.Sign_Key_center_Flag)//ͬ��
														{
																Key_Flag.Sign_Key_Flag = 0;
																u8 *content = data+10;
																char temp[40] = "";
																int tempLen = 0;
																u8 hash[33];
																int i=0;
																u8 hash1[33]={32};
#ifdef printf_debug									
																//Utils::PrintArray(content,25);
#endif										
																Alg_Base58Encode(content , 25 ,temp,&tempLen );
																u32 count = Get_Count_Num();
																u32 address_flash = Get_Flash_Address(temp,count);
																u8 privateKey[32];
																memset(privateKey,0,32);
																STMFLASH_Read(address_flash+40,(u32 *)privateKey,8);
																for(i=1;i<33;i++)
																		hash1[i] = privateKey[i-1];
																Utils::Sha256(hash1, 33, hash, 32);														
																memmove(this->dataSave, hash1,33);														
																this->reqSerial = Utils::RandomInteger();
																Commands::getInstance().SendHidFrame(CMD_NOTIFY_DATA,this->reqSerial,33,hash,32);																	
#ifdef HID_Delay															
																HAL_Delay(delay_hid);
#endif	
																Commands::getInstance().SendHidFrame(CMD_RETURN_MESSAGE,serialId_getprikey,32,hash,32);
																break;
														}
														else if((Key_Flag.Sign_Key_left_Flag)||(Key_Flag.Sign_Key_right_Flag))//�ܾ�
														{
																Key_Flag.Sign_Key_Flag = 0;
																Commands command( CMD_GET_PRIKEY_FAILED, serialId);
																command.SendToPc();
																break;								
														}
												}
												Key_Flag.Sign_Key_Flag = 0;
												memset(&Key_Flag,0,sizeof(Key_Flag));
												view::DisplayMem::getInstance().clearAll();//����
												view::DisplayMem::getInstance().drawString(92,20,"NeoDun",view::FONT_12X24);	
												if(Set_Flag.Backup_Address_Flag)
														passport_flag_bek = 0;
												else
														passport_flag_bek = 1;
												break;
										}
								}
								case CMD_SIGN_DATA ://0x020a  ǩ��
								{
										if(passport_flag_sign)
										{
												passport_flag_sign = 0;
												int i = 0;
												char temp[40] = "";									
												u32 count;
												u32 address_flash;
												int serialId_sign_data = Utils::ReadU16(data + 2);
												u16 addressType = Utils::ReadU16(data + 4);							  							
												u32 dataId0 = Utils::ReadU32(data +46);
												u8 privateKey[32];
#ifdef printf_debug								
												printf("sign data\r\n");
												printf("addressType = 0x%04x\r\n",addressType);	
												printf("dataId0 = %d\r\n",dataId0);
#endif							
												if(addressType == ADDR_XiaoYi)
												{									
														u8 * content = data+6;
														int tempLen = 0;
#ifdef printf_debug									
														//Utils::PrintArray(content,25);
#endif										
														Alg_Base58Encode(content , 25 ,temp,&tempLen );
														count = Get_Count_Num();
														address_flash = Get_Flash_Address(temp,count);						
												}
												memset(privateKey,0,32);
												STMFLASH_Read(address_flash+40,(u32 *)privateKey,8);
#ifdef printf_debug									
		//										printf("privateKey:\r\n");
		//										Utils::PrintArray(privateKey,32);
#endif																													
												SIGN_Out_Para Sign;
												memset(&Sign,0,sizeof(Sign));
												int result = Alg_ShowSignData(this->dataSave,this->dataLen,&Sign);
												if(result == 1)
												{
														Commands command( CMD_SIGN_FAILED, serialId);
														command.SendToPc();
														break;										
												}
												if(Sign.type == 0x80)
												{
												}
												else
												{
														view::DisplayMem::getInstance().clearAll();//����
														view::DisplayMem::getInstance().drawString(92,20,"Undefined Transaction,continue?",view::FONT_6X12);
														//ȡ��  ȷ��  ȡ��
														view::DisplayMem::getInstance().drawHZString(36,48,9,10);
														view::DisplayMem::getInstance().drawHZString(124,48,4,5);	
														view::DisplayMem::getInstance().drawHZString(208,48,9,10);		
														//��ʾ������
														view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],5,7,48,60);//��������
														view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],27,29,48,60);//��������
														view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],48,50,48,60);//��������	
														view::DisplayMem::getInstance().clearArea(0,60,256,1);//������һ�еİ׵�
													
														Key_Flag.Sign_Key_Flag = 1;//������Ч
														memset(&Key_Flag,0,sizeof(Key_Flag));
														while(1)
														{
																if(Key_Flag.Sign_Key_center_Flag)//ȷ��
																{
																		break;
																}
																else if(Key_Flag.Sign_Key_left_Flag&&Key_Flag.Sign_Key_right_Flag)//ȡ��
																{
																		Commands command( CMD_SIGN_FAILED, serialId);
																		command.SendToPc();
																		break;
																}
														}
														Key_Flag.Sign_Key_Flag = 0;//������Ч
														memset(&Key_Flag,0,sizeof(Key_Flag));
												}
												
												int status = 1;
												for(i=0;i<Sign.countOutputs;i++)
												{
														if(Utils::MemoryCompare(Sign.address[i],temp,strlen(Sign.address[i])) == 0)//�Ե�ַ���бȽ�,��һ��
														{
																status = 0;
																break;
														}						
												}
												
												if((status)||(Sign.coin == 0xff))//�����������ַ
												{
														Commands command( CMD_SIGN_FAILED, serialId);
														command.SendToPc();
														view::DisplayMem::getInstance().clearAll();//����
														view::DisplayMem::getInstance().drawString(92,20,"NeoDun",view::FONT_12X24);
														break;
												}										
												
												//printf("����һ�ʽ�����Ҫ��ǩ�����Ƿ�ͬ�⣿\r\n");
												//view::DisplayMem::getInstance().drawHZString(0,32,0,15);
																						
												for(i = 0;i < Sign.countOutputs;i++)
												{																				
														if(Utils::MemoryCompare(Sign.address[i],temp,strlen(Sign.address[i])))//�Ե�ַ���бȽ�
														{
																continue;
														}
		/*********************************************************************************************************************
		����ת��ʱ����ʾҳ�棺
												ת�� xxx NEO/GAS ��
														
												xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
												
												ȡ��          ȷ��          ȡ��		
		*********************************************************************************************************************/												
														view::DisplayMem::getInstance().clearAll();//�����ʾ
														//ת��
														view::DisplayMem::getInstance().drawHZString(0,0,22,23);
														
														//��ʾ��Ŀ   ��ֵΪһ��long long��  ��Ӧ��ʮ�������ĺ��λ��ΪС��
														int count_bit=0;
														int count_int = 0;//��ʾ��������ռ�õ���ʾλ��
														int count_dec = 8;//��ʾС�����ֺ�׺����ĸ���
														
														count_int = view::DisplayMem::getInstance().drawNumber(28,0,Sign.money[i]/100000000,8,view::FONT_8X16);
														if(Sign.money[i]%100000000)//����ֵ����Ϊ100000000����ʾBUG
														{
																view::DisplayMem::getInstance().drawString(28+count_int*8,0,".");												
																count_dec = view::DisplayMem::getInstance().drawxNumber(28+(count_int+1)*8,0,Sign.money[i]%100000000,8,view::FONT_8X16) - 1;//-1�ǰ�С�������ȥ
														}
														if(Sign.money[i] == 0)//ֵΪ0�����
																count_dec = 8;
														count_bit = 28 + (count_int + 8 -count_dec)*8 + 4; //28��ǰ��ռ�õ���ʾ��+4����ʾ��϶������
														
														if(Sign.coin == 1)												
																view::DisplayMem::getInstance().drawString(count_bit,0,"NEO");												
														else if(Sign.coin == 0)
																view::DisplayMem::getInstance().drawString(count_bit,0,"GAS");
														
														count_bit = count_bit + 28;//3*8 + 4											
#ifdef printf_debug															
														printf("address:%s\r\n",Sign.address[i]);
#endif														
														view::DisplayMem::getInstance().drawHZString(count_bit,0,24,24);//��
														view::DisplayMem::getInstance().drawString(0,16,Sign.address[i],view::FONT_6X12);//��ʾ��ַ											
														//ȡ��  ȷ��  ȡ��
														view::DisplayMem::getInstance().drawHZString(36,48,9,10);
														view::DisplayMem::getInstance().drawHZString(124,48,4,5);	
														view::DisplayMem::getInstance().drawHZString(208,48,9,10);		
														//��ʾ������
														view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],5,7,48,60);//��������
														view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],27,29,48,60);//��������
														view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],48,50,48,60);//��������	
														view::DisplayMem::getInstance().clearArea(0,60,256,1);//������һ�еİ׵�	
												}									
												
												Key_Flag.Sign_Key_Flag = 1;//������Ч
												memset(&Key_Flag,0,sizeof(Key_Flag));
												while(1)
												{		
														if(Key_Flag.Sign_Key_center_Flag)//ǩ������������ͬ�ⰴť
														{		
																Key_Flag.Sign_Key_Flag = 0;
																u8 hash_all[32];
																u8 hash_sign[32];
																int len;
																u8 resultsign[64];
																
																memset(hash_all,0,32);
																memset(hash_sign,0,32);
																memset(resultsign,0,64);
																memset(resultsignRecord,0,98);
																//ǩ���Ľ������
																Alg_ECDSASignData(this->dataSave,this->dataLen,resultsign,&len,privateKey);														
																//��ϳ����յ����ݣ�����1�ֽ�+��Կ33�ֽ�+ǩ�����64�ֽ�
																PrivateKey prkey(privateKey);													
#ifdef printf_debug									
																printf("privateKey:\r\n");
																Utils::PrintArray(prkey.getData(),32);
#endif															
																PublicKey pubK = prkey.GetPublicKey(true);
																uint8_t pubKEY[33]; 
																memmove(pubKEY,pubK.getData(),33);
																//��Ҫȷ����λ��������Ҫ�ĸ����ݣ��ǳ���Ϊ98�ģ����ǳ���Ϊ64�ģ�������
																resultsignRecord[0] = 33;
																memmove(resultsignRecord+1,pubKEY,33);
																memmove(resultsignRecord+34,resultsign,64);

																Utils::Sha256(resultsignRecord, 98, hash_all, 32);	
																clearData();														
		//														memmove(this->dataSave, resultsign,DATA_PACK_SIZE);
		//														memmove(this->dataSave + DATA_PACK_SIZE, resultsign + DATA_PACK_SIZE,64-DATA_PACK_SIZE);
																memmove(this->dataSave, resultsignRecord,DATA_PACK_SIZE);
																memmove(this->dataSave + DATA_PACK_SIZE, resultsignRecord + DATA_PACK_SIZE,98-DATA_PACK_SIZE);
																
																Utils::Sha256(resultsign, 64, hash_sign, 32);														
#ifdef printf_debug														
																printf("hash\r\n");
																Utils::PrintArray(hash_all,32);																												
																Utils::PrintArray(hash_sign,32);
																printf("*********************************\r\n");
																Utils::PrintArray(resultsignRecord,98);
#endif															
																this->reqSerial = Utils::RandomInteger();
																Commands::getInstance().SendHidFrame(CMD_NOTIFY_DATA,this->reqSerial,98,hash_all,32);
#ifdef HID_Delay														
																HAL_Delay(delay_hid);
#endif														
																Commands::getInstance().SendHidFrame(CMD_SIGN_OK,serialId_sign_data,98,hash_all,32);																												
																view::DisplayMem::getInstance().clearAll();//����
																view::DisplayMem::getInstance().drawString(92,20,"NeoDun",view::FONT_12X24);
																break;												
														}
														else if((Key_Flag.Sign_Key_left_Flag)||(Key_Flag.Sign_Key_right_Flag))//���¾ܾ�ǩ����ť
														{
																Key_Flag.Sign_Key_Flag = 0;
																Commands command( CMD_SIGN_FAILED, serialId);
																command.SendToPc();
																view::DisplayMem::getInstance().clearAll();//����
																view::DisplayMem::getInstance().drawString(92,20,"NeoDun",view::FONT_12X24);
																break;
														}										
												}
												Key_Flag.Sign_Key_Flag = 0;//������Ч
												memset(&Key_Flag,0,sizeof(Key_Flag));
												break;
										}
										break;
								}
								case CMD_SET_INFO:  //0x021a
								{								
										//Set_Flag.New_Device_Flag = Utils::ReadU16(data + 4);
										Set_Flag.Auto_Show_Flag = Utils::ReadU16(data + 6); 
										Set_Flag.Auto_Update_Flag = Utils::ReadU16(data + 8);
										Set_Flag.Add_Address_Flag = Utils::ReadU16(data + 10);
										Set_Flag.Del_Address_Flag = Utils::ReadU16(data + 12);
										Set_Flag.Backup_Address_Flag = Utils::ReadU16(data + 14);
										Set_Flag.Backup_Address_Encrypt_Flag = Utils::ReadU16(data + 16);
										Updata_Set_Flag(&Set_Flag);
										if(1)
										{										
												Commands command( CMD_SET_INFO_OK, serialId);
												command.SendToPc();										
										}
										else
										{
												Commands command( CMD_SET_INFO_FAILED, serialId);
												command.SendToPc();												
										}
										break;
								}							
								default:			
								break;
						}
				}
				
				//û��֤����ʱ��ֻ�������¼���
				switch (cmd)
				{
						case CMD_GET_INFO:  //0x021b
						{															
								if(1)
								{																			
										Commands command( CMD_GET_INFO_OK, serialId);
										command.AppendU16(Set_Flag.New_Device_Flag);
										command.AppendU16(Set_Flag.Auto_Show_Flag);
										command.AppendU16(Set_Flag.Auto_Update_Flag);
										command.AppendU16(Set_Flag.Add_Address_Flag);
										command.AppendU16(Set_Flag.Del_Address_Flag);
										command.AppendU16(Set_Flag.Backup_Address_Flag);
										command.AppendU16(Set_Flag.Backup_Address_Encrypt_Flag);
										command.SendToPc();										
								}
								else
								{
										Commands command( CMD_GET_INFO_FAILED, serialId);
										command.SendToPc();										
								}
								break;
						}						
						case CMD_SET_PASSPORT://��������  0x020b
						{
								if(Set_Flag.New_Device_Flag)
										passport_flag_set = 1;
								if(passport_flag_set)
								{
										passport_flag_set = 0;
										int i = 0;
										char passport_new[6] = "";
										u32 passport_array[6] = {0,0,0,0,0,0};
										int len = Utils::ReadU16(data+4);								
										u8* passport = data+8;
										
										if((Utils::ReadU16(data+6))&&(len != 0))
										{
												//memset(passport_new,0,6);
												Utils::HexToNum(passport,len,passport_new);
#ifdef printf_debug							
												printf("passport_new: %s\r\n",passport_new);
#endif							
												for(i=0;i<6;i++)
												{
														passport_array[i] = passport_num[passport_new[i] - 0x31];
#ifdef printf_debug													
														printf("%d ",passport_array[i]);
#endif													
												}	
				//								view::DisplayMem::getInstance().GetPassportFromString((u8*)passport_new,passport_num,passport_array);								
												Update_Passport(passport_array);//����FLASH�������ֵ
												
												
												if(Set_Flag.New_Device_Flag)//�������������룬���豸���������豸
												{
														Set_Flag.New_Device_Flag = 0;
														Updata_Set_Flag(&Set_Flag);
												}
												
												Commands command( CMD_SET_OK, serialId);
												command.SendToPc();											
												break;
										}
										else
										{
												Commands command( CMD_SET_FAILED, serialId);
												command.SendToPc();											
												break;								
										}
								}
								break;
						}						
						case CMD_VERIFY_PASSPORT://��֤����  0x020c
						{
								int i = 0;
								char passport_new[6] = "";
								u16 function_code = Utils::ReverseU16(Utils::ReadU16(data+6));
#ifdef printf_debug							
								printf("Verify function_code: 0x%x\r\n",function_code);							
#endif							
								int len = Utils::ReadU16(data+4);
								u8* passport = data+8;
								u32 passport_array[6] = {0,0,0,0,0,0};
								u32 passport_old[6] = {0,0,0,0,0,0}; 
								
								if((len == 0)||(Utils::ReadU16(data+6) == 0))//����
								{
										Commands command( CMD_VERIFY_FAILED, serialId);//��֪��λ��������֤δͨ��
										command.SendToPc();
										passport_flag_poweron = 0;
										view::DisplayMem::getInstance().drawString(34,24,"Error");
										break;
								}
								
								Utils::HexToNum(passport,len,passport_new);
#ifdef printf_debug							
								printf("passport_new: %s\r\n",passport_new);							
#endif							
								for(i=0;i<6;i++)
								{										
										passport_array[i] = passport_num[passport_new[i] - 0x31];
								}														
								Get_Passport(passport_old);
								if(Utils::PassportMemoryCompare(passport_old,passport_array,6))
								{
										Commands command( CMD_VERIFY_OK, serialId);//��֪��λ��������֤ͨ��
										command.AppendU16(function_code);
										command.SendToPc();
									
										switch(function_code)
										{									
												case CMD_ADD_ADDRESS:			//���ӵ�ַ
														passport_flag_add = 1;
														break;
												case CMD_DEL_ADDRESS:			//ɾ����ַ
														passport_flag_del = 1;
														break;
												case CMD_GET_PRIKEY:			//��ȡ��ȫ��Ϣ
														passport_flag_bek = 1;
														break;												
												case CMD_SIGN_DATA:				//����ǩ��
														passport_flag_sign = 1;
														break;
												case CMD_SET_PASSPORT:		//��������
														passport_flag_set = 1;
														break;
												default:
														break;
										}																		
										passport_flag_poweron = 1;//��������֤��־λ																																				
										view::DisplayMem::getInstance().clearAll();//����
										view::DisplayMem::getInstance().drawString(92,20,"NeoDun",view::FONT_12X24);										
								}
								else
								{
										Commands command( CMD_VERIFY_FAILED, serialId);//��֪��λ��������֤δͨ��
										command.SendToPc();
										passport_flag_poweron = 0;
										view::DisplayMem::getInstance().drawString(34,24,"Error");
								}
								break;
						}
						case CMD_SHOW_PASSPORT: //0x021c
						{							
								int value = 0;
								view::DisplayMem::getInstance().clearAll();
								view::DisplayMem::getInstance().drawString(0,0,"PassPort:");
								value = view::DisplayMem::getInstance().GetPassportArray(passport_num); //������ʾ���9��������
#ifdef printf_debug
								printf("******  Passport \r\n");
								for(int m = 0;m<9;m++)
										printf("%d ",passport_num[m]);
								printf("\r\n");
#endif														
								if(value)
								{
										Commands command( CMD_SHOW_OK, serialId);
										command.SendToPc();	
								}
								else
								{
										Commands command( CMD_SHOW_FAILED, serialId);
										command.SendToPc();								
								}
						}
						default:
#ifdef printf_debug							
								printf("not handle CMD = %x\r\n",cmd);
#endif				
						break;						
				}
		} 
		else 
		{
				int serialId = Utils::ReadU16(data + 2);
				Commands command( CMD_CRC_FAILED, serialId);
				command.SendToPc();			
				printf("crc error\r\n");
		}
}

void ReceiveAnalysis::clearData() 
{
		for (int i = 0; i < sizeof(dataSave); i++) 
		{
				dataSave[i] = 0;
		}
		for (int i = 0; i < sizeof(packIndexRecord); i++) 
		{
				packIndexRecord[i] = false;
		}
}

bool ReceiveAnalysis::isAllPackGot() 
{
		for (int i = 0; i < packCount; i++) 
		{
				if (!packIndexRecord[i]) 
				{
						return false;
				}
		}
		return true;
}

bool ReceiveAnalysis::hashCompare(u8* hash1, u8* hash2) 
{
		for (int i = 0; i < 32; i++) 
		{
				if (hash1[i] != hash2[i])
						return false;
		}
		return true;
}

void ReceiveAnalysis::Poll() 
{
		switch (this->state) 
		{
				case CON_STATE_IDLE: 
				{
						break;
				}
				case CON_START_PULL: 
				{
						this->state = CON_STATE_IDLE;
						break;
				}
		}
}


