#include "ecdsa.h"
#include <string.h>

uint8_t pri_key_malloc[200];
uint8_t digest_malloc[128];
uint8_t pub_x_malloc[50];
uint8_t pub_y_malloc[50];
uint8_t sign_r_malloc[50];
uint8_t sign_s_malloc[50];
uint8_t preallocated_buffer[ECC_STORE_SPACE];

/******************************************************************************/
/******** Parameters for Elliptic Curve P-256 SHA-256 from FIPS 186-3**********/
/******************************************************************************/
const uint8_t P_256_a[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC
  };
const uint8_t P_256_b[] =
  {
    0x5a, 0xc6, 0x35, 0xd8, 0xaa, 0x3a, 0x93, 0xe7, 0xb3, 0xeb, 0xbd, 0x55, 0x76,
    0x98, 0x86, 0xbc, 0x65, 0x1d, 0x06, 0xb0, 0xcc, 0x53, 0xb0, 0xf6, 0x3b, 0xce,
    0x3c, 0x3e, 0x27, 0xd2, 0x60, 0x4b
  };
const uint8_t P_256_p[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
  };
const uint8_t P_256_n[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xBC, 0xE6, 0xFA, 0xAD, 0xA7, 0x17, 0x9E, 0x84, 0xF3, 0xB9,
    0xCA, 0xC2, 0xFC, 0x63, 0x25, 0x51
  };
const uint8_t P_256_Gx[] =
  {
    0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47, 0xF8, 0xBC, 0xE6, 0xE5, 0x63,
    0xA4, 0x40, 0xF2, 0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB, 0x33, 0xA0, 0xF4, 0xA1,
    0x39, 0x45, 0xD8, 0x98, 0xC2, 0x96
  };
const uint8_t P_256_Gy[] =
  {
    0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F, 0x9B, 0x8E, 0xE7, 0xEB, 0x4A, 0x7C,
    0x0F, 0x9E, 0x16, 0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31, 0x5E, 0xCE, 0xCB, 0xB6,
    0x40, 0x68, 0x37, 0xBF, 0x51, 0xF5
  };
	
void fun_print(uint8_t *str)
{
		uint8_t i=0;
		while(i<32)
		{	
				i++;
				printf("%x ", *str++);
		}
}

void prinfInfo(Sign_Para *sign, Priv_Key_Para *priv_key,Digest_Para *digest)
{
	  printf("priv key:	size:%d\r\n",priv_key->priv_size);
		fun_print(priv_key->priv);	
		printf("\r\n");	
	
		printf("sign:		r_size:%d		s_size:%d\r\n",sign->sign_rSize,sign->sign_sSize);
		printf("r:");
		fun_print(sign->sign_r);	
		printf("\r\n");
		printf("s:");
		fun_print(sign->sign_s);	
		printf("\r\n");		
	
	  printf("digest:		digest_size:%d\r\n",digest->digest_size);
		fun_print(digest->digt);	
		printf("\r\n");	
}	

/******************************************************************
* 函数名：EC_ParaInit
* 函数说明：EC初始化
* 输入参数： ec                   椭圆参数'
*            pub_key              公钥参数         in
* 输出参数：无
*******************************************************************/	
void	EC_ParaInit(EC_Para *ec, Pub_Key_Para *pub_key, Sign_Para *sign)
{
		ec->p_a = (uint8_t *)P_256_a;
    ec->p_b = (uint8_t *)P_256_b;
    ec->p_p = (uint8_t *)P_256_p;
    ec->p_n = (uint8_t *)P_256_n;
    ec->p_Gx = (uint8_t *)P_256_Gx;
    ec->p_Gy = (uint8_t *)P_256_Gy;
    ec->a_size = sizeof(P_256_a);
    ec->b_size = sizeof(P_256_b);
		ec->n_size = sizeof(P_256_n);
		ec->p_size = sizeof(P_256_p);
		ec->Gx_size = sizeof(P_256_Gx);
		ec->Gy_size = sizeof(P_256_Gy);
	
	
		pub_key->pub_x = (uint8_t *)pub_x_malloc;
		pub_key->pub_y = (uint8_t *)pub_y_malloc;		
		sign->sign_r = (uint8_t *)sign_r_malloc;
    sign->sign_s = (uint8_t *)sign_s_malloc;
}

