
#include <string.h>//memcmp
#include "atca_iface.h"
#include "hal/hal_timer.h"

#include "bitBang_I2c.h"

#ifdef USE_TWI_INTERFACE//只有使用I2C接口才会编译这个版本的函数，在atca_iface.h头文件设置编译开关

ATCA_STATUS atsend(uint8_t *txdata, int txlength)//发送长度实际函数里都强制为uint8_t
{
	ATCA_STATUS status = ATCA_TX_TIMEOUT;//可不必初始化

	txdata[0] = 0x03;   //!< insert the Word Address Value, Command token
	txlength++;         //!< account for word address value byte.

	status = ATSHA_ISendNBytes(txdata,txlength);
	
	return status;
}

/*
  uint16_t *rxlength：
  接收长度实际函数里都强制为uint8_t
  参数为指针型，实际在函数内部并没有对实际接收长度有任何修改
  调用者对rxlength的返回值做了判断，这变得没有意义。
  rxdata[0]实际保存了实际读回的数据长度，调用者对它进行了检查。
*/

ATCA_STATUS atreceive(uint8_t *rxdata, uint16_t *rxlength)
{
	ATCA_STATUS status = ATCA_RX_TIMEOUT;//可不必初始化

  status = ATSHA_IRcvNBytes(rxdata,*rxlength);

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

  ATSHA_i2c_send_wake_token();
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
	ATCA_STATUS status = ATCA_TX_TIMEOUT;//可不必初始化
	uint8_t WordAddress = 0x02;//!< Word Address Value = Idle

	status = ATSHA_ISendNBytes(&WordAddress,1);
	
	atca_delay_ms(1);
	return status;
}

ATCA_STATUS atsleep(void)
{
	ATCA_STATUS status = ATCA_TX_TIMEOUT;//可不必初始化

	uint8_t WordAddress = 0x01;//!< Word Address Value = Sleep

	status = ATSHA_ISendNBytes(&WordAddress,1);
	
	atca_delay_ms(1);
	return status;
}

#endif //#ifdef USE_TWI_INTERFACE


