#include "getaddress.h" 
#include "encrypt.h"
#include "showsign.h"
#include "bignum.h"   
#include "string.h"
#include "ripemd160.h"
#include "Algorithm.h"

#define  ADDRESS_BUF_LEN	100
extern char finaladdress[40];
extern char Alphabet[58];

/******************************************************************
*	函数名：	 GetAddressFromPublickey
*	函数说明：从公钥中计算出地址
* 输入参数：publickey	公钥
* 输出参数：address		地址
						len       公钥标准长度，必须为33或者65
		成功		  		返回0
		输入数据错误  返回1
		地址转换错误  返回2
*******************************************************************/			
uint8_t Alg_GetAddressFromPublic(uint8_t * PublicKey, char *address,int len)
{		
		int i;
		int lenout=0,lenhash = 32;
		uint8_t src1[35];
		uint8_t src2[67];
		uint8_t hash1[32];
		uint8_t hash2[20];
		uint8_t data[21] = {0x17};
		
		memset(hash1,0,32);
		memset(hash2,0,20);
		memset(address,0,40);
		if((len != 33)&&(len != 65))
				return 1;
		
		if(len == 33)
		{	
				//组合完整的数据src
				src1[0] = len;
				src1[len+1] = 172;
				for(i=0;i<len;i++)
				{
						src1[i+1] = PublicKey[i];
				}					
				//对组合的数据进行两次hash计算
				Alg_HashData(src1,len+2,hash1,&lenhash);			
		}
		else
		{
				//组合完整的数据src
				src2[0] = len;
				src2[len+1] = 172;
				for(i=0;i<len;i++)
				{
						src2[i+1] = PublicKey[i];
				}					
				//对组合的数据进行两次hash计算
				Alg_HashData(src2,len+2,hash1,&lenhash);		
		}		

		Ripemd160(hash1,hash2);			
		for(i=0;i<20;i++)
		{
				data[i+1] = hash2[i];
		}
		//对hash计算的结果进行58编码，即可得到地址
		return Base58_Encode(data,21,address,&lenout);
}

/******************************************************************
*	函数名：	 Base58_Encode
*	函数说明：对输入的字符数组进行Base58编码解析出地址，
						存储在全局数组address中
* 输入参数：buff 输入数组
					  lenin 输入数组的长度
* 输出参数：result 输出数组
					  lenout 输出数组的长度 
		成功		  		返回0
		输入数据错误  返回1
		地址转换错误  返回2
*******************************************************************/	
uint8_t Base58_Encode(uint8_t *buff,int lenin,char *result,int *lenout)
{
		uint8_t alldata[25];
		uint8_t digest[32];
		uint8_t digest1[32];	
	  char address58[BUFSIZ] = "";
		char Hex[BUFSIZ] = "";
		int i,hashlen =32;
		int Len_of_Dec = 0;

		//进行两次哈希计算得到摘要
		Alg_HashData(buff,lenin,digest,&hashlen);
		Alg_HashData(digest,32,digest1,&hashlen);	
	
		//拷贝摘要的后四位，组合成最终的数据alldata
		for(i=0;i<21;i++)		
				alldata[i] = buff[i];
		for(i=0;i<4;i++)
				alldata[21+i] = digest1[i];

		HexToString(alldata,25,Hex);										//将数据转换成字符串形式		
		if(HexToDec(Hex,50,address58,&Len_of_Dec) == 0) //将16进制的字符串转换成10进制的字符串
		{
				return 1;
		}
		if(DecTOAddress(address58,Len_of_Dec) == 0)		  //对10进制表示的字符串大数进行地址解析
		{
				return 2;
		}
		*lenout = strlen(finaladdress);	
		for(i=0;i<(*lenout);i++)
				result[i] = finaladdress[i];
		return 0;
}

