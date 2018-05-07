/**
  ******************************************************************************
  * @file    sha256.h
  * @author  MCD Application Team
  * @version V2.0.6
  * @date    25-June-2013
  * @brief   Provides SHA256 functions
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
  ******************************************************************************/

/*! \page Tutorial_SHA256 SHA256 Tutorial
  *
  * Here follows an example of SHA256 functionality.
  * Please remember that before starting to call the Init function the context \b requires user 
  * initialization. The mFlags and mTagSize member must be initialized prior to calling the 
  *  init function. Look at the each function documentation to see what is needed prior of calling.
  *
  * The API functions to be used are:
  *  - \ref SHA256_Init initialize an \ref SHA256ctx_stt context with required digest size and flags.
  *  - \ref SHA256_Append process input buffers.
  *    It can be called multiple times.
  *  - \ref SHA256_Finish can be called only one time for the digest generation process.
  *
  * The following code performs a SHA256 hash of a zero-size buffer in a single Append call
  * \code
  * #include <stdio.h>
  * #include "crypto.h"
  * int32_t main()
  * {
  *   uint8_t message[1024]={...};
  *   uint8_t digest[CRL_SHA256_SIZE]; // 32 byte buffer
  *   // outSize is for digest output size, retval is for return value
  *   int32_t outSize, retval;
  *   SHA256ctx_stt context_st; // The SHA256 context
  *
  *   // Initialize Context Flag with default value
  *   context_st.mFlags = E_HASH_DEFAULT;
  *   // Set the required digest size to be of 32 byte
  *   context_st.mTagSize = 32;
  *
  *   // call init function
  *   retval = SHA256_Init(&context_st);
  *   if (retval != HASH_SUCCESS)
  *   {  ... }
  *
  *   retval = SHA256_Append(&context_st, message, 0);
  *   if (retval != HASH_SUCCESS)
  *   {  ... }
  *
  *   //Generate the message digest
  *   retval = SHA256_Finish(&context_st, digest, &outSize );
  *   if (retval != HASH_SUCCESS)
  *   {  ... }
  * }
  * \endcode
*/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CRL_SHA256_H__
#define __CRL_SHA256_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @ingroup SHA256
  * @{
  */ 
#define CRL_SHA256_SIZE 32 /*!<  Number of bytes (uint8_t) to store a SHA-256 digest. */

/** Structure for the SHA-256 context */ 
typedef HASHctx_stt SHA256ctx_stt;

/* Initialization of Context */
int32_t SHA256_Init(SHA256ctx_stt *P_pSHA256ctx);
/* Data processing function */
int32_t SHA256_Append(SHA256ctx_stt *P_pSHA256ctx, const uint8_t *P_pInputBuffer, int32_t P_inputSize);
/* Returns digest */
int32_t SHA256_Finish(SHA256ctx_stt *P_pSHA256ctx, uint8_t *P_pOutputBuffer, int32_t *P_pOutputSize);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif   /*__CRL_SHA256_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
