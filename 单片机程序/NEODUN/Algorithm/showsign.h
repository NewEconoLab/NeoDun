#ifndef __SHOWSIGN_H
#define	__SHOWSIGN_H

#include "stm32f4xx.h"

enum
{
		/// 用于分配字节费的特殊交易
		MinerTransaction = 0x00,

		/// 用于分发资产的特殊交易
		IssueTransaction = 0x01,

		/// 用于分配小蚁币的特殊交易
		ClaimTransaction = 0x02,

		/// 用于报名成为记账候选人的特殊交易
		EnrollmentTransaction = 0x20,

		/// 用于资产登记的特殊交易
		RegisterTransaction = 0x40,

		/// 合约交易，这是最常用的一种交易
		ContractTransaction = 0x80,

		/// Publish scripts to the blockchain for being invoked later.
		PublishTransaction = 0xd0,
	
		//nep5交易
		InvocationTransaction = 0xd1
};

//6+50+64+16=136 Byte
typedef struct
{
    uint8_t type;
    uint8_t version;
		uint8_t coin;						 //2标识nnc, 1 表示NEO， 0 表示GAS，0xff其它暂不识别
    uint8_t countAttributes;
    uint8_t countInputs;
    uint8_t countOutputs;
    uint8_t address[2][25];//每一行存储一个地址，0为from，1为to
		uint8_t assetid[2][32];
    uint64_t money[2];
}SIGN_Out_Para;

//不同的交易类型定义的数组和变量，输出不同的数据
typedef struct
{
		uint64_t Nonce;				//MinerTransaction
		uint8_t Claims[68]; 	//ClaimTransaction			PrevHash:0-63,PrevIndex:64-67
		uint8_t PublicKey[64];//EnrollmentTransaction
		int AssetType;				//RegisterTransaction
		uint64_t Amount;			//RegisterTransaction
		int Precision;				//RegisterTransaction
		uint8_t Owner[66];		//RegisterTransaction
		uint8_t Admin[40];		//RegisterTransaction
		uint8_t Code[6];			//PublishTransaction
		int NeedStorage;			//PublishTransaction
}SIGN_Out_Data;



int 		GetIndexFromBuff(char *buff,char ch);
int 		bigdiv(char *diva, int lena, char *divb, int lenb, char *shang_div,int* index_div);
uint8_t DecTOAddress(char *str,int len_of_str);
void 		mystrlwr(char *buff);
void 		mystrupr(char *buff,int len);
uint8_t HexToDec(char *src, int Lenin, char *result, int *Lenout);
void 		DecToHex(char *str,char *result);
void 		HexToString(uint8_t *src,int len,char *desc);
void 		StringToHex(char *src,int len,uint8_t *desc);
void 		ReverseString(char *buf);

#endif /* __DASHU_H */