/******************************************************************
*	函数名：	Alg_Base58Encode
*	函数说明：对输入的字符数组进行Base58编码解析出地址
* 输入参数：dataIn 	   输入数组  第一个值为0x17  ,最后四个值为哈希计算的值
					  dataInLen  输入数组的长度  25
* 输出参数：dataOut 	 输出数组
					  dataoutLen 输出数组的长度
		成功		  		返回0
		输入数据错误  返回1
		地址转换错误  返回2
*******************************************************************/	
uint8_t Alg_Base58Encode(uint8_t *dataIn , int dataInLen , char *dataOut , int *dataoutLen )
{	
	  char address58[ADDRESS_BUF_LEN] = "";
		char Hex[ADDRESS_BUF_LEN] = "";
		int Len_of_Dec = 0;
		int i;
		
		HexToString(dataIn,dataInLen,Hex);											//将数据转换成字符串形式		
		if(HexToDec(Hex,2*dataInLen,address58,&Len_of_Dec) == 0)//将16进制的字符串转换成10进制的字符串
		{
				return 1;
		}
		if(DecTOAddress(address58, Len_of_Dec) == 0)						//对10进制表示的字符串大数进行地址解析
		{
				return 2;
		}
		*dataoutLen = strlen(finaladdress);	
		for(i=0;i<(*dataoutLen);i++)
				dataOut[i] = finaladdress[i];
		return 0;
}

/******************************************************************
*	函数名：	 Base58_Decode
*	函数说明：对输入的字符数组进行Base58编码解析出地址，
						存储在全局数组address中
* 输入参数：dataIn 		输入数组
					 dataInLen  输入数组长度
* 输出参数：dataOut		输出数组
					 dataoutLen 输出数组长度
*******************************************************************/
void Alg_Base58Decode(char    *dataIn , int dataInLen , uint8_t *dataOut , int *dataoutLen )
{
		int i,j,index,stripSignByte;
		int leadingZeros = 0;
		char index_string[2] = "";
		char a[2] = "58";
		char bi[BUFSIZ] = "";
		char temp_pow[BUFSIZ] = "";
		char temp[BUFSIZ] = "";
		char final[BUFSIZ] = "";
		char result_hex[BUFSIZ] = "";
		uint8_t bytes[BUFSIZ] = "";
	

		for(i=dataInLen-1;i>=0;i--)
		{		
				index = GetIndexFromBuff(Alphabet,dataIn[i]);//得到10进制的Int型数组下标
				//将下标转换成字符形式，存储到要计算的数组中
				if(index <10)
				{	
						index_string[0] = index + '0';
				}
				else
				{
						index_string[0] = index/10 + '0';
						index_string[1] = index%10 + '0';
				}					
							
				bigpow(a,2,dataInLen-1-i,temp_pow);//进行幂运算							
				bigmul(index_string,strlen(index_string),temp_pow,strlen(temp_pow),temp);//下标乘法运算				
				bigadd(temp,strlen(temp),bi,strlen(bi),final);//加法累积运算						
				memset(bi,0,512);				
				for(j=0;j<strlen(final);j++)		//得到计算的结果 bi
						bi[j] = final[j];				
				//进行下次计算前，清空数组
				memset(temp_pow,0,512);
				memset(temp,0,512);
				memset(final,0,512);
				memset(index_string,0,2);
				a[0] = '5';
				a[1] = '8';
		}
		DecToHex(bi,result_hex);
		j = strlen(result_hex)/2;
		StringToHex(result_hex,j,bytes);//将字符数组保存成uint8_t数组
				
		if((sizeof(bytes)>1)&&(bytes[0]==0)&&(bytes[1]>=0x80))//是否为特殊情况？
				stripSignByte = 1;
		else
				stripSignByte = 0;
		
		for(i=0;i<dataInLen && dataIn[i] == Alphabet[0];i++)//计算前导0的个数 
				leadingZeros++;
		
//		len = dataInLen - stripSignByte + leadingZeros;		
		if(stripSignByte)
		{
				for(i=0;i<dataInLen-leadingZeros;i++)
						dataOut[i+leadingZeros] = bytes[i+1];
		}
		else
		{
				for(i=0;i<dataInLen-leadingZeros;i++)
						dataOut[i+leadingZeros] = bytes[i];
		}	
		
		while(dataOut[--j] == 0)//避免数组中间出现0的情况，出现计数错误
		{				
		}		
		*dataoutLen = j + 1;	
}
