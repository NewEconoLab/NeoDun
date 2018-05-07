#include "app_aes.h"
#include "crypto.h"
#include <stdio.h>
#include <string.h>

// AES 使用的密码，可以考虑通过SHA256得来


int32_t STM32_AES_ECB_Encrypt(uint8_t* InputMessage,
                        uint32_t InputMessageLength,
                        uint8_t  *AES256_Key,
                        uint8_t  *OutputMessage,
                        uint32_t *OutputMessageLength)
{
		AESECBctx_stt AESctx;

		uint32_t error_status = AES_SUCCESS;

		int32_t outputLength = 0;

		/* Set flag field to default value */
		AESctx.mFlags = E_SK_DEFAULT;

		/* Set key size to 32 (corresponding to AES-256) */
		AESctx.mKeySize = 32;

		/* Initialize the operation, by passing the key.
		 * Third parameter is NULL because ECB doesn't use any IV */
		error_status = AES_ECB_Encrypt_Init(&AESctx, AES256_Key, NULL );

		/* check for initialization errors */
		if (error_status == AES_SUCCESS)
		{
				/* Encrypt Data */
				error_status = AES_ECB_Encrypt_Append(&AESctx,
																							InputMessage,
																							InputMessageLength,
																							OutputMessage,
																							&outputLength);

				if (error_status == AES_SUCCESS)
				{
						/* Write the number of data written*/
						*OutputMessageLength = outputLength;
						/* Do the Finalization */
						error_status = AES_ECB_Encrypt_Finish(&AESctx, OutputMessage + *OutputMessageLength, &outputLength);
						/* Add data written to the information to be returned */
						*OutputMessageLength += outputLength;
				}
		}

		return error_status;
}

int32_t STM32_AES_ECB_Decrypt(uint8_t* InputMessage,
                        uint32_t InputMessageLength,
                        uint8_t *AES256_Key,
                        uint8_t  *OutputMessage,
                        uint32_t *OutputMessageLength)
{
		AESECBctx_stt AESctx;

		uint32_t error_status = AES_SUCCESS;

		int32_t outputLength = 0;

		/* Set flag field to default value */
		AESctx.mFlags = E_SK_DEFAULT;

		/* Set key size to 32 (corresponding to AES-256) */
		AESctx.mKeySize = 32;

		/* Initialize the operation, by passing the key.
		 * Third parameter is NULL because ECB doesn't use any IV */
		error_status = AES_ECB_Decrypt_Init(&AESctx, AES256_Key, NULL );

		/* check for initialization errors */
		if (error_status == AES_SUCCESS)
		{
				/* Decrypt Data */
				error_status = AES_ECB_Decrypt_Append(&AESctx,
																							InputMessage,
																							InputMessageLength,
																							OutputMessage,
																							&outputLength);

				if (error_status == AES_SUCCESS)
				{
						/* Write the number of data written*/
						*OutputMessageLength = outputLength;
						/* Do the Finalization */
						error_status = AES_ECB_Decrypt_Finish(&AESctx, OutputMessage + *OutputMessageLength, &outputLength);
						/* Add data written to the information to be returned */
						*OutputMessageLength += outputLength;
				}
		}

		return error_status;
}

//void Test_AES(void)
//{
//		const uint8_t Plaintext[64] =
//		{
//				0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
//				0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
//				0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
//				0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
//				0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
//				0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
//				0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
//				0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
//		};
//		uint8_t Key[CRL_AES256_KEY] =
//		{
//				0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
//				0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
//				0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
//				0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4,
//		};		
//		const uint8_t Expected_Ciphertext[64] =
//		{
//				0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c,
//				0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8,
//				0x59, 0x1c, 0xcb, 0x10, 0xd4, 0x10, 0xed, 0x26,
//				0xdc, 0x5b, 0xa7, 0x4a, 0x31, 0x36, 0x28, 0x70,
//				0xb6, 0xed, 0x21, 0xb9, 0x9c, 0xa6, 0xf4, 0xf9,
//				0xf1, 0x53, 0xe7, 0xb1, 0xbe, 0xaf, 0xed, 0x1d,
//				0x23, 0x30, 0x4b, 0x7a, 0x39, 0xf9, 0xf3, 0xff,
//				0x06, 0x7d, 0x8d, 0x8f, 0x9e, 0x24, 0xec, 0xc7
//		};
//		int32_t status = AES_SUCCESS;
//		uint8_t OutputMessage[64];
//		uint32_t OutputMessageLength = 0;
//		int i = 0;
//		
//		status = STM32_AES_ECB_Encrypt( (uint8_t *) Plaintext, 64, Key, OutputMessage,
//                            &OutputMessageLength);
//		if (status == AES_SUCCESS)
//		{
//				printf("AES_Encrypt:\r\n");
//				for(i=0;i<64;i++)
//						printf("0x%x,",OutputMessage[i]);
//				printf("\r\n");		
//		}
//		else
//		{
//				printf("AES Encrypt Failed!!!\r\n");
//		}
//		
//		memset(OutputMessage,0,64);
//		OutputMessageLength = 0;
//		status = STM32_AES_ECB_Decrypt( (uint8_t *) Expected_Ciphertext, 64, Key, OutputMessage,
//                            &OutputMessageLength);		
//		if (status == AES_SUCCESS)
//		{
//				printf("AES_Decrypt:\r\n");
//				for(i=0;i<64;i++)
//						printf("0x%x,",OutputMessage[i]);
//				printf("\r\n");		
//		}
//		else
//		{
//				printf("AES Decrypt Failed!!!\r\n");
//		}		
//}



