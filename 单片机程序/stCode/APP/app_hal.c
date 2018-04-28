#include "app_hal.h"
#include "usbd_customhid.h"

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern USBD_HandleTypeDef hUsbDeviceFS;

void USB_DataReceiveHander(uint8_t * data, int len) //USB HID 接收处理函数
{		
		len_hid = len;
		memmove(hid_data,data,len_hid);
		hid_flag = 1;
}

void SendUSBData(uint8_t *data ,int len)
{
		USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,data,len);
		USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)&hUsbDeviceFS;
		while(hhid->state == CUSTOM_HID_BUSY);
}

uint8_t Scan_USB(void)
{
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9))
				return 1;
		else
				return 0;
}

void System_Reset(void)
{
		__set_FAULTMASK(1);
    NVIC_SystemReset();
}

void SysTick_Handler(void)
{
		HAL_IncTick();
		HAL_SYSTICK_IRQHandler();
}

void OTG_FS_IRQHandler(void)
{
		HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}


