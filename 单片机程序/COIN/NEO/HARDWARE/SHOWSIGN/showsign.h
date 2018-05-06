#ifndef __SHOWSIGN_H
#define	__SHOWSIGN_H

#include "stm32f4xx.h"

typedef struct
{
    int type;
    int version;
		uint8_t coin;						// 1 ��ʾNEO�� 0 ��ʾGAS��0xff�����ݲ�ʶ��
    int countAttributes;
    int countInputs;
    int countOutputs;
    uint8_t address[2][25];//ÿһ�д洢һ����ַ��10����ַ
		uint8_t assetid[2][32];
    long long money[2];
}SIGN_Out_Para;

//��ͬ�Ľ������Ͷ��������ͱ����������ͬ������
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

int GetIndexFromBuff(char *buff,char ch);
int Ascii2Dec(char ch);
void ReverseArray(char *buf);
int bigdiv(char *diva,int lena,char *divb,int lenb);
void DecTOAddress(char *str);

void strlwr(char *buff);
void strupr(char *buff);
void HexToDec(char *src,char *result);

void Div(char s1[],char s2[]);
void convert2to16(char s2[],char s16[]);
void DecToHex(char *str,char *result);

void Convert2address(char *buff);


void HexToString(uint8_t *src,int len,char *desc);
void StringToHex(char *src,int len,uint8_t *desc);

void ReverseString(char *buf);


int ReadByteLengthIndex(int fb);
uint64_t ReadByteLength(uint8_t *buff,int start,int max,int fb);

int Alg_ShowSignData(uint8_t *dataIn,int dataInLen,SIGN_Out_Para *SIGN_Out);

#endif /* __DASHU_H */
