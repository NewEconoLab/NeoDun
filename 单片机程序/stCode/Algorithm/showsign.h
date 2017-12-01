#ifndef __SHOWSIGN_H
#define	__SHOWSIGN_H

#include "stm32f4xx.h"

//#define Printf

typedef struct
{
    int type;
    int version;
    int countAttributes;
    int countInputs;
    int countOutputs;
    char address[10][40];//每一行存储一个地址，10个地址
		char assetid[10][65];
    long long money[10];
}SIGN_Out_Para;


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
void ShowTransactionType(int type);


int ReadByteLengthIndex(int fb);
uint64_t ReadByteLength(uint8_t *buff,int start,int max,int fb);

#endif /* __DASHU_H */