/******************************************************************
* 函数名：HASH_DigestCompute
* 函数说明：初始化RNG引擎
* 输入参数：InputMessage         指向需要进行HASH计算的的输入消息指针
*          InputMessageLength   输入消息长度
*          MessageDigest        指向输出参数，它将处理消息摘要
*          MessageDigestLength  输出摘要长度
* 输出参数：错误码
*******************************************************************/		
static int32_t RNG_init_for_sign(RNGstate_stt *P_pRNGstat)
{
		RNGinitInput_stt RNGinit_st;

	  int32_t error_sta = RNG_ERR_UNINIT_STATE;
	
		uint8_t entropy_data[32] =
    {
      0x9d, 0x20, 0x1a, 0x18, 0x9b, 0x6d, 0x1a, 0xa7, 0x0e, 0x79, 0x57, 0x6f, 0x36,
      0xb6, 0xaa, 0x88, 0x55, 0xfd, 0x4a, 0x7f, 0x97, 0xe9, 0x71, 0x69, 0xb6, 0x60,
      0x88, 0x78, 0xe1, 0x9c, 0x8b, 0xa5
    };
		
		/* 随机数 */
		uint8_t nonce[4] = {0, 1, 2, 3};
			
		//Random number generation (RNG) 初始化随机数引擎
		RNGinit_st.pmEntropyData = entropy_data;
		RNGinit_st.mEntropyDataSize = sizeof(entropy_data);
		RNGinit_st.pmNonce = nonce;
		RNGinit_st.mNonceSize = sizeof(nonce);	
		RNGinit_st.mPersDataSize = 0;
		RNGinit_st.pmPersData = NULL;
		
		error_sta = RNGinit((const RNGinitInput_stt *)&RNGinit_st,C_DRBG_AES128, P_pRNGstat);
		if (error_sta != RNG_SUCCESS)
		{
			return error_sta;
		} 
		
		return error_sta;
}	

/******************************************************************
* 函数名：HASH_DigestCompute
* 函数说明：ecc的哈希计算
* 输入参数：InputMessage         指向需要进行HASH计算的的输入消息指针
*          InputMessageLength   输入消息长度
*          MessageDigest        指向输出参数，它将处理消息摘要
*          MessageDigestLength  输出摘要长度
* 输出参数：错误码
*******************************************************************/			
static int32_t HASH_DigestCompute(const uint8_t* InputMessage, uint32_t InputMessageLength,
                           uint8_t *MessageDigest, int32_t* MessageDigestLength)
{
		HASHctx_stt           HASHctx;        
		int32_t error_sta  =  HASH_SUCCESS;

		HASHctx.mFlags = E_HASH_DEFAULT;
		HASHctx.mTagSize = CRL_HASH_SIZE;   //设置摘要大小 1~CRL_HASH_SIZE的范围内
	
		//HASH初始化   
		error_sta = HASH_INIT(&HASHctx);
		if (error_sta != HASH_SUCCESS)
		{
			return error_sta;
		}
		
		//处理输入数据并将更新的哈希算法结构体
		error_sta = HASH_APPEND(&HASHctx, InputMessage, InputMessageLength);
		if (error_sta != HASH_SUCCESS)
		{
			return error_sta;
		}
		
		//Hash算法完成函数,产生HASH算法输出
		error_sta = HASH_FINISH(&HASHctx, MessageDigest, MessageDigestLength);
		if (error_sta != HASH_SUCCESS)
		{	
			return error_sta;
		}

		return error_sta;
}

