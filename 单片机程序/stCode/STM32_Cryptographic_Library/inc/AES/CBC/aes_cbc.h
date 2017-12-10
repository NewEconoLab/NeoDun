/**
  ******************************************************************************
  * @file    aes_cbc.h
  * @author  MCD Application Team
  * @version V2.0.6
  * @date    25-June-2013
  * @brief   AES in CBC Mode
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/*! \page Tutorial_AES_CBC AES-CBC Tutorial
  *
  * Here follows an example of AES-128-CBC encryption.
  * Please remember that before starting to call the Encryption function the context \b requires user 
  * initialization. The Key Size, Iv Size, Flags member must be initialized prior to calling the 
  *  init function. Look at the each function documentation to see what is needed prior of calling.
  *
  * The API functions to be used are:
  *  - \ref AES_CBC_Encrypt_Init initialize an \ref AESCBCctx_stt context with key and initialization vector
  *  - \ref AES_CBC_Encrypt_Append process the input and produces the output. 
  *    It can be called multiple times but the input size must be multiple of 16. 
  *    A last append call is allowed with input size greater than 16 but not multiple of 16, 
  *    in this case Ciphertext Stealing will be used
  *  - \ref AES_CBC_Encrypt_Finish can be called only one time for the finalization process
  *
  * The following code performs a CBC encryption with AES-128 of 1024 in 4 Append calls.
  * \code
  * #include <stdio.h>
  * #include "crypto.h"
  * int32_t main()
  * {
  *   uint8_t key[CRL_AES128_KEY]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  *   uint8_t iv[CRL_AES_BLOCK]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  *   uint8_t plaintext[1024]={...};
  *   uint8_t ciphertext[1024];
  *   // outSize is for output size, retval is for return value
  *   int32_t outSize, retval;
  *   AESCBCctx_stt AESctx_st; // The AES context
  *
  *   // Initialize Context Flag with default value
  *   AESctx_st.mFlags = E_SK_DEFAULT;
  *   // Set Iv size to 16
  *   AESctx_st.mIvSize=16;
  *   // Set key size to 16
  *   AESctx_st.mKeySize=CRL_AES128_KEY;
  *   // call init function
  *   retval = AES_CBC_Encrypt_Init(&AESctx_st, key, iv);
  *   if (retval != AES_SUCCESS)
  *   {  ... }
  *
  *   // Loop to perform four calls to AES_CBC_Encrypt_Append, each processing 256 bytes
  *   for (i = 0; i < 1024; i += 256)
  *   { //Encrypt i bytes of plaintext. Put the output data in ciphertext and number 
  *     //of written bytes in outSize
  *     retval = AES_CBC_Encrypt_Append(&AESctx_st, plaintext, 256, ciphertext, &outSize);
  *     if (retval != AES_SUCCESS)
  *     {  ... }
  *   }
  *   //Do the finalization call (in CBC it will not return any output)
  *   retval = AES_CBC_Encrypt_Finish(&AESctx_st, ciphertext+outSize, &outSize );
  *   if (retval != AES_SUCCESS)
  *   {  ... }
  * }
  * \endcode
*/

#ifndef __CRL_AES_CBC_H__
#define __CRL_AES_CBC_H__

#ifdef __cplusplus
 extern "C" {
#endif

/** @ingroup AESCBC
  * @{
  */

#ifdef INCLUDE_ENCRYPTION
/* load the key and ivec, eventually performs key schedule, init hw, etc. */
int32_t AES_CBC_Encrypt_Init (AESCBCctx_stt *P_pAESCBCctx, const uint8_t *P_pKey, const uint8_t *P_pIv);

/* launch crypto operation , can be called several times */
int32_t AES_CBC_Encrypt_Append (AESCBCctx_stt *P_pAESCBCctx,
                                const uint8_t *P_pInputBuffer,
                                int32_t        P_inputSize,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);

/* Possible final output */
int32_t AES_CBC_Encrypt_Finish (AESCBCctx_stt *P_pAESCBCctx,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);
#endif
#ifdef INCLUDE_DECRYPTION
/* load the key and ivec, eventually performs key schedule, init hw, etc. */
int32_t AES_CBC_Decrypt_Init (AESCBCctx_stt *P_pAESCBCctx, const uint8_t *P_pKey, const uint8_t *P_pIv);

/* launch crypto operation , can be called several times */
int32_t AES_CBC_Decrypt_Append (AESCBCctx_stt *P_pAESCBCctx,
                                const uint8_t *P_pInputBuffer,
                                int32_t        P_inputSize,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);

/* Possible final output */
int32_t AES_CBC_Decrypt_Finish (AESCBCctx_stt *P_pAESCBCctx,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);
#endif
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __CRL_AES_CBC_H__*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
