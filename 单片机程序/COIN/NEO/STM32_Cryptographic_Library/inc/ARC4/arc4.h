/**
  ******************************************************************************
  * @file    arc4.h
  * @author  MCD Application Team
  * @version V2.0.6
  * @date    25-June-2013
  * @brief   ARC4
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
/*! \page Tutorial_ARC4 ARC4 Tutorial
  *
  * Here follows an example of ARC4 encryption.
  * Please remember that before starting to call the Encryption function the context \b requires user 
  * initialization. The Key Size member must be initialized prior to calling the 
  *  init function. Look at the each function documentation to see what is needed prior of calling.
  *
  * The API functions to be used are:
  *  - \ref ARC4_Encrypt_Init initialize an \ref ARC4ctx_stt context with the key (the IV should be NULL).
  *  - \ref ARC4_Encrypt_Append process the input and produces the output. 
  *    It can be called multiple times with any input size.
  *  - \ref ARC4_Encrypt_Finish can be called only one time for the finalization process
  *
  * The following code performs a ARC4 encryption of 1024 bytes in a single Append call.
  * \code
  * #include <stdio.h>                                                          
  * #include "crypto.h"                                                         
  *                                                                             
  * int32_t main()                                                              
  * {                                                                           
  *   const uint8_t ARC4_key[10]={0,1,2,3,4,5,6,7,8,9};                         
  *   const uint8_t plaintext[1024]={...};                                      
  *   const uint8_t ciphertext[1024];                                           
  *   // outSize is for output size, retval is for return value                 
  *   int32_t outSize, retval;                                                  
  *   ARC4ctx_stt ctx; // The ARC4 context                                      
  *   // Initialize context with Key Size                                       
  *   ctx.mKeySize = sizeof(ARC4_key);                                          
  *   // Call Init Function (note, there's no IV in ARC4)                       
  *   retval = ARC4_Encrypt_Init(&ctx, ARC4_key, NULL);                         
  *   if (retval != ARC4_SUCCESS)                                               
  *   { ... }                                                                   
  *   // Perform a single Append call to process all plaintext                  
  *   retval = ARC4_Encrypt_Append(&ctx, plaintext, 1024, ciphertext, &outSize);
  *   if (retval != ARC4_SUCCESS || outSize != 1024)                            
  *   { ... }                                                                   
  *   //Do the finalization call (in ARC4 it will not return any output)        
  *   retval = ARC4_Encrypt_Finish(&ctx, NULL, &outSize);                       
  *   if (retval != ARC4_SUCCESS)                                               
  *   { ... }                                                                   
  * }                                                                           
  * \endcode
*/  
#ifndef __CRL_ARC4_H__
#define __CRL_ARC4_H__

#ifdef __cplusplus
 extern "C" {
#endif

/** @ingroup ARC4
  * @{
  */

/*---------**- Types and Constants definition -**-----------------------------------------------------------*/

/** 
  * @brief  Structure describing an ARC4 context 
  */ 
typedef struct
{
  uint32_t   mContextId;  /*!< Unique ID of this AES-GCM Context. \b Not \b used in current implementation. */  
  SKflags_et mFlags;      /*!< 32 bit mFlags, for future use */
  const uint8_t *pmKey;   /*!< Pointer to original Key buffer */
  int32_t   mKeySize;     /*!< ARC4 Key length in bytes. This must be set by the caller prior to calling Init */  
  uint8_t   mX;           /*!< Internal members: This describe one of two index variables of the ARC4 state */  
  uint8_t   mY;           /*!< Internal members: This describe one of two index variables of the ARC4 state */  
  uint8_t   amState[256]; /*!< Internal members: This describe the 256 bytes State Matrix */
} ARC4ctx_stt;


/*---------**-  HIGH LEVEL FUNCTIONS -**------------------------------------------------*/

int32_t ARC4_Encrypt_Init(ARC4ctx_stt *P_pARC4ctx, const uint8_t *P_pKey, const uint8_t *P_pIv);

int32_t ARC4_Encrypt_Append(ARC4ctx_stt *P_pARC4ctx,
                            const uint8_t *P_pInputBuffer,
                            int32_t        P_inputSize,
                            uint8_t       *P_pOutputBuffer,
                            int32_t       *P_pOutputSize);

int32_t ARC4_Encrypt_Finish(ARC4ctx_stt *P_pARC4ctx,
                            uint8_t       *P_pOutputBuffer,
                            int32_t       *P_pOutputSize);

int32_t ARC4_Decrypt_Init(ARC4ctx_stt *P_pARC4ctx, const uint8_t *P_pKey, const uint8_t *P_pIv);

int32_t ARC4_Decrypt_Append(ARC4ctx_stt *P_pARC4ctx,
                            const uint8_t *P_pInputBuffer,
                            int32_t        P_inputSize,
                            uint8_t       *P_pOutputBuffer,
                            int32_t       *P_pOutputSize);

int32_t ARC4_Decrypt_Finish (ARC4ctx_stt *P_pARC4ctx,
                             uint8_t       *P_pOutputBuffer,
                             int32_t       *P_pOutputSize);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /*__CRL_AES_ECB_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