/******************************************************************
* 函数名：ECCSignGenerate
* 函数说明：ecdsa的生成Key pair 以及签名
* 输入参数： ec                   椭圆参数     in
*           inputMsg             输入数据     in
*           digest               摘要参数     out
*           pub_key              公钥参数     out
*						priv_key             私钥参数     out
*           sign                 签名参数     out 
* 输出参数：错误码 
*          没有错误就为成功
*---------------------------------------------------------
* 调试说明：ECDSA已通过P-192认证
*******************************************************************/
int32_t ECCSignGenerate(const EC_Para *ec,const InputMsg_Para *inputMsg, Digest_Para *digest, 
 		                      Pub_Key_Para *pub_key, Priv_Key_Para *priv_key, Sign_Para *signature,uint8_t *PriveKey)
{
		int32_t error_sta  = ECC_SUCCESS;
		RNGstate_stt         RNGstate;
		const	uint8_t * InputMessage;
		uint32_t InputLength = NULL;
		EC_stt                ECctx;
		membuf_stt            ECCMem_ctx;        //这个地方有疑惑，这个是否需要长期保存
		ECpoint_stt *PubKey = NULL;
		ECCprivKey_stt *PrivKey = NULL;
		ECDSAsignature_stt *sign = NULL;
		ECDSAsignCtx_stt signCtx;
	
		//HASH初始化 
		InputMessage =  inputMsg->input_msg;
		InputLength = inputMsg->inputMsg_size;	
		error_sta = HASH_DigestCompute(InputMessage, InputLength, 
			        digest->digt, &digest->digest_size);                    
		if (error_sta != HASH_SUCCESS)   
			return  error_sta;
	
		//初始随机数引擎
		error_sta = RNG_init_for_sign(&RNGstate);
		if (error_sta != ECC_SUCCESS)
    {
			return RNG_SUCCESS;
		}
		
	  ECCMem_ctx.pmBuf =  preallocated_buffer;
		ECCMem_ctx.mUsed = 0;
		ECCMem_ctx.mSize = sizeof(preallocated_buffer);	
					
		ECctx.pmA = ec->p_a;
    ECctx.pmB = ec->p_b;
    ECctx.pmP = ec->p_p;
    ECctx.pmN =  ec->p_n;
    ECctx.pmGx = ec->p_Gx;
    ECctx.pmGy =  ec->p_Gy;
    ECctx.mAsize = ec->a_size;
    ECctx.mBsize = ec->b_size;
		ECctx.mNsize = ec->n_size;
		ECctx.mPsize = ec->p_size;
		ECctx.mGxsize = ec->Gx_size;
		ECctx.mGysize = ec->Gy_size;
		error_sta = ECCinitEC(&ECctx, &ECCMem_ctx);
		if (error_sta != ECC_SUCCESS)
		{ 
			goto ECCKeyGen_L1;
		}
				
 		//初始化私钥
 		error_sta = ECCinitPrivKey(&PrivKey, &ECctx, &ECCMem_ctx);
		if (error_sta != ECC_SUCCESS)
		{ 
			goto ECCKeyGen_L3;
		}		

		//设置私钥的值
 		error_sta = ECCsetPrivKeyValue(PrivKey, PriveKey,32);
		if (error_sta != ECC_SUCCESS)
		{ 
				goto ECCKeyGen_L4;
		}				
				
		 /* 初始化ECDSA 签名结构体*/
     error_sta = ECDSAinitSign(&sign, &ECctx, &ECCMem_ctx);
		if (error_sta != ECC_SUCCESS)
		{ 
			goto ECCKeyGen_L4;
		}		
		
		 /* 生成签名 */
		signCtx.pmEC = &ECctx;
		signCtx.pmPrivKey = PrivKey;
		signCtx.pmRNG = &RNGstate;
		error_sta = ECDSAsign(digest->digt, digest->digest_size, sign, &signCtx, &ECCMem_ctx);
		if (error_sta != ECC_SUCCESS)
		{ 
			goto ECCKeyGen_L5;
		}
		
		/* 导出信息 */			
//		error_sta = ECCgetPrivKeyValue(PrivKey, priv_key->priv, &priv_key->priv_size);
	
		error_sta = ECDSAgetSignature(sign, E_ECDSA_SIGNATURE_R_VALUE, signature->sign_r, &signature->sign_rSize);
		error_sta |= ECDSAgetSignature(sign, E_ECDSA_SIGNATURE_S_VALUE, signature->sign_s, &signature->sign_sSize);
		
		
ECCKeyGen_L5:		
		ECDSAfreeSign(&sign, &ECCMem_ctx);
		
ECCKeyGen_L4:
		ECCfreePrivKey(&PrivKey, &ECCMem_ctx);

ECCKeyGen_L3:
 		ECCfreePoint(&PubKey, &ECCMem_ctx);
		 		
ECCKeyGen_L1:		
 		RNGfree(&RNGstate);
		
		return error_sta;
}

