/**
  ******************************************************************************
  * @file    aes_keywrap.h
  * @author  MCD Application Team
  * @version V2.0.6
  * @date    25-June-2013
  * @brief   AES in KeyWrap Mode
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
  *****************************************************************************/
/*!  
* \page Tutorial_AES_Key_Wrap AES-Key Wrap Tutorial
*
* Key Wrap mode is an authenticated encrypted mode specifically designed to designed to encapsulate 
* (encrypt) cryptographic key material (i.e, keys).
* Here follows an example of AES-128-Key Wrap encryption, it's used to wrap (encrypt and authenticate) another
* AES 128 key. 
* Please note that the IV is defined as a 64-bit block, and not as a 128-bit block as in the case of the general AES. \n
* Another property is that the \ref AES_KeyWrap_Encrypt_Append function can be called only one time, and all the input
* should be passed in that call. This is usually not a problem since input size is very small for key wrap usages.
* The output size will be 8 byte longer than input size in order to contain the authentication tag, which is
* appended to the output message. In decryption the input size should consider this 8 byte for the tag.
* 
* Please remember that before starting to call the Encryption function the context \b requires user 
* initialization. Look at the each function documentation to see what is needed prior of calling.
*
* The API functions to be used are:
*  - \ref AES_KeyWrap_Encrypt_Init initialize an \ref AESKWctx_stt context with key, iv and flags.
*  - \ref AES_KeyWrap_Encrypt_Append process the input and produces the encrypted output.
*    It can be called one time only, its output size will be 8 bytes longer than input size.
*  - \ref AES_KeyWrap_Encrypt_Finish can be called only one time for the finalization process
*
* Example:
*
* \code
* #include <stdio.h>
* #include "crypto.h"
* int32_t main()
* {
*   // Key that will be used to wrap another key 
*   const uint8_t key_128[CRL_AES128_KEY]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};  
*   // Key to be wrapped (input)
*   const uint8_t kwrap_plaintext[CRL_AES128_KEY]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
*   // IV ( NIST defined )
*   const uint8_t kwrap_iv[CRL_AES_KWRAP_BLOCK]={0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6};
*   //Output 
*   uint8_t kwrap_output[CRL_AES128_KEY + 8]={0,};
*   int32_t outSize, retval;  
*   AESKWctx_stt KWctx_st; // The AES context    
*    
*   // Initialize Context Flag with default value  
*   KWctx_st.mFlags = E_SK_DEFAULT;
*   
*   // Set key size to 16
*   KWctx_st.mKeySize=CRL_AES128_KEY;
*   
*   // call init function
*   retval = AES_KeyWrap_Encrypt_Init(&KWctx_st, key_128, kwrap_iv);
*   if (retval != AES_SUCCESS)
*   {  ... }
*   
*   // call append function, this will produce the output
*   retval = AES_KeyWrap_Encrypt_Append(&KWctx_st, kwrap_plaintext, CRL_AES128_KEY,  kwrap_output, &outSize);
*   if (retval != AES_SUCCESS)
*   {  ... }
*   
*   // call final function
*   retval += AES_KeyWrap_Encrypt_Finish(&KWctx_st, kwrap_output, &outSize);
*   if (retval != AES_SUCCESS)
*   {  ... }
*   
* }
* \endcode
*/
#ifndef __CRL_AES_KEYWRAP_H__
#define __CRL_AES_KEYWRAP_H__

#ifdef __cplusplus
 extern "C" {
#endif

/** @ingroup AESKEYWRAP
  * @{
  */

/** AES context structure for Key Wrap mode */
typedef AESCBCctx_stt AESKWctx_stt;
/** Number of uint32_t words in a Key Wrap Block */
#define  CRL_KWRAP_BLOCK_SIZE 2
/** Number of uint8_t bytes in a Key Wrap Block */
#define  CRL_AES_KWRAP_BLOCK 8

#ifdef INCLUDE_ENCRYPTION
/* load the key and ivec, eventually performs key schedule, init hw, etc. */
int32_t AES_KeyWrap_Encrypt_Init (AESKWctx_stt *P_pAESKWctx, const uint8_t *P_pKey, const uint8_t *P_pIv);

/* launch crypto operation , can be called just one times */
int32_t AES_KeyWrap_Encrypt_Append (AESKWctx_stt *P_pAESKWctx,
                                    const uint8_t *P_pInputBuffer,
                                    int32_t        P_inputSize,
                                    uint8_t       *P_pOutputBuffer,
                                    int32_t       *P_pOutputSize);

/* Possible final output */
int32_t AES_KeyWrap_Encrypt_Finish (AESKWctx_stt *P_pAESKWctx,
                                    uint8_t       *P_pOutputBuffer,
                                    int32_t       *P_pOutputSize);
#endif /* ENC */
#ifdef INCLUDE_DECRYPTION
/* load the key and ivec, eventually performs key schedule, init hw, etc. */
int32_t AES_KeyWrap_Decrypt_Init (AESKWctx_stt *P_pAESKWctx, const uint8_t *P_pKey, const uint8_t *P_pIv);

/* launch crypto operation , can be called several times */
int32_t AES_KeyWrap_Decrypt_Append (AESKWctx_stt *P_pAESKWctx,
                                    const uint8_t *P_pInputBuffer,
                                    int32_t        P_inputSize,
                                    uint8_t       *P_pOutputBuffer,
                                    int32_t       *P_pOutputSize);

/* Possible final output */
int32_t AES_KeyWrap_Decrypt_Finish (AESKWctx_stt *P_pAESKWctx,
                                    uint8_t       *P_pOutputBuffer,
                                    int32_t       *P_pOutputSize);
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /*__CRL_AES_KEYWRAP_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
