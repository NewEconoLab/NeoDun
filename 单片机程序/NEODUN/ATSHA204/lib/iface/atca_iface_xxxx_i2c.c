
#include <string.h>//memcmp
#include "atca_iface.h"
#include "hal/hal_timer.h"

/*
  函数名，返回类型及参数类型、列表请用与原库保持一致，不要修改;
  以下提到的“主机”是指主控单片机如SM32,PIC32等，“芯片”指的是加密芯片ATSHA204A;
  关于硬件层如I2C总线，single-wire总线等如何配置，初始化等，需用户自行实现;
  芯片的I2C接口符合标准I2c协议，速度可达1MHz，one-wire总线用户可参考芯片数据手册实现。
*/

ATCA_STATUS atsend(uint8_t *txdata, int txlength)
{
	ATCA_STATUS status = ATCA_TX_TIMEOUT;//可不必初始化

	txdata[0] = 0x03;   //!< insert the Word Address Value, Command token，only for TWI.
	txlength++;         //!< account for word address value byte.

	////////////////////////////////////////////////////////////////////////////////////////////
	//实现细节：
	/* 负责将txdata里的数据通过通信总线发给芯片，例如I2C的实现流程如下：
	主机发送起始位；
	主机发送芯片地址（典型为0xC8,写，最后一位为0）；
	主机发送txdata里的数据，数据长度为参数txlength所指定；
	数据发送完毕后，主机发送停止位；
	*/
	////////////////////////////////////////////////////////////////////////////////////////////

	//关于返回值：如果数据发送成功，芯片应该对主机发送的每一个字节包括地址有正反馈（ACK），此种情况下返回ATCA_SUCCESS即可，
	//否则可参考atca_status.h文件里的定义，酌情返回错误代码：	
	return status;
}

ATCA_STATUS atreceive(uint8_t *rxdata, uint16_t *rxlength)
{
	ATCA_STATUS status = ATCA_RX_TIMEOUT;//可不必初始化

	////////////////////////////////////////////////////////////////////////////////////////////
	//实现细节：
	/*
	负责从芯片接收数据，期望的数据长度为参数*rxlength所指定，I2C的实现流程如下：
	a.读首字节(代表芯片本次可读回数据的长度)：
	  发送启动位,发送芯片地址（典型为0xC9,读，最后一位为1），然后读一个字节（actual_Len），主机发给芯片正反馈（ACK），不发送停止位；
	  将首字节与期望值rxlength进行比较，如果不匹配(可参考源码)，则认为出错，主机补充发送停止位后函数返回错误ATCA_INVALID_SIZE；
	b.读回剩余的数据流：
	  注意返回数据的首字节应存放实际数据长度：rxdata[0] = actual_Len;
	  读回流程：数据长度为actual_Len - 1, 存放首地址为&rxdata[1]
	  不发送启动位，不发送地址，仅读数据，每个字节发送正应答，最后字节发送负应答，最后发送停止位；
	*/
	////////////////////////////////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////////////////////////////////
	//实现细节：
	/*负责唤醒芯片，并确认其状态；I2C的实现流程如下：
	发送wake token: 方式1：将SCL线拉低，然后延时至少延时60uS,典型值为80uS，延时结束将SCL释放拉高；
	               方式2：通过I2C总线发送单字节0x00,但总线速率必须足够低，本例设为100kHz:
	                     启动总线,发字节0x00(主机应收到非反馈nack),主机发送停止位
	主机等待至少2.5mS，典型值3mS；
	此时芯片应该被唤醒，主机读它的状态以确认，直接调用上面写好的函数atreceive，期望数据长度是4：
	     status = atreceive(response, 4);
	如果读取状态数据成功，则将读回的数据流与预期的比较，一致则说明唤醒成功，否则返回失败：
	 if (status == ATCA_SUCCESS)
	 {
	    //! Compare response with expected_response
	    if (memcmp(response, expected_response, 4) != 0)
	    status = ATCA_WAKE_FAILED;
	 }
	*/
	////////////////////////////////////////////////////////////////////////////////////////////

	return status;
}

ATCA_STATUS atidle(void)
{
	ATCA_STATUS status = ATCA_TX_TIMEOUT;//可不必初始化
	uint8_t WordAddress = 0x02;//!< Word Address Value = Idle

	////////////////////////////////////////////////////////////////////////////////////////////
	//实现细节：
	/*
	  令芯片进入Idle状态而非Sleep状态，I2C的实现流程如下：
	  发送启动位,发送芯片地址（典型为0xC8,写，最后一位为0），然后写一个字节（0x02），最后发送停止位；
	  芯片应该对主机发送的每一个字节包括地址有正反馈（ACK），否则应返回失败。
	*/
	////////////////////////////////////////////////////////////////////////////////////////////
	
	atca_delay_ms(1);
	return status;
}

ATCA_STATUS atsleep(void)
{
	ATCA_STATUS status = ATCA_TX_TIMEOUT;//可不必初始化

	uint8_t WordAddress = 0x01;//!< Word Address Value = Sleep

	////////////////////////////////////////////////////////////////////////////////////////////
	//实现细节：
	/*
	  令芯片进入Sleep低功耗休眠状态，I2C的实现流程如下：
	  发送启动位,发送芯片地址（典型为0xC8,写，最后一位为0），然后写一个字节（0x01），最后发送停止位；
	  芯片应该对主机发送的每一个字节包括地址有正反馈（ACK），否则应返回失败。
	*/
	////////////////////////////////////////////////////////////////////////////////////////////
	
	atca_delay_ms(1);
	return status;
}

