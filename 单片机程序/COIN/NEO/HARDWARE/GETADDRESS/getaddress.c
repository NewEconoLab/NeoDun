#include "getaddress.h" 
#include "encrypt.h"
#include "showsign.h"
#include "bignum.h"   
#include "string.h"
#include "usart.h"
#include "ripemd160.h"

extern char Dec[BUFSIZ];				//�洢10���ƵĴ���
extern char Hex[BUFSIZ];				//�洢16���ƵĴ���
extern char finaladdress[50];
extern char Alphabet[58];

/******************************************************************
*	��������	 GetAddressFromPublickey
*	����˵�����ӹ�Կ�м������ַ
* ���������publickey	��Կ
					 len				��Կ����
* ���������
*******************************************************************/												
void GetAddressFromPublickey(uint8_t *publickey,int len)
{		
		int i;
		uint8_t src[len+2];		
		uint8_t hash1[32];
		uint8_t hash2[20];
		uint8_t data[21] = {0x17};
		char stringdata[42];
		
		//�������������src
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
		
		//����ϵ����ݽ�������hash����
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
		
		//��hash����Ľ������58���룬���ɵõ���ַ
//		Base58_Encode(stringdata);
		printf("address = %s\r\n",finaladdress);
}

/******************************************************************
*	��������	 Base58_25Bytes
*	����˵������������ַ�������ϳ�Base58�����25�ֽ�����
* ���������buff ��������
					  lenin ��������ĳ���
* ���������result ������飬����Ϊ25�ֽ�
*******************************************************************/	
void	Base58_25Bytes(uint8_t *buff,int lenin,uint8_t result[25])
{
		uint8_t digest[32];
		uint8_t digest1[32];
		uint8_t i;
		int hashlen = 32;

		//�������ι�ϣ����õ�ժҪ
		Alg_HashData(buff,lenin,digest,&hashlen);
		Alg_HashData(digest,32,digest1,&hashlen);	
	
		//����ժҪ�ĺ���λ����ϳ����յ�����alldata
		for(i=0;i<21;i++)		
				result[i] = buff[i];
		for(i=0;i<4;i++)
				result[21+i] = digest1[i];
}

/******************************************************************
*	��������	 Base58_Encode
*	����˵������������ַ��������Base58�����������ַ��
						�洢��ȫ������address��
* ���������buff ��������
					 lenin ��������ĳ���
* ���������result �������
					 lenout �������ĳ��� 
*******************************************************************/	
void Base58_Encode(uint8_t *buff,int lenin,char *result,int *lenout)
{
		uint8_t alldata[25];
		uint8_t digest[32];
		uint8_t digest1[32];	
	  char address58[BUFSIZ];	
		int i,hashlen =32;	
	
		//�������ι�ϣ����õ�ժҪ
		Alg_HashData(buff,lenin,digest,&hashlen);
		Alg_HashData(digest,32,digest1,&hashlen);
	
	
		//����ժҪ�ĺ���λ����ϳ����յ�����alldata
		for(i=0;i<21;i++)		
				alldata[i] = buff[i];
		for(i=0;i<4;i++)
				alldata[21+i] = digest1[i];	
		
	  //����base58���룬�õ�address
		HexToString(alldata,25,Hex);//������ת�����ַ�����ʽ		
		HexToDec(Hex,address58);//��16���Ƶ��ַ���ת����10���Ƶ��ַ���
		DecTOAddress(address58);//��10���Ʊ�ʾ���ַ����������е�ַ����
		*lenout = strlen(finaladdress);	
		for(i=0;i<(*lenout);i++)
				result[i] = finaladdress[i];
}

/******************************************************************
*	��������	 Base58Encode
*	����˵������������ַ��������Base58�����������ַ
* ���������dataIn 	  ��������  ��һ��ֵΪ0x17  ,����ĸ�ֵΪ��ϣ�����ֵ
					 dataInLen  ��������ĳ���  25
* ���������dataOut 		�������
					 dataoutLen �������ĳ��� 
*******************************************************************/	
void Alg_Base58Encode(uint8_t *dataIn , int dataInLen , char *dataOut , int *dataoutLen )
{	
	  char address58[BUFSIZ];	
		int i;	
		
	  //����base58���룬�õ�address
		HexToString(dataIn,dataInLen,Hex);//������ת�����ַ�����ʽ		
		HexToDec(Hex,address58);//��16���Ƶ��ַ���ת����10���Ƶ��ַ���
		DecTOAddress(address58);//��10���Ʊ�ʾ���ַ����������е�ַ����
		*dataoutLen = strlen(finaladdress);	
		for(i=0;i<(*dataoutLen);i++)
				dataOut[i] = finaladdress[i];
}

/******************************************************************
*	��������	 Base58_Decode
*	����˵������������ַ��������Base58�����������ַ��
						�洢��ȫ������address��
* ���������buff ��������                
* ���������
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
	
		length = strlen(buff);	//�����ַ����ĳ���		
		for(i=length-1;i>=0;i--)
		{		
				index = GetIndexFromBuff(Alphabet,buff[i]);//�õ�10���Ƶ�Int�������±�
				//���±�ת�����ַ���ʽ���洢��Ҫ�����������
				if(index <10)
				{	
						index_string[0] = index + '0';
				}
				else
				{
						index_string[0] = index/10 + '0';
						index_string[1] = index%10 + '0';
				}					
							
				bigpow(a,2,length-1-i,temp_pow);//����������			

//				printf("temp_pow:%s\r\n",temp_pow);
				
				bigmul(index_string,strlen(index_string),temp_pow,strlen(temp_pow),temp);//�±�˷�����				

//				printf("temp_pow:%s\r\n",temp);
				
				bigadd(temp,strlen(temp),bi,strlen(bi),final);//�ӷ��ۻ�����		

//				printf("final:%s\r\n",final);
				
				memset(bi,0,512);				
				for(j=0;j<strlen(final);j++)		//�õ�����Ľ�� bi
						bi[j] = final[j];				
				//�����´μ���ǰ���������
				memset(temp_pow,0,512);
				memset(temp,0,512);
				memset(final,0,512);
				memset(index_string,0,2);
				a[0] = '5';
				a[1] = '8';
		}
		DecToHex(bi,result_hex);		
		
//		printf("result_hex:%s\r\n",result_hex);
		
//		ReverseString(result_hex);	//��ת����	
		StringToHex(result_hex,strlen(result_hex)/2,bytes);//���ַ����鱣���uuint8_t����
				
		if((sizeof(bytes)>1)&&(bytes[0]==0)&&(bytes[1]>=0x80))//�Ƿ�Ϊ���������
				stripSignByte = 1;
		else
				stripSignByte = 0;
		
		for(i=0;i<length && buff[i] == Alphabet[0];i++)//����ǰ��0�ĸ��� 
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


