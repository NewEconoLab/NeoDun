/**
  ******************************************************************************
  * @file    aes_ccm.h
  * @author  MCD Application Team
  * @version V2.0.6
  * @date    25-June-2013
  * @brief   AES in CCM (Counter with CBC-MAC) Mode
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
*
* \page Tutorial_AES_CCM AES-CCM Tutorial
*
* Here follows an example of AES-128-CCM encryption.
* Please remember that before starting to call the Encryption function the context \b requires user 
* initialization. The Key Size, Iv (counter) Size, Tag Size, Flags member must be initialized prior to calling the 
*  init function. Look at the each function documentation to see what is needed prior of calling.
*
* The API functions to be used are:
*  - \ref AES_CCM_Encrypt_Init initialize an \ref AESCCMctx_stt context with key, IV, tag size, flags.
*  - \ref AES_CCM_Header_Append process the header data. This data is only authenticated and not encrypted.
*  - \ref AES_CCM_Encrypt_Append process the input and produces the encrypted output.
*    It can be called multiple times but the input size must be multiple of 16. 
*    A last append call is allowed with any value for the input size.
*  - \ref AES_CCM_Encrypt_Finish can be called only one time for the finalization process and it will generate
*         the authentication TAG
* \b Authenticated \b Encryption \b API
*
* Authenticated encryption functionality usually require more parameters to be initialized by the user
* inside the context. Such as the size of the returned/checked authentication tag and a pointer to it in
* case of verification.
* Example:
*
* \code
* #include <stdio.h>
* #include "crypto.h"
* int32_t main()
* {
*   uint8_t key_128[CRL_AES128_KEY]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
*   uint8_t iv[12]={0,1,2,3,4,5,6,7,8,9,10,11};
*   uint8_t plaintext[1024]={...};
*   uint8_t ciphertext[1024];
*   uint8_t header[65]={...};
*   uint8_t tag[16];
*   AESCCMctx_stt CCMctx_st;
*   int32_t retval, outSize;
*   
*   // Set default value for Flag
*   CCMctx_st.mFlags = E_SK_DEFAULT;
*   // Set Tag Size to 16 bytes
*   CCMctx_st.mTagSize = 16;
*   //Set Nonce size to 12 ( the standard for CCM )
*   CCMctx_st.mNonceSize = 12;
*   //Set Key Size to 16
*   CCMctx_st.mKeySize = CRL_AES128_KEY;
*
*   //CCM also requires to write before the Init the number of bytes that will be processed:
*   //For the payload:
*   CCMctx_st.mPayload = sizeof(plaintext);
*   //And for the associated data (i.e. Header)
*   CCMctx_st.mAssDataSize = sizeof(header);

*   // Initialize the context 
*   retval = AES_CCM_Encrypt_Init(&CCMctx_st, key_128, iv);
*   if (retval != AES_SUCCESS)
*   { ... }
*   
*   // Process Header (data to be authenticated but NOT encrypted)
*   retval = AES_CCM_Header_Append(&CCMctx_st, header, sizeof(header));
*   if (retval != AES_SUCCESS)
*   { ... }
*
*   // Process payload (data to be authenticated and encrypted)
*   retval = AES_CCM_Encrypt_Append(&CCMctx_st, plaintext, 1024, ciphertext, &outSize);
*   if (retval != AES_SUCCESS)
*   { ... }
*  
*   // Generate the authentication TAG
*   retval =  AES_CCM_Encrypt_Finish(&CCMctx_st, tag, &outSize);
*   if (retval != AES_SUCCESS)
*   { ... }
*
*   // Now reprocess the data in decryption and verify the tag we just generated
*   // First set context flag to E_SK_DONT_PERFORM_KEY_SCHEDULE. This will avoid to spend time to reperform
*   // The AES CCM key schedule, that has already be done previously 
*   // (CCM has the same Key Schedule for encryption and decryption)
*   CCMctx_st.mFlags = E_SK_DONT_PERFORM_KEY_SCHEDULE;
*   //Then we have to set the pointer to the tag to be verified
*   CCMctx_st.pmTag = tag;
*   // Init
*   retval = AES_CCM_Decrypt_Init(&CCMctx_st, key_128, iv);
*   // Process Header
*   retval |= AES_CCM_Header_Append(&CCMctx_st, header, sizeof(header));
*   // Decrypt Payload
*   retval |= AES_CCM_Decrypt_Append(&CCMctx_st, ciphertext, 1024, plaintext, &outSize);
*   if (retval != AES_SUCCESS) // (AES_SUCCESS is defined to 0)
*   { ... }
*   retval = AES_CCM_Decrypt_Finish(&CCMctx_st, NULL, &outSize);
*   if ( retval == AUTHENTICATION_SUCCESSFUL )
*     printf("Decryption/Authentication OK!");
*   else
*     printf("ERROR!");
* }
* \endcode
*/

