#ifndef __APP_ECDSA_H
#define __APP_ECDSA_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "crypto.h"
#include "main_define.h"

#define   CONCAT(A, B)               (A ## B)
#define   _HASH_INIT(e)              CONCAT(e, _Init)
#define   _HASH_APPEND(e)            CONCAT(e, _Append)
#define   _HASH_FINISH(e)            CONCAT(e, _Finish)

#if       ECC_HASH == SHA256
#define   HASH_INIT(x)               _HASH_INIT(SHA256)(x)
#define   HASH_FINISH(x, y, z)       _HASH_FINISH(SHA256)(x, y, z) 
#define   HASH_APPEND(x, y, z)       _HASH_APPEND(SHA256)(x, y, z) 
#define   CRL_HASH_SIZE              CRL_SHA256_SIZE
#define   HASHctx_stt                SHA256ctx_stt
#elif     ECC_HASH == SHA1
#define   HASH_INIT(x)               _HASH_INIT(SHA1)(x)
#define   HASH_FINISH(x, y, z)       _HASH_FINISH(SHA1)(x, y, z)
#define   HASH_APPEND(x, y, z)       _HASH_APPEND(SHA1)(x, y, z) 
#define   CRL_HASH_SIZE              CRL_SHA1_SIZE
#define   HASHctx_stt                SHA1ctx_stt
#elif     ECC_HASH == SHA224
#define   HASH_INIT(x)               _HASH_INIT(SHA224)(x)
#define   HASH_FINISH(x, y, z)       _HASH_FINISH(SHA224)(x, y, z)
#define   HASH_APPEND(x, y, z)       _HASH_APPEND(SHA224)(x, y, z)
#define   CRL_HASH_SIZE              CRL_SHA224_SIZE
#define   HASHctx_stt                SHA224ctx_stt
#elif     ECC_HASH == MD5
#define   HASH_INIT(x)               _HASH_INIT(MD5)(x)
#define   HASH_FINISH(x, y, z)       _HASH_FINISH(MD5)(x, y, z) 
#define   HASH_APPEND(x, y, z)       _HASH_APPEND(MD5)(x, y, z) 
#define   CRL_HASH_SIZE              CRL_MD5_SIZE
#define   HASHctx_stt                MD5ctx_stt
#else 
#error    "Not define ECC_HASH !"
#endif 


//ecc��Ҫ�õ��Ŀռ�
#define          ECC_STORE_SPACE             3584
// hash�㷨������MD5, SHA1, SHA224 or SHA256
#define          ECC_HASH                    SHA256     //����HASH�㷨ΪSHA256  
//ǩ����ʽ
#define          ECC_SIGN_ALGORITHM          ECDSA


//������Բ�����Ľṹ��
//����a��p��n�Ǳ���ģ�GX��GYҲ�Ǳ���ģ�
//ָ����ֵ��Ҫ��ֵΪ��, ����Ϊ0
typedef struct 
{
	uint8_t *p_a;
	uint8_t *p_p;
	uint8_t *p_b;
	uint8_t *p_n;
	uint8_t *p_Gx;
	uint8_t *p_Gy;
	int32_t a_size;
	int32_t b_size;	
	int32_t p_size;
	int32_t n_size;
	int32_t Gx_size;
	int32_t Gy_size;
}EC_Para;


//��Կ�ṹ��
typedef struct 
{
	uint8_t *pub_x;
	uint8_t *pub_y;
	int32_t pub_xSize;
	int32_t pub_ySize;
}Pub_Key_Para;


//˽Կ�ṹ��
typedef struct 
{
	uint8_t *priv;
	int32_t priv_size;
}Priv_Key_Para;


//G��ṹ��
typedef struct 
{
	uint8_t *g_x;
	uint8_t *g_y;
	int32_t g_xSize;
	int32_t g_ySize;
}G_Para;


//ǩ���ṹ��
typedef struct  
{
	uint8_t *sign_r;
	uint8_t *sign_s;
	int32_t sign_rSize;
	int32_t sign_sSize;
}Sign_Para;


//ժҪ�ṹ��
typedef struct  
{
	uint8_t *digt;
	int32_t digest_size;
}Digest_Para;


//������Ϣ�ṹ��
typedef struct  
{
	uint8_t *input_msg;
	int32_t inputMsg_size;
}InputMsg_Para;

uint8_t Alg_ECDSASignVerify(uint8_t *public_key,BIN_FILE_INFO *file,uint8_t *input_msg);

void	EC_paraTestInit(EC_Para *ec, Pub_Key_Para *pub_key, Sign_Para *sign, 
	                    InputMsg_Para *inputMsg, Digest_Para *digest);

//ECDSAǩ����֤
int32_t ECCSignVerify(const EC_Para *ec, const Pub_Key_Para *pub_key, const Sign_Para *sign, 
                      const InputMsg_Para *inputMsg, Digest_Para *digest);


//��ӡ�����Ϣ
void prinfInfo(Sign_Para *sign, Priv_Key_Para *priv_key,Digest_Para *digest);

void ECDSA_SignVerify_TEST(void);

#endif
