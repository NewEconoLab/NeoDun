/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @author         : MCD Application Team
  * @version        : V2.2.0
  * @date           : 13-June-2014
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */ 
#define PACK_SIZE      64
	0x05, 0x8c, /* USAGE_PAGE (ST Page) */ 
	0x09, 0x01, /* USAGE (Demo Kit) */ 
	0xa1, 0x01, /* COLLECTION (Application) */ 
	
	// ���뱨�� 
	0x09,0x03, // USAGE ID - Vendor defined 
	0x15,0x00, // LOGICAL_MINIMUM (0) 
	0x26,0x00, 0xff, // LOGICAL_MAXIMUM (255) 
	0x75,0x08, // REPORT_SIZE (8bit) 
	0x95,0x40, // REPORT_COUNT (64Byte) 
	0x81,0x02, // INPUT (Data,Var,Abs) 

	// ������� 
	0x09,0x04, // USAGE ID - Vendor defined 
	0x15,0x00, // LOGICAL_MINIMUM (0) 
	0x26,0x00,0xff, // LOGICAL_MAXIMUM (255) 
	0x75,0x08, // REPORT_SIZE (8bit) 
	0x95,0x40, // REPORT_COUNT (64Byte) 
	0x91,0x02, // OUTPUT (Data,Var,Abs) 
  /* USER CODE END 0 */ 
  0xC0    /*     END_COLLECTION	             */
   
}; 
/* USB handler declaration */
/* Handle for USB Full Speed IP */
USBD_HandleTypeDef  *hUsbDevice_0;

extern USBD_HandleTypeDef hUsbDeviceFS;

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static int8_t CUSTOM_HID_Init_FS     (void);
static int8_t CUSTOM_HID_DeInit_FS   (void);
static int8_t CUSTOM_HID_OutEvent_FS (uint8_t event_idx, uint8_t state);
/* USER CODE BEGIN 2 */ 

extern USBD_HandleTypeDef hUsbDeviceFS;
extern void USB_DataReceiveHander(uint8_t * data ,int len );
/* USER CODE END 2 */ 

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS = 
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS,
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  CUSTOM_HID_Init_FS
  *         Initializes the CUSTOM HID media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  hUsbDevice_0 = &hUsbDeviceFS;
  /* USER CODE BEGIN 4 */ 
  return (0);
  /* USER CODE END 4 */ 
}

/**
  * @brief  CUSTOM_HID_DeInit_FS
  *         DeInitializes the CUSTOM HID media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */ 
  return (0);
  /* USER CODE END 5 */ 
}

/**
  * @brief  CUSTOM_HID_OutEvent_FS
  *         Manage the CUSTOM HID class events       
  * @param  event_idx: event index
  * @param  state: event state
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS  (uint8_t event_idx, uint8_t state)
{ 
  /* USER CODE BEGIN 6 */ 
	USB_DataReceiveHander(((USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData)->Report_buf,USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
  return (0);
  /* USER CODE END 6 */ 
}

/* USER CODE BEGIN 7 */ 
/**
  * @brief  USBD_CUSTOM_HID_SendReport_FS
  *         Send the report to the Host       
  * @param  report: the report to be sent
  * @param  len: the report length
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
/*  
static int8_t USBD_CUSTOM_HID_SendReport_FS ( uint8_t *report,uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(hUsbDevice_0, report, len); 
}
*/
/* USER CODE END 7 */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