#ifndef __CRL_AES_CCM_H__
#define __CRL_AES_CCM_H__

#ifdef __cplusplus
 extern "C" {
#endif

/** @ingroup AESCCM
  * @{
  */

/** Structure used only for AES-GCM. Used to store the expanded key and, eventually,
*   precomputed tables, according the the defined value of CRL_GFMUL in config.h */
typedef struct {
  /** Unique ID of this AES-GCM Context. \b Not \b used in current implementation. */
  uint32_t   mContextId;
  /** 32 bit mFlags, used to perform keyschedule, choose betwen hw/sw/hw+dma and future use */
  SKflags_et mFlags;
  /** Pointer to original Key buffer */
  const uint8_t *pmKey;
  /** Pointer to original Nonce buffer */
  const uint8_t *pmNonce;
  /** Size of the Nonce in bytes. This must be set by the caller prior to calling Init.
       Possible values are {7,8,9,10,11,12,13}  */
  int32_t   mNonceSize;
  /** This is the current IV value for encryption.*/
  uint32_t   amIvCTR[4];
  /** This is the current IV value for authentication.*/
  uint32_t   amIvCBC[4];
  /** AES Key length in bytes. This must be set by the caller prior to calling Init */
  int32_t   mKeySize;
  /** Pointer to Authentication TAG. This value must be set in decryption, and this TAG will be verified */
  const uint8_t *pmTag;
  /** Size of the Tag to return. This must be set by the caller prior to calling Init.
      Possible values are values are {4,6,8,10,12,14,16} */
  int32_t mTagSize;
  /** Size of the associated data to be processed yet. This must be set by the caller prior to calling Init.*/
  int32_t mAssDataSize;
  /** Size of the payload data to be processed yet size. This must be set by the caller prior to calling Init. */
  int32_t mPayloadSize;
  /** AES Expanded key. For internal use.  */
  uint32_t amExpKey[CRL_AES_MAX_EXPKEY_SIZE];
  /** Temp Buffer */
  uint32_t amTmpBuf[CRL_AES_BLOCK/sizeof(uint32_t)];
  /**  Number of bytes actually in use */
  int32_t mTmpBufUse;
} AESCCMctx_stt;

/* load the key and ivec, eventually performs key schedule, init hw, etc. */
int32_t AES_CCM_Encrypt_Init(AESCCMctx_stt *P_pAESCCMctx, const uint8_t *P_pKey, const uint8_t *P_pNonce);

/* Header Processing Function */
int32_t AES_CCM_Header_Append(AESCCMctx_stt *P_pAESCCMctx,
                                const uint8_t *P_pInputBuffer,
                                int32_t        P_inputSize);

/* launch crypto operation , can be called several times */
int32_t AES_CCM_Encrypt_Append (AESCCMctx_stt *P_pAESCCMctx,
                                const uint8_t *P_pInputBuffer,
                                int32_t        P_inputSize,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);

/* Possible final output */
int32_t AES_CCM_Encrypt_Finish (AESCCMctx_stt *P_pAESCCMctx,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);

/* load the key and ivec, eventually performs key schedule, init hw, etc. */
int32_t AES_CCM_Decrypt_Init(AESCCMctx_stt *P_pAESCCMctx, const uint8_t *P_pKey, const uint8_t *P_pNonce);

/* launch crypto operation , can be called several times */
int32_t AES_CCM_Decrypt_Append (AESCCMctx_stt *P_pAESCCMctx,
                                const uint8_t *P_pInputBuffer,
                                int32_t        P_inputSize,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);

/* Possible final output */
int32_t AES_CCM_Decrypt_Finish (AESCCMctx_stt *P_pAESCCMctx,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);

 /**
  * @}  
  */

#ifdef __cplusplus
}
#endif
  
#endif /* __CRL_AES_CCM_H__ */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
