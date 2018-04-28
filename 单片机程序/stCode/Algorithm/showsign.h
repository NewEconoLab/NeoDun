#ifndef __SHOWSIGN_H
#define	__SHOWSIGN_H

#include "stm32f4xx.h"

typedef struct
{
    int type;
    int version;
		uint8_t coin;						 // 1 表示NEO， 0 表示GAS，0xff其它暂不识别
    int countAttributes;
    int countInputs;
    int countOutputs;
    char address[10][40];//每一行存储一个地址，10个地址
		uint8_t assetid[10][32];
    long long money[10];
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
		int64_t Gas;					//InvocationTransaction	
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
