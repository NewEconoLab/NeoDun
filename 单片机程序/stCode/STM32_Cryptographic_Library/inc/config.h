/**
  ******************************************************************************
  * @file    config.h
  * @author  MCD Application Team
  * @version V2.0.6
  * @date    25-June-2012
  * @brief   This file contains all the functions prototypes for the config firmware 
  *          library.
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

/* Define to prevent recursive inclusion -------------------------------------*/


#ifndef __CRL_CONFIG_H__
#define __CRL_CONFIG_H__

/** @defgroup ConfOptions Library Configuration Options
  * @{
  */

  
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/*************************************************************************************************
***-*-*-*-*--**- CONFIGURATION OPTIONS -**--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-***
*************************************************************************************************/

/** 
  * @defgroup CortexConf Compiler setting
  * @{
  */
//#define CORTEX_M0 /*!< Library is compiled for Cortex M0 */
//#define CORTEX_M3 /*!< Library is compiled for Cortex M3 */
//#define CORTEX_M4 /*!< Library is compiled for Cortex M4 */
/**
  * @}
  */ 


/** 
  * @defgroup SoCConf Cryptographic hardware accellerators
  * @{
  */
//#define STM32F2XX
//#define STM32F21X /*!< Use this if you are using STM32 Series F21X which has the cryptographic hardware accellerators */
//#define STM32F4XX
//#define STM32F41X /*!< Use this if you are using STM32 Series F41X which has the cryptographic hardware accellerators */

/**
  * @}
  */


/** 
  * @defgroup EndiannessConf Endianness
  * @{
  */
/*! Used to specify the memory representation of the platform.
  *   - CRL_ENDIANNESS=1 for LITTLE ENDIAN
  *   - CRL_ENDIANNESS=2 for BIG ENDIAN \n
  *   This setting is \b VERY \b IMPORTANT */
#define CRL_ENDIANNESS  1  /*!< LITTLE ENDIAN is selected */
/**
  * @}
  */ 

/**  
  * @defgroup  MisalignedConf Misaligned words read/write operations.
  * @{
  */
/*! Used to specify if the processor supports misaligned integers read/write operations. \n
  * To be portable this flag is not defined but if the processor supports read/write operations of 4
  * bytes to address not multiple by 4 then setting this flag to 1 will improve the performance
  * of AES when used through high level functions (ex AES_CBC or crl_AES).
  *******************************************************************/
#define CRL_CPU_SUPPORT_MISALIGNED  /*!< improve the performance of AES when used through high level functions */
 /**
  * @}
  */ 

/** 
  * @defgroup EncDecConf Encryption and/or Decryption functionalities
  * @{
  */
#define INCLUDE_ENCRYPTION  /*!< This defines includes the Encryption functionalities, remove it if only decryption is needed */
#define INCLUDE_DECRYPTION  /*!< This defines includes the Decryption functionalities, remove it if only encryption is needed */
/**
  * @}
  */ 

/** 
  * @defgroup SymKeyConf Symmetric Key Algorithms
  * @{
  */

#define INCLUDE_DES     /*!< This defines if DES functions are included in the library. */
#define INCLUDE_TDES    /*!< This defines if TripleDES (TDES) functions are included in the library. */
#define INCLUDE_AES128  /*!< This defines if AES functions with key size of 128 bit are included in the library \n If it's \b NOT defined then aes128.c is not needed. */ 
#define INCLUDE_AES192  /*!< This defines if AES functions with key size of 192 bit are included in the library \n If it's \b NOT defined then aes192.c is not needed. */
#define INCLUDE_AES256  /*!< This defines if AES functions with key size of 256 bit are included in the library \n If it's \b NOT defined then aes256.c is not needed. */
#define INCLUDE_ARC4    /*!< This defines enable the ARC4 algorithm */
/**
  * @}
  */ 


/**
  * @defgroup SymKeyModesConf Modes of Operations for Symmetric Key Algorithms
  * @{
  */

