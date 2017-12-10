#include "Algorithm.h"
//#include "usart.h"

////  用来测试的WIF码
//char test_wif[] = "Ky1X7GjA1y7UrUWhSWvAvNGdgWNvVJcL1t3AgRMApTcPkf85XS3i";

////  用来测试的公钥
//uint8_t testbuff_pubkey[64] = 
//{
//		0x71,0x33,0x44,0x7e,0x32,0xfd,0x2d,0xbd,0x9d,0xb9,0x36,0x01,0xe2,0x07,0x32,0x51,
//		0x51,0xeb,0x4c,0x3e,0x3e,0x3e,0x1b,0x1b,0x87,0x8f,0xf9,0x80,0xc3,0x15,0xeb,0xfa,
//		0x12,0xbc,0xdd,0x9d,0xb7,0x3f,0xeb,0xf9,0xf7,0xe9,0x76,0x21,0xc0,0x15,0xe4,0xd6,
//		0x05,0xa0,0xac,0xa3,0x30,0xc2,0xce,0xb0,0xbf,0x51,0xc0,0xb3,0x58,0x89,0x7f,0x4c
//};

////  用来测试的解释签名数据
//char test_buff[2048] = "80000002235fc72a3372fa601d39017685c3a77ebb24f1369a16e80f218f5ff76880fb3800005d1ab60715d73e806cca1449fc089520211360ea526450803345e8987ad35e76010002e72d286979ee6cb1b7e65dfddfb2e384100b8d148e7758de42e4168b71792c60008b585a170000001b02c180df019e6113a985411cae62db80f90db4e72d286979ee6cb1b7e65dfddfb2e384100b8d148e7758de42e4168b71792c6070222f1817000000738679b1fd7dbc21fa7ebc1218e74f08e6afbdae";

////  用来测试的签名数据
////uint8_t ECDSAInputmessage[] = {1,2,3};//签名的数据		

////	用来测试Base58编码的数据
//uint8_t test_base58encode[25] = {0x17,0x88,0xd4,0x8e,0x42,0xa4,0xe5,0xcb,0x34,0x0b,0x5c,0x82,0x01,0x75,0xcd,
//																 0xc7,0x2d,0xff,0xa8,0x24,0xf0,0x67,0xea,0x7f,0x88};

//uint8_t test_prikey[32] = {0x35,0x67,0x3e,0xcf,0x9f,0x18,0xf4,0x4e,0xbd,0x1c,0x9f,0xc7,0xca,0xc4,0x32,0x7d,
//													 0x72,0xee,0xb0,0x81,0x4,0xef,0xfd,0x20,0xfb,0xbb,0x62,0x13,0x41,0xb8,0xa,0xc1} ;
																 
//uint8_t result_pubkey[64];
//uint8_t result_prikey[32];
//char test_address[50] = "";
//uint8_t test_sign[64];
//uint8_t test_hash[32];
//uint8_t test_base58decode[50];
//char test_base58Encode[50] = "";	
	

////测试函数
//void Alg_test(void)
//{
//		int32_t value = 0,i = 0;
//		int len_sign;
//		int len_hash;
//		int len_base58decode;
//		int len_base58encode;
	
		
//		printf("WIF:%s\r\n",test_wif);
//		value = Alg_GetPrivateFromWIF(test_wif,result_prikey);
//	  printf("prikey:");
//		for(i=0;i<32;i++)
//				printf(" 0x%x",result_prikey[i]);
//		printf("\r\n");
//		printf("value = %d\r\n",value);
		
		
//		value = Alg_GetPublicFromPrivate(test_prikey,result_pubkey);
//		printf("pubkey:");
//		for(i=0;i<64;i++)
//				printf(" 0x%x",result_pubkey[i]);
//		printf("\r\n");
//		printf("value = %d\r\n",value);

		
//		Alg_GetAddressFromPublic(testbuff_pubkey,test_address);
//		printf("address = %s\r\n",test_address);
	
		
//		Alg_Base58Encode(test_base58encode,25,test_base58Encode,&len_base58encode);
//		printf("len of Base58Encode : %d\r\n",len_base58encode);
//		printf("result of Base58Encode : %s\r\n",test_base58Encode);

		
//		Alg_Base58Decode(test_wif,52,test_base58decode,&len_base58decode);
//		printf("result of Base58Decode:");
//		for(i=0;i<len_base58decode;i++)
//				printf(" 0x%x",test_base58decode[i]);
//		printf("\r\n");
	

//		printf("哈希计算\r\n");
//		value = Alg_HashData(ECDSAInputmessage,3,test_hash,&len_hash);
//		for(i=0;i<len_hash;i++)
//				printf(" 0x%x",test_hash[i]);
//		printf("\r\n");
//		printf("value = %d\r\n",value);	


//		printf("数据签名\r\n");
//		value = Alg_ECDSASignData(ECDSAInputmessage,3,test_sign,&len_sign,test_prikey);
//		for(i=0;i<len_sign;i++)
//				printf(" 0x%x",test_sign[i]);
//		printf("\r\n");
//		printf("value = %d\r\n",value);		
		
//}


