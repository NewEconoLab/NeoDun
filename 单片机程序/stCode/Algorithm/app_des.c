#include "app_des.h"
#include "crypto.h"
#include <stdio.h>
#include <string.h>

int32_t My_DES_Encrypt(uint8_t* InputMessage,
													uint32_t  InputMessageLength,
													uint8_t  *DES_Key,
													uint8_t  *OutputMessage,
													uint32_t *OutputMessageLength)
{
		int32_t status = DES_SUCCESS;
		uint8_t IV[CRL_DES_BLOCK] = 
		{
				0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef
		};		
		
		status = STM32_DES_CBC_Encrypt(InputMessage, InputMessageLength, 
																	 DES_Key,IV, CRL_DES_BLOCK, 
																	 OutputMessage,OutputMessageLength);
		return status;
}

int32_t My_DES_Decrypt(uint8_t*  InputMessage,
													uint32_t  InputMessageLength,
													uint8_t  *DES_Key,
													uint8_t  *OutputMessage,
													uint32_t *OutputMessageLength
													)
{
		int32_t status = DES_SUCCESS;
		uint8_t IV[CRL_DES_BLOCK] = 
		{
				0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef
		};	
		status = STM32_DES_CBC_Decrypt(InputMessage, InputMessageLength, 
																	DES_Key, IV, sizeof(IV), 
																	OutputMessage,OutputMessageLength);
		return status;
}
	
int32_t STM32_DES_CBC_Encrypt(uint8_t*  InputMessage,
                        uint32_t  InputMessageLength,
                        uint8_t  *DES_Key,
                        uint8_t  *InitializationVector,
                        uint32_t  IvLength,
                        uint8_t  *OutputMessage,
                        uint32_t *OutputMessageLength)
{
		DESCBCctx_stt DESctx;

		uint32_t error_status = DES_SUCCESS;

		int32_t outputLength = 0;

		/* Set flag field to default value */
		DESctx.mFlags = E_SK_DEFAULT;

		/* Set iv size field to IvLength*/
		DESctx.mIvSize = IvLength;

		/* Initialize the operation, by passing the key.
		 * Third parameter is NULL because CBC doesn't use any IV */
		error_status = DES_CBC_Encrypt_Init(&DESctx, DES_Key, InitializationVector );

		/* check for initialization errors */
		if (error_status == DES_SUCCESS)
		{
				/* Encrypt Data */
				error_status = DES_CBC_Encrypt_Append(&DESctx,
																							InputMessage,
																							InputMessageLength,
																							OutputMessage,
																							&outputLength);

				if (error_status == DES_SUCCESS)
				{
					/* Write the number of data written*/
					*OutputMessageLength = outputLength;
					/* Do the Finalization */
					error_status = DES_CBC_Encrypt_Finish(&DESctx, OutputMessage + *OutputMessageLength, &outputLength);
					/* Add data written to the information to be returned */
					*OutputMessageLength += outputLength;
				}
		}

		return error_status;
}

int32_t STM32_DES_CBC_Decrypt(uint8_t*  InputMessage,
                        uint32_t  InputMessageLength,
                        uint8_t  *DES_Key,
                        uint8_t  *InitializationVector,
                        uint32_t  IvLength,
                        uint8_t  *OutputMessage,
                        uint32_t *OutputMessageLength)
{
		DESCBCctx_stt DESctx;

		uint32_t error_status = DES_SUCCESS;

		int32_t outputLength = 0;

		/* Set flag field to default value */
		DESctx.mFlags = E_SK_DEFAULT;

		/* Set iv size field to IvLength*/
		DESctx.mIvSize = IvLength;

		/* Initialize the operation, by passing the key.
		 * Third parameter is NULL because CBC doesn't use any IV */
		error_status = DES_CBC_Decrypt_Init(&DESctx, DES_Key, InitializationVector );

		/* check for initialization errors */
		if (error_status == DES_SUCCESS)
		{
				/* Decrypt Data */
				error_status = DES_CBC_Decrypt_Append(&DESctx,
																							InputMessage,
																							InputMessageLength,
																							OutputMessage,
																							&outputLength);

				if (error_status == DES_SUCCESS)
				{
						/* Write the number of data written*/
						*OutputMessageLength = outputLength;
						/* Do the Finalization */
						error_status = DES_CBC_Decrypt_Finish(&DESctx, OutputMessage + *OutputMessageLength, &outputLength);
						/* Add data written to the information to be returned */
						*OutputMessageLength += outputLength;
				}
		}

		return error_status;
}

//void Test_DES(void)
//{
//		const uint8_t Plaintext[32] =
//		{
//				0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,
//				0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31
//		};
//		uint8_t Key[CRL_DES_KEY] =
//		{
//				0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
//		};		
//		const uint8_t Expected_Ciphertext[32] =
//		{
//				0xd9,0x3d,0xe8,0xaf,0x2b,0x12,0x16,0xc3,0x5a,0x63,0x82,0x11,0x71,0x1b,0xf,0x65,
//				0x85,0xa6,0x85,0x92,0x9e,0x58,0xd5,0x22,0x93,0x24,0x15,0x6a,0xba,0x83,0xfa,0xdb, 
//		};	
//	
//		int32_t DesStatus = 0;
//		uint8_t OutPut[32];
//		uint32_t Len_OutPut;
//		int i;

//		memset(OutPut,0,32);
//		Len_OutPut = 0;
//		DesStatus = My_DES_Encrypt((uint8_t*)Plaintext,32,Key,OutPut,&Len_OutPut);
//		if(DesStatus == 0)
//		{
//				printf("DES_Encrypt:\r\n");
//				for(i=0;i<Len_OutPut;i++)
//						printf("0x%x,",OutPut[i]);
//				printf("\r\n");
//		}
//		else
//		{
//				printf("DES Encrypt Failed!!!\r\n");
//		}
//				
//		memset(OutPut,0,32);
//		Len_OutPut = 0;		
//		DesStatus = My_DES_Decrypt((uint8_t*)Expected_Ciphertext,32,Key,OutPut,&Len_OutPut);
//		if(DesStatus == 0)
//		{
//				printf("DES_Decrypt:\r\n");
//				for(i=0;i<Len_OutPut;i++)
//						printf("0x%x,",OutPut[i]);
//				printf("\r\n");
//		}
//		else
//		{
//				printf("DES Decrypt Failed!!!\r\n");
//		}	
//}
	
	
	