#define INCLUDE_ECB      /*!< This defines if AES high level functions for ECB mode are included in the library */
#define INCLUDE_CBC      /*!< This defines if AES high level functions for CBC mode are included in the library */
#define INCLUDE_CTR      /*!< This defines if AES high level functions for CTR mode are included in the library */
#define INCLUDE_GCM      /*!< This defines if AES high level functions for GCM mode are included in the library */
#define INCLUDE_KEY_WRAP /*!< This defines if AES-KWRAP is included in the library */
#define INCLUDE_CCM      /*!< This defines if AES-CCM is included in the library */
#define INCLUDE_CMAC     /*!< This defines if AES-CMAC is included in the library */
/**
  * @}
  */ 

/** 
  * @defgroup AsymKeyConf Asymmetric Key Algorithms
  * @{
  */

#define INCLUDE_RSA        /*!< This defines if RSA functions for signature verification are included in the library */
#define INCLUDE_ECC        /*!< This defines if ECC functions are included in the library */
/**
  * @}
  */ 

/** 
  * @defgroup HashConf Hash and HMAC Algorithms
  * @{
  */

#define INCLUDE_MD5     /*!< This defines if MD5 functions are included in the library */
#define INCLUDE_SHA1    /*!< This defines if SHA-1 functions are included in the library */
#define INCLUDE_SHA224  /*!< This defines if SHA-224 functions are included in the library */
#define INCLUDE_SHA256  /*!< This defines if SHA-256 functions are included in the library */
#define INCLUDE_HMAC    /*!< Select if, for the selected hash algorithms, HMAC should be included in the library */
 /**
  * @}
  */ 

/**
  * @defgroup DRBGConf Deterministic Random Bit Generator Based on AES-128
  * @{
  */
#define INCLUDE_DRBG_AES128  /*!< This includes the deterministic random bit generator */
#define CRL_RANDOM_REQUIRE_RESEED /*!< If defined CRL_RANDOM_REQUIRE_RESEED implements the request for reseed when using the DRBG too many times for security standards */
/**
  * @}
  */ 




/**
  * @defgroup OptiConf Configuration for Optimization
  * @{
  */

/** 
  * @defgroup AESConf AES version
  * @{
  */
/*! Used to select the AES algorithm version to use
 * - CRL_AES_ALGORITHM = 1 : version with 522 bytes of look-up tables, slower than version 2.
 * - CRL_AES_ALGORITHM = 2 : version with 2048 bytes of look-up tables, faster than version 1.*/
#define CRL_AES_ALGORITHM 2  /*!< version with 2048 bytes of look-up tables is selected */
/**
  * @}
  */ 

/**
  * @defgroup RSAConf RSA Window Size
  * @{
  */ 
/*! This parameter is used to speed up RSA operation with private key at expense of RAM memory.
  * It can't be set less than one.
  * Suggested values are 3 or 4. Entering a value of 7 or more will be probably worst than using 6.*/
#define RSA_WINDOW_SIZE 4 /*!< Window size selected is 4 */
/**
  * @}
  */ 

/** 
  * @defgroup GCMConf AES GCM GF(2^128) Precomputations Tables
   * @{
  */
/*! Used to specify the algorithm to be used for polynomial multiplication in AES-GCM.\n
  * The algorithm's choice also defines the size of the precomputed table made to speed
  * up the multiplication. \n There are two types of table, one is based on the value of
  * the key and so needs to be generated at runting (through AES_GCM_keyschedule), the other
  * is constant and is defined (if included here) in privkey.h.
  * There are 3 possible choices:
  *  - 0 = Without any tables. No space required. Slower version.
  *  - 1 = Key-dependent table for *Poly(y) 0000<y<1111 and constant table for *x^4 \n
  *       (256 key-dependent bytes - 32 constant bytes)
  *  - 2 = 4 key-dependent tables for *Poly(y^(2^(32*i))) and 4 key-dependent tables for *Poly((y*x^4)^(2^(32*i)))
  *        with 0000<y<1111 and 0<i<4 and constant tables for *x^8 and for *x^4\n
  *       (2048 key-dependent bytes - 544 constant bytes) */  
#define CRL_GFMUL 2 /*!< 4 key-dependent tables is selected */
/**
  * @}
  */ 

/**
  * @} End of optimization
  */ 

/**
  * @} End of options
  */ 

#endif  /*__CRL_CONFIG_H__*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