/******************************************************************
*	函数名：	 ECCSign
*	函数说明：对输入的数据进行签名
* 输入参数：str					要签名的数组
					 len					签名数组的长度
* 输出参数：return_value 返回值，为0表示成功，其它为错误码
*******************************************************************/	
int32_t Alg_ECDSASignData(uint8_t *dataIn,int dataInLen,uint8_t *dataOut,int *dataoutLen,uint8_t *PriveKEY)
{
		int32_t	return_value=0;
		uint8_t i;
	  EC_Para EC;
	  Pub_Key_Para pub_key;
	  Sign_Para sign;
	  InputMsg_Para inputMsg;
	  Digest_Para digest;
	  Priv_Key_Para priv_key;
	
		Crypto_DeInit();
		EC_ParaInit(&EC, &pub_key, &sign);
	  inputMsg.input_msg = (uint8_t *)dataIn;
	  inputMsg.inputMsg_size = dataInLen;
	 
	  digest.digt = digest_malloc;
	  priv_key.priv = pri_key_malloc;
    return_value = ECCSignGenerate(&EC, &inputMsg, &digest, &pub_key, &priv_key, &sign,PriveKEY);   //生成

		*dataoutLen = sign.sign_rSize + sign.sign_sSize;
		for(i=0;i<(*dataoutLen)/2;i++)
		{
				dataOut[i] = *(sign.sign_r+i);
				dataOut[i+sign.sign_rSize] = *(sign.sign_s+i);
		}
		return return_value;
}

void Test_Ecc_Sign_Data(void)
{
		uint8_t dataIn[] =
		{
				0x80,0x00,0x00,0x01,0x87,0xb4,0xd0,0x8a,0x2b,0x9b,0xf6,0x12,0x9b,0x80,0xee,0xdd,
				0x6d,0x4b,0xec,0x43,0x33,0x67,0x9e,0x0f,0x0a,0x4d,0x8f,0x10,0x86,0x54,0x7c,0x40,
				0x31,0xc6,0x95,0x77,0x00,0x00,0x01,0x9b,0x7c,0xff,0xda,0xa6,0x74,0xbe,0xae,0x0f,
				0x93,0x0e,0xbe,0x60,0x85,0xaf,0x90,0x93,0xe5,0xfe,0x56,0xb3,0x4a,0x5c,0x22,0x0c,
				0xcd,0xcf,0x6e,0xfc,0x33,0x6f,0xc5,0x00,0xe1,0xf5,0x05,0x00,0x00,0x00,0x00,0x35,
				0xb2,0x00,0x10,0xdb,0x73,0xbf,0x86,0x37,0x10,0x75,0xdd,0xfb,0xa4,0xe6,0x59,0x6f,
				0x1f,0xf3,0x5d,		
		};
		uint8_t PriveKEY[32] =
		{
				0x35,0x67,0x3e,0xcf,0x9f,0x18,0xf4,0x4e,0xbd,0x1c,0x9f,0xc7,0xca,0xc4,0x32,0x7d,
				0x72,0xee,0xb0,0x81,0x04,0xef,0xfd,0x20,0xfb,0xbb,0x62,0x13,0x41,0xb8,0x0a,0xc1
		};
		
	  EC_Para 			EC;
	  Pub_Key_Para 	pub_key;
	  Sign_Para 		sign;
	  InputMsg_Para inputMsg;
	  Digest_Para 	digest;
	  Priv_Key_Para priv_key;
	
		Crypto_DeInit();
		EC_ParaInit(&EC, &pub_key, &sign);
	  inputMsg.input_msg 		 = dataIn;
	  inputMsg.inputMsg_size = sizeof(dataIn);
	 
	  digest.digt 	= digest_malloc;
	  priv_key.priv = pri_key_malloc;
    ECCSignGenerate(&EC, &inputMsg, &digest, &pub_key, &priv_key, &sign,PriveKEY);
}

