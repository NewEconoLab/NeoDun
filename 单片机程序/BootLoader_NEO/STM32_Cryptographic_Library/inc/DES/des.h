/**
  ******************************************************************************
  * @file    des.h
  * @author  MCD Application Team
  * @version V2.0.6
  * @date    25-June-2013
  * @brief   Container for the DES functionalities
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

/*!
* \page Tutorial_DES_TDES DES and TDES Tutorials
*
*
*  In this tutorial there will be described the high level functions for DES and TDES in ECB and CBC modes.
*  The API is composed by separate functions for doing Initialization, Data Processing, Finalization. \n
*  To correctly use these API
*    the user should call once the Init function, which initialize a context, then the user can call the data
*    processing (Append) function as many times as it needs, to process different input, and at the end he should
*    call, once, the finalization function, to finish the processing. \n
*    Please refer to the following tutorials according to actual modes:
*    - \subpage Tutorial_DES_ECB
*    - \subpage Tutorial_DES_CBC
*    - \subpage Tutorial_TDES_ECB
*    - \subpage Tutorial_TDES_CBC
*
*   There are analogous functions for AES and ARC4 ( \ref AES, \ref ARC4 )
*
*/
#include "Common/des_common.h"

#ifdef INCLUDE_ECB
#include "ECB/des_ecb.h"
#endif

#ifdef INCLUDE_CBC
#include "CBC/des_cbc.h"
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
