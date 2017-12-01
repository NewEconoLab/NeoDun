/**
  ******************************************************************************
  * @file    aes_gcm.h
  * @author  MCD Application Team
  * @version V2.0.6
  * @date    25-June-2013
  * @brief   AES in GCM Mode
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
* \page Tutorial_AES_GCM AES-GCM Tutorial
*
* Here follows an example of AES-128-GCM encryption.
* Please remember that before starting to call the Encryption function the context \b requires user 
* initialization. The Key Size, Iv (counter) Size, Tag Size, Flags member must be initialized prior to calling the 
*  init function. Look at the each function documentation to see what is needed prior of calling.
*
* The API functions to be used are:
*  - \ref AES_GCM_Encrypt_Init initialize an \ref AESGCMctx_stt context with key, IV, tag size, flags.
*  - \ref AES_GCM_Header_Append process the header data. This data is only authenticated and not encrypted.
*  - \ref AES_GCM_Encrypt_Append process the input and produces the encrypted output.
*    It can be called multiple times but the input size must be multiple of 16. 
*    A last append call is allowed with any value for the input size.
*  - \ref AES_GCM_Encrypt_Finish can be called only one time for the finalization process and it will generate
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
*   AESGCMctx_stt GCMctx_st;
*   int32_t retval, outSize;
*   
*   // Set default value for Flag
*   GCMctx_st.mFlags = E_SK_DEFAULT;
*   // Set Tag Size to 16 bytes
*   GCMctx_st.mTagSize = 16;
*   //Set Iv size to 12 ( the standard for GCM )
*   GCMctx_st.mIvSize=12;
*   //Set Key Size to 16
*   GCMctx_st.mKeySize=CRL_AES128_KEY;
*
*   // Initialize the context 
*   retval = AES_GCM_Encrypt_Init(&GCMctx_st, key_128, iv);
*   if (retval != AES_SUCCESS)
*   { ... }
*   
*   // Process Header (data to be authenticated but NOT encrypted)
*   retval = AES_GCM_Header_Append(&GCMctx_st, header, sizeof(header));
*   if (retval != AES_SUCCESS)
*   { ... }
*
*   // Process payload (data to be authenticated and encrypted)
*   retval = AES_GCM_Encrypt_Append(&GCMctx_st, plaintext, 1024, ciphertext, &outSize);
*   if (retval != AES_SUCCESS)
*   { ... }
*  
*   // Generate the authentication TAG
*   retval =  AES_GCM_Encrypt_Finish(&GCMctx_st, tag, &outSize);
*   if (retval != AES_SUCCESS)
*   { ... }
*
*   // Now reprocess the data in decryption and verify the tag we just generated
*   // First set context flag to E_SK_DONT_PERFORM_KEY_SCHEDULE. This will avoid to spend time to reperform
*   // The AES GCM key schedule, that has already be done previously 
*   // (GCM has the same Key Schedule for encryption and decryption)
*   GCMctx_st.mFlags = E_SK_DONT_PERFORM_KEY_SCHEDULE;
*   //Then we have to set the pointer to the tag to be verified
*   GCMctx_st.pmTag = tag;
*   // Init
*   retval = AES_GCM_Decrypt_Init(&GCMctx_st, key_128, iv);
*   // Process Header
*   retval |= AES_GCM_Header_Append(&GCMctx_st, header, sizeof(header));
*   // Decrypt Payload
*   retval |= AES_GCM_Decrypt_Append(&GCMctx_st, ciphertext, 1024, plaintext, &outSize);
*   if (retval != AES_SUCCESS) // (AES_SUCCESS is defined to 0)
*   { ... }
*   retval = AES_GCM_Decrypt_Finish(&GCMctx_st, NULL, &outSize);
*   if ( retval == AUTHENTICATION_SUCCESSFUL )
*     printf("Decryption/Authentication OK!");
*   else
*     printf("ERROR!");
* }
* \endcode
*/
#ifndef __CRL_AES_GCM_H__
#define __CRL_AES_GCM_H__

