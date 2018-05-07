#include "getaddress.h" 
#include "encrypt.h"
#include "showsign.h"
#include "bignum.h"   
#include "string.h"
#include "usart.h"
#include "ripemd160.h"

extern char Dec[BUFSIZ];				//存储10进制的大数
extern char Hex[BUFSIZ];				//存储16进制的大数
extern char finaladdress[50];
extern char Alphabet[58];

/******************************************************************
*	函数名：	 GetAddressFromPublickey
*	函数说明：从公钥中计算出地址
* 输入参数：publickey	公钥
					 len				公钥长度
* 输出参数：
*******************************************************************/												
void GetAddressFromPublickey(uint8_t *publickey,int len)
{		
		int i;
		uint8_t src[len+2];		
		uint8_t hash1[32];
		uint8_t hash2[20];
		uint8_t data[21] = {0x17};
		char stringdata[42];
		
		//组合完整的数据src
		src[0] = len;
		src[len + 1] = 172;
		for(i=0;i<len;i++)
		{
				src[i+1] = publickey[i];
		}	

		printf("src:\r\n");
		for(i=0;i<len+2;i++)
				printf("0x%x ",src[i]);
		printf("\r\n");
		
		//对组合的数据进行两次hash计算
//		HashData(src,len+2,hash1,32);	
		
		printf("hash1:\r\n");
		for(i=0;i<32;i++)
				printf("0x%x ",hash1[i]);
		printf("\r\n");
		
		Ripemd160(hash1,hash2);			

		printf("hash2:\r\n");
		for(i=0;i<20;i++)
				printf("0x%x ",hash2[i]);
		printf("\r\n");		
		
		for(i=0;i<20;i++)
		{
				data[i+1] = hash2[i];
		}			
		
		HexToString(data,21,stringdata);
		printf("stringdata = %s\r\n",stringdata);
		
		//对hash计算的结果进行58编码，即可得到地址
//		Base58_Encode(stringdata);
		printf("address = %s\r\n",finaladdress);
}

/******************************************************************
*	函数名：	 Base58_25Bytes
*	函数说明：对输入的字符数组组合成Base58编码的25字节数组
* 输入参数：buff 输入数组
					  lenin 输入数组的长度
* 输出参数：result 输出数组，长度为25字节
*******************************************************************/	
void	Base58_25Bytes(uint8_t *buff,int lenin,uint8_t result[25])
{
		uint8_t digest[32];
		uint8_t digest1[32];
		uint8_t i;
		int hashlen = 32;

		//进行两次哈希计算得到摘要
		Alg_HashData(buff,lenin,digest,&hashlen);
		Alg_HashData(digest,32,digest1,&hashlen);	
	
		//拷贝摘要的后四位，组合成最终的数据alldata
		for(i=0;i<21;i++)		
				result[i] = buff[i];
		for(i=0;i<4;i++)
				result[21+i] = digest1[i];
}

/******************************************************************
*	函数名：	 Base58_Encode
*	函数说明：对输入的字符数组进行Base58编码解析出地址，
						存储在全局数组address中
* 输入参数：buff 输入数组
					 lenin 输入数组的长度
* 输出参数：result 输出数组
					 lenout 输出数组的长度 
*******************************************************************/	
void Base58_Encode(uint8_t *buff,int lenin,char *result,int *lenout)
{
		uint8_t alldata[25];
		uint8_t digest[32];
		uint8_t digest1[32];	
	  char address58[BUFSIZ];	
		int i,hashlen =32;	
	
		//进行两次哈希计算得到摘要
		Alg_HashData(buff,lenin,digest,&hashlen);
		Alg_HashData(digest,32,digest1,&hashlen);
	
	
		//拷贝摘要的后四位，组合成最终的数据alldata
		for(i=0;i<21;i++)		
				alldata[i] = buff[i];
		for(i=0;i<4;i++)
				alldata[21+i] = digest1[i];	
		
	  //进行base58编码，得到address
		HexToString(alldata,25,Hex);//将数据转换成字符串形式		
		HexToDec(Hex,address58);//将16进制的字符串转换成10进制的字符串
		DecTOAddress(address58);//对10进制表示的字符串大数进行地址解析
		*lenout = strlen(finaladdress);	
		for(i=0;i<(*lenout);i++)
				result[i] = finaladdress[i];
}

/******************************************************************
*	函数名：	 Base58Encode
*	函数说明：对输入的字符数组进行Base58编码解析出地址
* 输入参数：dataIn 	  输入数组  第一个值为0x17  ,最后四个值为哈希计算的值
					 dataInLen  输入数组的长度  25
* 输出参数：dataOut 		输出数组
					 dataoutLen 输出数组的长度 
*******************************************************************/	
void Alg_Base58Encode(uint8_t *dataIn , int dataInLen , char *dataOut , int *dataoutLen )
{	
	  char address58[BUFSIZ];	
		int i;	
		
	  //进行base58编码，得到address
		HexToString(dataIn,dataInLen,Hex);//将数据转换成字符串形式		
		HexToDec(Hex,address58);//将16进制的字符串转换成10进制的字符串
		DecTOAddress(address58);//对10进制表示的字符串大数进行地址解析
		*dataoutLen = strlen(finaladdress);	
		for(i=0;i<(*dataoutLen);i++)
				dataOut[i] = finaladdress[i];
}

/******************************************************************
*	函数名：	 Base58_Decode
*	函数说明：对输入的字符数组进行Base58编码解析出地址，
						存储在全局数组address中
* 输入参数：buff 输入数组                
* 输出参数：
*******************************************************************/
void Base58_Decode(char *buff,uint8_t *result)
{
		int i,j,length,index,stripSignByte;
		int leadingZeros = 0;
		char index_string[2] = "";
		char a[2] = "58";
		char bi[BUFSIZ] = "";
		char temp_pow[BUFSIZ] = "";
		char temp[BUFSIZ] = "";
		char final[BUFSIZ] = "";
		char result_hex[BUFSIZ] = "";
		uint8_t bytes[BUFSIZ];
	
		length = strlen(buff);	//计算字符串的长度		
		for(i=length-1;i>=0;i--)
		{		
				index = GetIndexFromBuff(Alphabet,buff[i]);//得到10进制的Int型数组下标
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
							
				bigpow(a,2,length-1-i,temp_pow);//进行幂运算			

//				printf("temp_pow:%s\r\n",temp_pow);
				
				bigmul(index_string,strlen(index_string),temp_pow,strlen(temp_pow),temp);//下标乘法运算				

//				printf("temp_pow:%s\r\n",temp);
				
				bigadd(temp,strlen(temp),bi,strlen(bi),final);//加法累积运算		

//				printf("final:%s\r\n",final);
				
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
		
//		printf("result_hex:%s\r\n",result_hex);
		
//		ReverseString(result_hex);	//翻转数组	
		StringToHex(result_hex,strlen(result_hex)/2,bytes);//将字符数组保存成uuint8_t数组
				
		if((sizeof(bytes)>1)&&(bytes[0]==0)&&(bytes[1]>=0x80))//是否为特殊情况？
				stripSignByte = 1;
		else
				stripSignByte = 0;
		
		for(i=0;i<length && buff[i] == Alphabet[0];i++)//计算前导0的个数 
				leadingZeros++;
		
//		len = length - stripSignByte + leadingZeros;		
		if(stripSignByte)
		{
				for(i=0;i<length-leadingZeros;i++)
						result[i+leadingZeros] = bytes[i+1];
		}
		else
		{
				for(i=0;i<length-leadingZeros;i++)
						result[i+leadingZeros] = bytes[i];				
		}	
}


