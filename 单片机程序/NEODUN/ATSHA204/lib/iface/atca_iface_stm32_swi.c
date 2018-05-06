
#include <string.h>//memcmp
#include "atca_iface.h"
#include "hal/hal_timer.h"

#include "SWI_Atsha204a.h"

/* * * * * * * 代码注释仅供参考 * * * * * * */

#ifdef USE_SWI_INTERFACE//只有使用SWI单线接口才会编译这个版本的函数，在atca_iface.h头文件设置编译开关

ATCA_STATUS atsend(uint8_t *txdata, int txlength)//发送长度实际函数里都强制为uint8_t
{
	//! Skip the Word Address data as SWI doesn't use it --no,just place the SWI_FLAG_CMD at the begin of the packet
	txdata[0] = 0x77;//SWI_FLAG_CMD;//!< flag preceding a command;send it first to tell chip command data is commming
	txlength++;
	
	swi_send_bytes(txlength, txdata);
	
	return ATCA_SUCCESS;
}


ATCA_STATUS atreceive(uint8_t *rxdata, uint16_t *rxlength)
{
	ATCA_STATUS status = ATCA_RX_TIMEOUT;
	uint8_t count;
	uint8_t retries = 10;
while (retries-- > 0 && status != ATCA_SUCCESS)//实测retries基本1次成功
{
	status =  swi_receive_bytes(*rxlength, rxdata);
	if (status == ATCA_RX_FAIL) //如果接收多于1个字节发生timeout，会返回这个代码，这很可能代表接收成功了一定数量的字节数，但应该是与预期的count不匹配(小于count),很可能是4.
   {
         count = rxdata[0];
         if ( (count < 4) || (count > *rxlength) ) 
          {
            status = ATCA_INVALID_SIZE;
            break;
          }
         else
         {
            status = ATCA_SUCCESS;
         }
    }
  else if (status == ATCA_RX_TIMEOUT) 
   {
      status = ATCA_RX_NO_RESPONSE;
   }
	 //else //status == ATCA_SUCCESS
	 
}
	return status;
}

ATCA_STATUS atwake(void)
{
	ATCA_STATUS status = ATCA_WAKE_FAILED;//可不必初始化
	uint8_t response[4] = { 0x00, 0x00, 0x00, 0x00 };
	//如果上次执行的命令不是Security Commands，返回的数据块长度总会是4（返回的第一个字节表示长度）：
	//count长度，status/error 状态值，2 字节CRC 校验值
	//After being Waked,Prior to First Command，(设备处于刚被唤醒状态时,读回的状态)0x11 Indicates that the ATSHA204A has received a proper Wake token.
	uint8_t expected_response[4] = { 0x04, 0x11, 0x33, 0x43 };
	uint16_t len = sizeof(response);

  swi_send_wake_token();
	atca_delay_ms(3);//WAKE_TWHI//SDA恢复高电平后要等待2.5mS
	
	status = atreceive(response, &len);
	if (status == ATCA_SUCCESS)
	{
		//! Compare response with expected_response
		if (memcmp(response, expected_response, 4) != 0)
			status = ATCA_WAKE_FAILED;
	}

	return status;
}

ATCA_STATUS atidle(void)
{
	uint8_t cmdFlag = 0xBB;//SWI_FLAG_IDLE//!< flag requesting to go into Idle mode

	swi_send_bytes(1,&cmdFlag);
	
	atca_delay_ms(1);
	
	return ATCA_SUCCESS;
}

ATCA_STATUS atsleep(void)
{
	uint8_t cmdFlag = 0xCC;//SWI_FLAG_SLEEP //!< flag requesting to go into Sleep mode

	swi_send_bytes(1,&cmdFlag);
	
	atca_delay_ms(1);
	
	return ATCA_SUCCESS;
}

#endif //#ifdef USE_SWI_INTERFACE