#ifdef __cplusplus
 extern "C" {
#endif

/** @ingroup AESGCMlowlevel
  * @{
  */
typedef uint32_t poly_t[4]; /*!< Definition of the way a polynomial of max degree 127 is represented */

#if CRL_GFMUL==1
typedef poly_t table16_t[16]; /*!< Definition of the type used for the precomputed table */
#elif CRL_GFMUL==2
typedef poly_t table8x16_t[8][16]; /*!< Definition of the type used for the precomputed table */
#else
#error "A value for CRL_GFMUL MUST be provided when INCLUDE_GCM is defined"
#endif

/** @} */

/** @ingroup AESGCM
  * @{
  */
  
typedef struct {  
  uint32_t   mContextId; /*!< Unique ID of this AES-GCM Context. \b Not \b used in current implementation. */  
  SKflags_et mFlags; /*!< 32 bit mFlags, used to perform keyschedule, choose betwen hw/sw/hw+dma and future use */  
  const uint8_t *pmKey; /*!< Pointer to original Key buffer */  
  const uint8_t *pmIv; /*!< Pointer to original Initialization Vector buffer */  
  int32_t   mIvSize; /*!< Size of the Initialization Vector in bytes. This must be set by the caller prior to calling Init */  
  uint32_t   amIv[4]; /*!< This is the current IV value.*/
  int32_t   mKeySize;   /*!< AES Key length in bytes. This must be set by the caller prior to calling Init */
  const uint8_t *pmTag; /*!< Pointer to Authentication TAG. This value must be set in decryption, and this TAG will be verified */  
  int32_t mTagSize; /*!< Size of the Tag to return. This must be set by the caller prior to calling Init */
  int32_t mAADsize;   /*!< Additional authenticated data size. For internal use. */
  int32_t mPayloadSize;   /*!< Payload size. For internal use. */  
  poly_t mPartialAuth; /*!< Partial authentication value. For internal use. */
  uint32_t amExpKey[CRL_AES_MAX_EXPKEY_SIZE];   /*!< AES Expanded key. For internal use.  */
#if CRL_GFMUL==0  
  poly_t  mPrecomputedValues; /*!< (CRL_GFMUL==0) No precomputed tables, just the polynomial. For internal use. */
#elif CRL_GFMUL==1  
  table16_t mPrecomputedValues; /*!< (CRL_GFMUL==1) Precomputation of polynomial according to Shoup's 4-bit table\n (Requires 256 bytes of key-dependent data and 32 bytes of constant data). For internal use. */
#elif CRL_GFMUL==2  
  table8x16_t mPrecomputedValues; /*!< (CRL_GFMUL==2) Precomputation of polynomial according to Shoup's 8-bit table\n (Requires 4096 bytes of key-dependent data and 512 bytes of constant data). For internal use. */
#endif
} AESGCMctx_stt; /*!< Structure used only for AES-GCM. Used to store the expanded key and, eventually,
                      precomputed tables, according the the defined value of CRL_GFMUL in config.h */

/* load the key and ivec, eventually performs key schedule, init hw, etc. */
int32_t AES_GCM_Encrypt_Init(AESGCMctx_stt *P_pAESGCMctx, const uint8_t *P_pKey, const uint8_t *P_pIv);

/* Header Processing Function */
int32_t AES_GCM_Header_Append(AESGCMctx_stt *P_pAESGCMctx,
                                const uint8_t *P_pInputBuffer,
                                int32_t        P_inputSize);

/* launch crypto operation , can be called several times */
int32_t AES_GCM_Encrypt_Append (AESGCMctx_stt *P_pAESGCMctx,
                                const uint8_t *P_pInputBuffer,
                                int32_t        P_inputSize,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);

/* Possible final output */
int32_t AES_GCM_Encrypt_Finish (AESGCMctx_stt *P_pAESGCMctx,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);

/* load the key and ivec, eventually performs key schedule, init hw, etc. */
int32_t AES_GCM_Decrypt_Init (AESGCMctx_stt *P_pAESGCMctx, const uint8_t *P_pKey, const uint8_t *P_pIv);

/* launch crypto operation , can be called several times */
int32_t AES_GCM_Decrypt_Append (AESGCMctx_stt *P_pAESGCMctx,
                                const uint8_t *P_pInputBuffer,
                                int32_t        P_inputSize,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);

/* Possible final output */
int32_t AES_GCM_Decrypt_Finish (AESGCMctx_stt *P_pAESGCMctx,
                                uint8_t       *P_pOutputBuffer,
                                int32_t       *P_pOutputSize);


/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __CRL_AES_GCM_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
