/*
 * ReceiveAnalysis.cpp
 *
 *  Created on: 2017年7月30日
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

extern "C"
{
#include "Algorithm.h"
#include "encrypt.h"

}

//add by hkh
#define printf_debug

bool hid_flag = false;
u8 hid_data[64];
int len_hid;
SIGN_KEY_FLAG Key_Flag;

//USB HID 接收处理函数
extern "C" void USB_DataReceiveHander(u8 * data, int len) 
{		
		len_hid = len;
		memmove(hid_data,data,len_hid);
		hid_flag = true;
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
				return false;	//校验失败
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
				switch (cmd) 
				{
						case CMD_NOTIFY_DATA://0x0101  向签名机发送数据包
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
										packIndex = 0;
										packCount = (dataLen + DATA_PACK_SIZE - 1) / DATA_PACK_SIZE;									
										u8* hash = data + 8;
										memmove(this->hashRecord, hash, 32);
#ifdef printf_debug																	
										printf("packCount = %d \r\n", packCount);									
										//Utils::PrintArray(hash, 32);
#endif										
										Commands command;
										command.CreateDataQuest(CMD_PULL_DATA, reqSerial = Utils::RandomInteger(), 0, packCount - 1, hash, 32);
										command.SendToPc();
								} 
								else 
								{
										//数据容量太大 无法缓存
										Commands command( CMD_ERR_DATA_TOO_BIG, serialId);
										command.SendToPc();
								}
								break;
						}
						case CMD_PULL_DATA_REQ://0x01a2 收到一个分片
						{
								u16 serial = Utils::ReadU16(data + 2);
								u16 packIndex = Utils::ReadU16(data + 4);
#ifdef printf_debug						
								printf("pk %d\r\n", packIndex);
#endif							
								if (serial == reqSerial) 
								{
										memmove(this->dataSave + packIndex * DATA_PACK_SIZE, data + 6,DATA_PACK_SIZE);
										packIndexRecord[packIndex] = true;
								} 
								else 
								{
#ifdef printf_debug									
										printf("not this serial");
#endif									
								}
								break;
						}
						case CMD_PULL_DATA://0x0110  上位机请求数据,主要是将签名发上去
						{			
								int blockbegin = Utils::ReadU16(data + 4);
								int blockend = Utils::ReadU16(data + 6);
								int j;
								for(int i=0;i<(blockend-blockbegin+1);i++)//????
								{
										Commands command( CMD_PULL_DATA_REQ, serialId);
										command.AppendU16(i);
										command.AppendBytes(resultsignRecord + i*DATA_PACK_SIZE,DATA_PACK_SIZE);
										command.SendToPc();				
										for(j = 0;j<0xfffff;j++);									
								}
								
								Commands command( CMD_PULL_DATA_REQ_OVER, serialId);
								command.SendToPc();
								break;
						}
						case CMD_PULL_DATA_REQ_OVER://0x01a3  接收完毕
						{
#ifdef printf_debug							
								printf("Receive data over\r\n");
#endif							
								if (isAllPackGot()) 
								{
										u8 res[32];
										Utils::Sha256(dataSave, this->dataLen, res, 32);
#ifdef printf_debug										
										printf("All pack got \r\n");
										//Utils::PrintArray(res, 32);
#endif
										if (hashCompare(res, hashRecord))//hash 正确
										{

#ifdef printf_debug											
												printf("hash ok \r\n");
#endif											
												Commands command( CMD_ACK_HASH_OK, this->notifySerial);
												command.AppendU32(0);
												command.AppendBytes(res, 32);
												command.SendToPc();
										} 
										else//hash 错误
										{
#ifdef printf_debug												
												printf("hash error \r\n");
#endif											
												Commands command( CMD_ERR_HASH, this->notifySerial);
												command.AppendU32(0);
												command.AppendBytes(res, 32);
												command.SendToPc();
										}
								} 
								else 
								{
										for (int i = 0; i < packCount; i++) 
										{
												if (packIndexRecord[i] == false)//把这一包拉下来
												{
#ifdef printf_debug														
														printf("repull %d\r\n", i);
#endif													
														Commands command;
														u8 hash[32];
														command.CreateDataQuest(CMD_PULL_DATA, reqSerial, i, i,hash, 32);
														command.SendToPc();
												}
										}
								}
								break;
						}
						case CMD_DEL_ADDRESS :
						{
								u16 addressType = Utils::ReadU16(data + 4);
#ifdef printf_debug
								printf("delete address\r\n");
								printf("addressType = 0x%04x\r\n",addressType);
#endif
								if(addressType == Address::ADDR_XiaoYi)
								{
										u8 * content = data+6;
										char temp[50] = "";
										int tempLen = 0;
										//Utils::PrintArray(content,25);
										Alg_Base58Encode(content , 25 ,temp,&tempLen);									
										
										//删除对应的地址和私钥
									
									
										
										
										Commands command( CMD_DEL_ADDRESS_OK, serialId);
										command.AppendU16(addressType);
										command.SendToPc();
										break;
								}
								else
								{
										Commands command( CMD_DEL_ADDRESS_FAILED, serialId);
										command.AppendU16(addressType);
										command.SendToPc();
										break;
								}
						}
						case CMD_ADD_ADDRESS ://0x0204  增加一个地址
						{								
								u16 addressType = Utils::ReadU16(data + 4);
#ifdef printf_debug								
								printf("add address\r\n");
							  printf("addressType = 0x%04x\r\n",addressType);	
#endif							
								if(addressType == Address::ADDR_XiaoYi) //neo的地址种类
								{
										u8 * content = data+6;
										char temp[50] = "";
										int tempLen = 0;
										//Utils::PrintArray(content,25);
										Alg_Base58Encode(content , 25 ,temp,&tempLen);																														
										u32 dataId0 = Utils::ReadU32(data +46);				
#ifdef printf_debug									
										printf("Base58 content address:%s\r\n",temp);
										printf("dataId0=%d\r\n",dataId0);	
#endif									
										PrivateKey prkey(this->dataSave + dataId0 * DATA_PACK_SIZE);//私钥的来自第一包(0x01a2)的数据																	
										PublicKey pubK = prkey.GetPublicKey(true);
										uint8_t pubKEY[33]; 
										memmove(pubKEY,pubK.getData(),33);								
										char addrCacl[40];
										Alg_GetAddressFromPublic(pubKEY,addrCacl,33);
#ifdef printf_debug									
										printf("Get address from Prikey:%s\r\n",addrCacl);																												
#endif										
										bool cmp = Utils::MemoryCompare(temp,addrCacl,tempLen);																			
										if(cmp) //地址匹配成功
										{
#ifdef printf_debug											
												printf("cmp address sucess\r\n");
#endif											
												u32 dataId1 = Utils::ReadU32(data +50);
												u32 dataId2 = Utils::ReadU32(data +54);
												u32 dataId3 = Utils::ReadU32(data +58);																								
												//字符串类型的地址，私钥，地址种类记录下来，保存到队列中
												Address addr(temp,prkey.getData(),addressType,dataId0,dataId1,dataId2,dataId3 );
												this->addreses.add(addr);
												//printf("addr = %s \r\n", content );
												Commands command( CMD_ADDR_ADD_OK, serialId);
												command.AppendU16(addressType);
												command.AppendBytes(data+6,Address::ADDR_SIZE);
												command.SendToPc();
												break;																								
										}
										else		 //地址匹配失败
										{
												Commands command( CMD_ADDR_ADD_FAILED, serialId);
												command.AppendU16(addressType);
												command.AppendBytes(data+6,40+2);
												command.SendToPc();						

												break;
										}
								}
								else  //不是neo的地址种类，直接失败
								{										
										Commands command( CMD_ADDR_ADD_FAILED, serialId);
										command.AppendU16(addressType);
										command.AppendBytes(data+6,40+2);
										command.SendToPc();
										break;
								}
						}
						case CMD_QUERY_ADDRESS ://0x0201  查询地址
						{
								int j;
#ifdef printf_debug							
								printf("getadr=%d\r\n",addreses.getContentSize());
#endif							
								for(int i = 0 ; i < this->addreses.getContentSize() ; i++  )
								{
										Address & addr =  this->addreses.get(i);										
										char buff [40] = "";
										u8 result[52];
										int lenout = 0;									
										memset(result,0,52);
										memmove(buff,addr.getContent(),Address::ADDR_SIZE);
										int len  = strlen(buff);												
										Alg_Base58Decode(buff,len,result,&lenout);
#ifdef printf_debug									
										printf("lenout:%d\r\n",lenout);
										Utils::PrintArray(result,lenout);
#endif										
										Commands command( CMD_RETURN_ADDRESS, serialId);
										command.AppendU16(i);
										command.AppendU16(addreses.getContentSize());
										command.AppendU16(Address::ADDR_XiaoYi );										
										command.AppendBytes((u8*)result,lenout);
										command.SendToPc();
#ifdef printf_debug									
										printf("adr = %s \r\n", addr.getContent());
#endif										
										for(j = 0;j<0xfffff;j++);
								}
								
								Commands command( CMD_RETURN_ADDRESS_OK, serialId);
								command.AppendU16(addreses.getContentSize());
								command.SendToPc();
								break;
						}
						case CMD_GET_PRIKEY://0x0206  获取地址安全信息
						{						
								int index = 0;
								u16 addressType = Utils::ReadU16(data+8);
#ifdef printf_debug							
								printf("Get security message\r\n");
								printf("addressType = 0x%04x\r\n",addressType);
#endif									
								if(1)//同意
								{
										u8 *content = data+10;
										char temp[50] = "";
										int tempLen = 0;
										u8 hash[32];
#ifdef printf_debug									
										//Utils::PrintArray(content,25);
#endif										
										Alg_Base58Encode(content , 25 ,temp,&tempLen );
										for(int i=0;i<this->addreses.getContentSize();i++)
										{
												Address & addr =  this->addreses.get(i);																							
												if(Utils::MemoryCompare(addr.getContent(),temp,tempLen))
												{
														index = i;
												}																									
										}
#ifdef printf_debug										
										printf("index = %d\r\n",index);
#endif													
										Address & addr =  this->addreses.get(index);
										Utils::Sha256(addr.getPrivatekey(), 32, hash, 32);										
										Commands command( CMD_RETURN_MESSAGE, reqSerial);
										command.AppendU32(len);
										command.AppendBytes(hash,32);
										command.SendToPc();																				
										
										//这里还需指定待会上位机需要拉的数据的存储位置
										
										
										
										break;
								}
								else//拒绝
								{
										Commands command( CMD_SIGN_FAILED, serialId);
										command.SendToPc();
										break;								
								}								
						}
						case CMD_SIGN_DATA ://0x020a  签名
						{								
								int index = 0;
								u16 addressType = Utils::ReadU16(data + 4);							  							
								u32 dataId0 = Utils::ReadU32(data +46);
#ifdef printf_debug								
								printf("sign data\r\n");
								printf("addressType = 0x%04x\r\n",addressType);	
								printf("dataId0 = %d\r\n",dataId0);
#endif							
								if(addressType == Address::ADDR_XiaoYi)
								{									
										u8 * content = data+6;
										char temp[50] = "";
										int tempLen = 0;
#ifdef printf_debug									
										//Utils::PrintArray(content,25);
#endif										
										Alg_Base58Encode(content , 25 ,temp,&tempLen );																																				
										for(int i=0;i<this->addreses.getContentSize();i++)
										{
												Address & addr =  this->addreses.get(i);																							
												if(Utils::MemoryCompare(addr.getContent(),temp,tempLen))
												{
														index = i;
												}																									
										}
#ifdef printf_debug										
										printf("index = %d\r\n",index);
#endif								
								}
																
								SIGN_Out_Para Sign;
								memset(&Sign,0,sizeof(Sign));
								int result = Alg_ShowSignData(this->dataSave + (dataId0-2) * DATA_PACK_SIZE,this->dataLen,&Sign);//?????
								if(result == 1)
								{
										Commands command( CMD_SIGN_FAILED, serialId);
										command.SendToPc();
										break;										
								}
																
								//printf("您有一笔交易需要您签名，是否同意？\r\n");
								//view::DisplayMem::getInstance().drawHZString(0,32,0,15);
								
								Address & addr =  this->addreses.get(index);
								for(int i = 0;i < Sign.countOutputs;i++)
								{																				
										if(Utils::MemoryCompare(Sign.address[i],addr.getContent(),strlen(Sign.address[i])))//对地址进行比较
										{
												continue;
										}
										
										view::DisplayMem::getInstance().clearAll();//清除显示
										//手续费 money NEO,确认发送？   浮点数用整数和小数分别显示
										view::DisplayMem::getInstance().drawHZString(0,0,0,2);
										//显示手续费   该值为一个long long型  对应的十进制数的低八位数为小数
										int count_bit=0;
										int count_int = 0;//表示整数部分占用的显示位数
										int count_dec = 0;//表示小数部分后缀的零的个数
										
										count_int = view::DisplayMem::getInstance().drawNumber(40,0,Sign.money[i]/100000000,8,view::FONT_8X16);
										if(Sign.money[i]%100000000)//消除值正好为100000000的显示BUG
										{
												view::DisplayMem::getInstance().drawString(40+count_int*8,0,".");												
											  count_dec = view::DisplayMem::getInstance().drawxNumber(40+(count_int+1)*8,0,Sign.money[i]%100000000,8,view::FONT_8X16) - 1;//-1是把小数点算进去
										}
										if(Sign.money[i] == 0)//值为0的情况
												count_dec = 8;
										count_bit = 40 + (count_int + 8 -count_dec)*8 + 4; //40是前面占用的显示，+4是显示空隙，美观
										
										//view::DisplayMem::getInstance().drawString(40,0,"0.1");
										view::DisplayMem::getInstance().drawString(count_bit,0,"NEO");
										view::DisplayMem::getInstance().drawHZString(count_bit+26,0,3,8);
									
										view::DisplayMem::getInstance().drawHZString(36,48,9,10);
										view::DisplayMem::getInstance().drawHZString(124,48,4,5);	
										view::DisplayMem::getInstance().drawHZString(208,48,9,10);		
										
										view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],5,7,48,60);//画三角形
										view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],27,29,48,60);//画三角形
										view::DisplayMem::getInstance().drawPicture(&gImage_triangle[0],48,50,48,60);//画三角形	
										view::DisplayMem::getInstance().clearArea(0,60,256,1);//清除最后一行的白点	
								}								
										
								Key_Flag.Sign_Key_Flag = 1;
								while(1)//签名机按键按下同意按钮
								{		
										if(Key_Flag.Sign_Key_center_Flag)
										{		
												Key_Flag.Sign_Key_Flag = 0;
												u8 hash[32];
												int len;
												u8 resultsign[64];
											
												for(int m=0;m<64;m++) resultsignRecord[m] = 0;
												//签名的结果保存
												Alg_ECDSASignData(this->dataSave + dataId0 * DATA_PACK_SIZE,this->dataLen,resultsign,&len,addr.getPrivatekey());
												
												//组合成最终的数据，长度1字节+公钥33字节+签名结果64字节
												PrivateKey prkey(addr.getPrivatekey());															
												PublicKey pubK = prkey.GetPublicKey(true);
												uint8_t pubKEY[33]; 
												memmove(pubKEY,pubK.getData(),33);																				
												resultsignRecord[0] = 33;
												memmove(resultsignRecord+1,pubKEY,33);
												memmove(resultsignRecord+34,resultsign,64);
											
												Utils::PrintArray(resultsignRecord,98);
												Utils::Sha256(resultsignRecord, 98, hash, 32);										
												
												if(1)//准备好数据块，飞回去		将outdata 发回上位机
												{
														reqSerial = Utils::RandomInteger();
														Commands command( CMD_NOTIFY_DATA, reqSerial);
														command.AppendU32(len);
														command.AppendBytes(hash,32);
														command.SendToPc();
												}
												
												for(int j = 0;j<0xfffff;j++);
												//再发条通知消息 告诉上位机hash
												Commands command( CMD_SIGN_OK, serialId);
												command.AppendU32(len);
												command.AppendBytes(hash,32);									
												command.SendToPc();									
												view::DisplayMem::getInstance().drawString(26,20,"Welcom Use NeoDun",view::FONT_12X24);
												break;												
									  }
										else if((Key_Flag.Sign_Key_left_Flag)||(Key_Flag.Sign_Key_right_Flag))//按下拒绝签名按钮
										{
												Key_Flag.Sign_Key_Flag = 0;
												Commands command( CMD_SIGN_FAILED, serialId);
												command.SendToPc();								
												view::DisplayMem::getInstance().drawString(26,20,"Welcom Use NeoDun",view::FONT_12X24);
												break;
										}										
								}
								Key_Flag.Sign_Key_Flag = 0;
								break;
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


