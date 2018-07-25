#include "app_ecdsa.h"
#include <string.h> 

//分配存储空间
uint8_t digest_malloc[128];
uint8_t pub_x_malloc[50];
uint8_t pub_y_malloc[50];
uint8_t sign_r_malloc[50];
uint8_t sign_s_malloc[50];

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

uint8_t Alg_ECDSASignVerify(uint8_t *public_key,BIN_FILE_INFO *file,uint8_t *input_msg)
{
		int32_t error_sta = 0;
	  EC_Para EC;
	  Pub_Key_Para pub_key;
	  Sign_Para sign;
	  Digest_Para digest;
	  InputMsg_Para inputMsg;
	
		Crypto_DeInit();	
		EC_paraTestInit(&EC, &pub_key, &sign, &inputMsg, &digest);

		pub_key.pub_xSize = 32;
		pub_key.pub_ySize = 32;
		pub_key.pub_x 		= public_key;
		pub_key.pub_y 		= public_key + 32;

		sign.sign_rSize 	= file->Len_sign/2;
		sign.sign_sSize 	= file->Len_sign/2;
		sign.sign_r 			= file->signature;
		sign.sign_s 			= file->signature + file->Len_sign/2;
	
		inputMsg.inputMsg_size = 32;
		inputMsg.input_msg 		 = input_msg;
			
		digest.digt = digest_malloc;
		error_sta 	= ECCSignVerify(&EC, &pub_key, &sign, &inputMsg, &digest);
	
		if (error_sta == AUTHENTICATION_SUCCESSFUL)
		{
				return 1;
		}
		else
		{		
				return 0;
		}
}

/******************************************************************
* 函数名：EC_paraTestInit
* 函数说明：EC初始化测试
* 输入参数： ec                   椭圆参数'
*            pub_key              公钥参数         in
* 输出参数：无
*******************************************************************/

void	EC_paraTestInit(EC_Para *ec, Pub_Key_Para *pub_key, Sign_Para *sign, 
	                    InputMsg_Para *inputMsg, Digest_Para *digest)
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

