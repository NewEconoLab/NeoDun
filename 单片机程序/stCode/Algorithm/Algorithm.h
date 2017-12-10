/*
 * Algorithm.h
 *
 *  Created on: 2017年9月3日
 *      Author: Administrator
 */

#ifndef ALGORITHM_ALGORITHM_H_
#define ALGORITHM_ALGORITHM_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "showsign.h"

/*	这里是流程中需要用的算法的方法，所有的实现放到一个文件夹中,实现部分何康华维护
	接口我来维护
	所有实现部分放在一个文件夹中即可，实现要求使用纯c99代码，不得包含其他平台的头文件以防止我不能编译通过
	实现部分注意编码格式全部采用UTF-8 ,
*/

#ifdef __cplusplus
 extern "C" {
#endif


//从私钥得到公钥	返回错误代码
int32_t Alg_GetPublicFromPrivate(uint8_t *PrivateKey,uint8_t *Publickey);

//从WIF得到私钥
int32_t Alg_GetPrivateFromWIF(char *wif,uint8_t *PrivKey);

//从公钥得到地址
void Alg_GetAddressFromPublic(uint8_t * PublicKey, char *address,int len);

//哈希计算
int32_t Alg_HashData(uint8_t *dataIn,int dataInLen,uint8_t *dataOut,int *dataoutLen);
	 
//ECDSA数字签名	 
int32_t Alg_ECDSASignData(uint8_t *dataIn,int dataInLen,uint8_t *dataOut,int *dataoutLen,uint8_t *PriveKEY);	 

//解释签名函数	 
int Alg_ShowSignData(uint8_t *dataIn,int dataInLen,SIGN_Out_Para *SIGN_Out); 
	 
//Base58 编码 二进制数组 转字符串
void Alg_Base58Encode(uint8_t *dataIn , int dataInLen , char *dataOut , int *dataoutLen );
	 	 
//Base58 解码 字符串转二进制数组
void Alg_Base58Decode(char    *dataIn , int dataInLen , uint8_t *dataOut , int *dataoutLen );




//测试函数
void Alg_test(void);

#ifdef __cplusplus
 }
#endif

#endif /* ALGORITHM_ALGORITHM_H_ */