/******************************************************************
*	函数名：	 HashData
*	函数说明：对输入数组进行哈希计算
* 输入参数：dataIn			要计算的数组
					 dataInLen	计算数组的长度
* 输出参数：dataOut		输出数据的数组
					 dataoutLen 输出数组的长度
					 value      返回值，为0表示成功，其它为错误码
*******************************************************************/
int32_t Alg_HashData(uint8_t *dataIn,int dataInLen,uint8_t *dataOut,int *dataoutLen)
{
		int32_t value;		
	
		Crypto_DeInit();	
		value = HASH_DigestCompute(dataIn,dataInLen,dataOut,dataoutLen);
	
		return value;
}
/******************************************************************
*	函数名：	 ECCGetPublickeyFromPrivKey
*	函数说明：从输入的私钥计算对应公钥
* 输入参数：PrivKey		输入的私钥
					 Publickey	输出的公钥
* 输出参数：value 			返回值，为0表示成功，其它为错误码
*******************************************************************/	
int32_t ECCGetPublickeyFromPrivateKey_OLD(uint8_t *PrivateKey,uint8_t *Publickey)
{
		int32_t value = ECC_SUCCESS;
		int i;
		//相关参数
		RNGstate_stt RNGstate1;	
		Pub_Key_Para pub_key1;
		EC_stt ECctx1;	
		membuf_stt ECCMem_ctx1;  		
		ECpoint_stt *PubKey1 = NULL;
		ECCprivKey_stt *PrivKey1 = NULL;
		ECpoint_stt *G1 = NULL;
	
		memset(pub_x_malloc,0,50);
		memset(pub_y_malloc,0,50);
	
		Crypto_DeInit();	
	
		pub_key1.pub_x = (uint8_t *)pub_x_malloc;
		pub_key1.pub_y = (uint8_t *)pub_y_malloc;	
		
		//初始随机数引擎
		value = RNG_init_for_sign(&RNGstate1);
		if (value != ECC_SUCCESS)
    {
			return RNG_SUCCESS;
		}			
		
		//设置相关参数
	  ECCMem_ctx1.pmBuf =  preallocated_buffer;
		ECCMem_ctx1.mUsed = 0;
		ECCMem_ctx1.mSize = sizeof(preallocated_buffer);	
					
		ECctx1.pmA = P_256_a;
    ECctx1.pmB = P_256_b;
    ECctx1.pmP = P_256_p;
    ECctx1.pmN = P_256_n;
    ECctx1.pmGx = P_256_Gx;
    ECctx1.pmGy = P_256_Gy;
    ECctx1.mAsize = sizeof(P_256_a);
    ECctx1.mBsize = sizeof(P_256_b);
		ECctx1.mNsize = sizeof(P_256_n);
		ECctx1.mPsize = sizeof(P_256_p);
		ECctx1.mGxsize = sizeof(P_256_Gx);
		ECctx1.mGysize = sizeof(P_256_Gy);
				
		//初始化参数
		value = ECCinitEC(&ECctx1, &ECCMem_ctx1);	
		if (value != ECC_SUCCESS)
		{ 
				goto ECCKeyGenPub_L1;
		}
		
		//初始化G点
		value = ECCinitPoint(&G1,  &ECctx1, &ECCMem_ctx1);	
		if (value != 0)
		{
				goto ECCKeyGenPub_L2;
		}
		
		//设置G点的坐标
		value = ECCsetPointGenerator(G1, &ECctx1);
		if (value != 0)
		{
				goto ECCKeyGenPub_L2;
		}	
		
 		//初始化公钥
 		value = ECCinitPoint(&PubKey1, &ECctx1, &ECCMem_ctx1);		
		if (value != ECC_SUCCESS)
		{ 
			goto ECCKeyGenPub_L2;
		}	
		
 		//初始化私钥
 		value = ECCinitPrivKey(&PrivKey1, &ECctx1, &ECCMem_ctx1);		
		if (value != ECC_SUCCESS)
		{ 
			goto ECCKeyGenPub_L3;
		}	
		
		//设置私钥的值
 		value = ECCsetPrivKeyValue(PrivKey1, PrivateKey,32);		
		if (value != ECC_SUCCESS)
		{ 
				goto ECCKeyGenPub_L4;
		}			
		
		//加载参数，并计算公钥
		value = ECCscalarMul(G1, PrivKey1, PubKey1, &ECctx1, &ECCMem_ctx1);		
		if (value != ECC_SUCCESS)
		{ 
				goto ECCKeyGenPub_L4;
		}
		
		value = ECCvalidatePubKey(PubKey1,&ECctx1,&ECCMem_ctx1);
		
		/* 导出信息 */	
		value = ECCgetPointCoordinate(PubKey1, E_ECC_POINT_COORDINATE_X, pub_key1.pub_x, &pub_key1.pub_xSize);
		value |= ECCgetPointCoordinate(PubKey1, E_ECC_POINT_COORDINATE_Y, pub_key1.pub_y, &pub_key1.pub_ySize);
	
		
		//将公钥拷贝出来
		if(value == 0)
		{
				for(i=0;i<32;i++)
				{
						Publickey[i] = *(pub_key1.pub_x + i);
				}
				for(i=0;i<32;i++)
				{		
						Publickey[i+32] = *(pub_key1.pub_y+i);
				}
		}		
		
		
ECCKeyGenPub_L4:
		ECCfreePrivKey(&PrivKey1, &ECCMem_ctx1);

ECCKeyGenPub_L3:
 		ECCfreePoint(&PubKey1, &ECCMem_ctx1);
		
ECCKeyGenPub_L2:
 		ECCfreeEC(&ECctx1, &ECCMem_ctx1);

ECCKeyGenPub_L1:
 		RNGfree(&RNGstate1);
		
		return value;
}

/******************************************************************
*	函数名：	 ECCGetPublickeyFromPrivKey
*	函数说明：从输入的私钥计算对应公钥
* 输入参数：PrivKey		输入的私钥
						state			要计算的公钥组成形式
* 输出参数：Publickey	输出的公钥
					 value 			返回值，为0表示成功，其它为错误码
*******************************************************************/	
int32_t Alg_GetPublicFromPrivate(uint8_t *PrivateKey,uint8_t Publickey[65],uint8_t state)
{
		int32_t value = ECCGetPublickeyFromPrivateKey_OLD(PrivateKey,Publickey+1);
		if(state)
		{
				if(Publickey[64]%2)
						Publickey[0] = 3;
				else
						Publickey[0] = 2;
		}
		else
		{
				Publickey[0] = 4;
		}
		return value;
}