void fun_print(uint8_t *str)
{
		uint8_t i = 0;
		while(i < 32)
		{	
				i++;
				printf("0x%x,", *str++);
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
* 函数名：ECCSignVerify
* 函数说明：ECDSA签名认证
* 输入参数： ec                   椭圆参数          in
*           pub_key              公钥参数          in
*           sign                 签名参数          in
*           inputMsg             输入数据          in
*           digest               摘要参数          out
* 输出参数：错误码     
*          如果都正确返回  AUTHENTICATION_SUCCESSFUL  1003
*---------------------------------------------------------
* 调试说明：ECDSA已通过P-192认证
*******************************************************************/
int32_t ECCSignVerify(const EC_Para *ec, const Pub_Key_Para *pub_key, const Sign_Para *sign, 
                      const InputMsg_Para *inputMsg, Digest_Para *digest)
{
		int32_t error_sta  = ECC_SUCCESS;
		uint8_t preallocated_buffer[ECC_STORE_SPACE];
		const	uint8_t * InputMessage;
		uint32_t InputLength = NULL;

		EC_stt                ECctx;
		membuf_stt            ECDSAMem_ctx;
	
		ECpoint_stt *PubKey = NULL; 

		ECDSAsignature_stt *  ECDSAsignature_ctx = NULL;
		
		ECDSAverifyCtx_stt verctx;

	  const uint8_t * pub_x;
		const uint8_t * pub_y;
	  const uint8_t * sign_r;
		const uint8_t * sign_s;
		int32_t pub_x_size = 0x00;
		int32_t pub_y_size = 0x00;
		int32_t signRsize = 0x00;
		int32_t signSsize = 0x00;
		
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
		
		pub_x = pub_key->pub_x;
    pub_y = pub_key->pub_y;		
		sign_r = sign->sign_r;
    sign_s = sign->sign_s;
    pub_x_size = pub_key->pub_xSize;
    pub_y_size = pub_key->pub_ySize;
		signRsize = sign->sign_rSize;
    signSsize = sign->sign_sSize;
		InputMessage =  inputMsg->input_msg;
		InputLength = inputMsg->inputMsg_size;

	  //HASH初始化  
		error_sta = HASH_DigestCompute(InputMessage, InputLength, 
			        digest->digt, &digest->digest_size);                    
		if (error_sta != HASH_SUCCESS)   
			return  error_sta;
	
		ECDSAMem_ctx.mSize = sizeof(preallocated_buffer);
    ECDSAMem_ctx.mUsed = 0;
    ECDSAMem_ctx.pmBuf = preallocated_buffer;
		
		//ecc初始化                                        
		error_sta = ECCinitEC(&ECctx, &ECDSAMem_ctx);
		if (error_sta != ECC_SUCCESS)
		{ 
			return error_sta;
		}
		
		//初始化ECC点
 		error_sta =ECCinitPoint(&PubKey, &ECctx, &ECDSAMem_ctx);
		if (error_sta != ECC_SUCCESS)
		{ 
			goto ECCVerify_L1;
		}
		
		//设置值的ECC点的坐标之一  初始化点，导入公钥
		ECCsetPointCoordinate(PubKey, E_ECC_POINT_COORDINATE_X, pub_x, pub_x_size);
    ECCsetPointCoordinate(PubKey, E_ECC_POINT_COORDINATE_Y, pub_y, pub_y_size);
	
		//验证
		error_sta = ECCvalidatePubKey(PubKey, &ECctx, &ECDSAMem_ctx);
		if (error_sta != ECC_SUCCESS)
		{ 
			goto ECCVerify_L2;
		}
 		
		//初始化签名结构体
		error_sta = ECDSAinitSign(&ECDSAsignature_ctx, &ECctx, &ECDSAMem_ctx);
		if (error_sta != ECC_SUCCESS)
		{ 
			goto ECCVerify_L2;
		}
		
		//导入签名    
		error_sta = ECDSAsetSignature(ECDSAsignature_ctx, E_ECDSA_SIGNATURE_R_VALUE, sign_r, signRsize);
		error_sta |= ECDSAsetSignature(ECDSAsignature_ctx, E_ECDSA_SIGNATURE_S_VALUE, sign_s, signSsize);
		if (error_sta != ECC_SUCCESS)
		{ 
			goto ECCVerify_L3;
		}
		
		 /*编写结构对椭圆曲线数字签名验证*/
		verctx.pmEC = &ECctx;
		verctx.pmPubKey = PubKey;
		error_sta = ECDSAverify(digest->digt, digest->digest_size, ECDSAsignature_ctx, &verctx, &ECDSAMem_ctx);
		
ECCVerify_L3:		
		ECDSAfreeSign(&ECDSAsignature_ctx, &ECDSAMem_ctx);
		
ECCVerify_L2:
		ECCfreePoint(&PubKey, &ECDSAMem_ctx);

ECCVerify_L1:		
		ECCfreeEC(&ECctx, &ECDSAMem_ctx);
				
		return error_sta;
}
 
void ECDSA_SignVerify_TEST(void)
{
		uint8_t hash_file[32] = 
		{
				0x5a,0x56,0xaa,0x70,0x06,0x0a,0x66,0x69,0x80,0xeb,0x4f,0x10,0xb1,0xee,0xc4,0xa8,
				0xfc,0xf0,0x40,0x27,0xbf,0x71,0x5c,0x96,0x12,0x60,0x3a,0x42,0x96,0x5e,0xd0,0x77
		};

		uint8_t public_xkey[32] = 
		{
				220,114,233,198,133,184, 81,202,202, 10, 23,234, 25,144, 45,196,
				 80, 41,188,166, 57,158,131,237,241,  7,175, 52, 57,203,166, 48
		};

		uint8_t public_ykey[32] = 
		{
				 77,129,140,102, 91, 55,100, 75, 22,199,102,163,254, 39,208,  8,
				242,115,116, 26,250, 72,  8,254, 61,123,213,224,137,112,238, 69
		};

		uint8_t signature_r1[32] = 
		{
				196, 53,233, 73,164,160,108,221, 47, 50, 57, 72,118,  7,105,220,
				188, 67, 73,200,171,100,102,  9,250,242, 82,103,225, 78,188,246
		};

		uint8_t signature_s1[32] = 
		{
				169,200, 61,233,144,140, 35,111, 24, 83, 88, 77,138,214,218,221,
				112,156, 46, 92,252,201,139,185,106, 88, 43,251,235,176,184,160
		};
		
		//uint8_t signature_r2[32] = 
		//{		
		//		0xd8,0x47,0xc9,0x0a,0xab,0xc9,0xb0,0xff,0x69,0x6f,0x07,0x00,0xae,0x90,0xac,0xc4,
		//		0x66,0x47,0x34,0x41,0xfa,0x59,0xaf,0x37,0xf9,0x44,0x08,0xe8,0x02,0xcf,0x68,0x37
		//};

		//uint8_t signature_s2[32] = 	
		//{
		//		0x25,0x8b,0x00,0xb6,0xbe,0x82,0x45,0x07,0x27,0xd0,0xb8,0x8a,0x26,0xf9,0x82,0x5c,
		//		0x4b,0x30,0x3b,0x17,0xe2,0x49,0x74,0x67,0x38,0x6c,0xdc,0x83,0x19,0xcb,0x71,0x7c
		//};
		
		int32_t error_sta = 0;
		//相关参数
	  EC_Para EC;
	  Pub_Key_Para pub_key;
	  Sign_Para sign;
	  Digest_Para digest;
	  InputMsg_Para inputMsg;
	
		Crypto_DeInit();	
		EC_paraTestInit(&EC, &pub_key, &sign, &inputMsg, &digest);

		pub_key.pub_xSize = 32;
		pub_key.pub_ySize = 32;
		pub_key.pub_x = public_xkey;
		pub_key.pub_y = public_ykey;

		sign.sign_rSize = 32;
		sign.sign_sSize = 32;
		sign.sign_r = signature_r1;
		sign.sign_s = signature_s1;
	
		inputMsg.inputMsg_size = 32;
		inputMsg.input_msg = hash_file;
			
		digest.digt = digest_malloc;
		error_sta = ECCSignVerify(&EC, &pub_key, &sign, &inputMsg, &digest);
#ifdef Debug_Print			
		printf("error_sta = %d\n",error_sta);
#endif	
		if (error_sta == AUTHENTICATION_SUCCESSFUL)  //认证
		{
#ifdef Debug_Print				
				//认证成功
				printf("verify successful!\r\n");
#endif
		}
		else
		{
#ifdef Debug_Print				
				//认证失败
				printf("verify fault!\r\n");				
#endif
		}
}

