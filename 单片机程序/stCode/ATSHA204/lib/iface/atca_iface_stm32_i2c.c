
#include <string.h>//memcmp
#include "atca_iface.h"
#include "hal/hal_timer.h"

#include "bitBang_I2c.h"

#ifdef USE_TWI_INTERFACE//ֻ��ʹ��I2C�ӿڲŻ��������汾�ĺ�������atca_iface.hͷ�ļ����ñ��뿪��

ATCA_STATUS atsend(uint8_t *txdata, int txlength)//���ͳ���ʵ�ʺ����ﶼǿ��Ϊuint8_t
{
	ATCA_STATUS status = ATCA_TX_TIMEOUT;//�ɲ��س�ʼ��

	txdata[0] = 0x03;   //!< insert the Word Address Value, Command token
	txlength++;         //!< account for word address value byte.

	status = ATSHA_ISendNBytes(txdata,txlength);
	
	return status;
}

/*
  uint16_t *rxlength��
  ���ճ���ʵ�ʺ����ﶼǿ��Ϊuint8_t
  ����Ϊָ���ͣ�ʵ���ں����ڲ���û�ж�ʵ�ʽ��ճ������κ��޸�
  �����߶�rxlength�ķ���ֵ�����жϣ�����û�����塣
  rxdata[0]ʵ�ʱ�����ʵ�ʶ��ص����ݳ��ȣ������߶��������˼�顣
*/

ATCA_STATUS atreceive(uint8_t *rxdata, uint16_t *rxlength)
{
	ATCA_STATUS status = ATCA_RX_TIMEOUT;//�ɲ��س�ʼ��

  status = ATSHA_IRcvNBytes(rxdata,*rxlength);

	return status;
}

ATCA_STATUS atwake(void)
{
	ATCA_STATUS status = ATCA_WAKE_FAILED;//�ɲ��س�ʼ��
	uint8_t response[4] = { 0x00, 0x00, 0x00, 0x00 };
	//����ϴ�ִ�е������Security Commands�����ص����ݿ鳤���ܻ���4�����صĵ�һ���ֽڱ�ʾ���ȣ���
	//count���ȣ�status/error ״ֵ̬��2 �ֽ�CRC У��ֵ
	//After being Waked,Prior to First Command��(�豸���ڸձ�����״̬ʱ,���ص�״̬)0x11 Indicates that the ATSHA204A has received a proper Wake token.
	uint8_t expected_response[4] = { 0x04, 0x11, 0x33, 0x43 };
	uint16_t len = sizeof(response);

  ATSHA_i2c_send_wake_token();
	atca_delay_ms(3);//WAKE_TWHI//SDA�ָ��ߵ�ƽ��Ҫ�ȴ�2.5mS
	
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
	ATCA_STATUS status = ATCA_TX_TIMEOUT;//�ɲ��س�ʼ��
	uint8_t WordAddress = 0x02;//!< Word Address Value = Idle

	status = ATSHA_ISendNBytes(&WordAddress,1);
	
	atca_delay_ms(1);
	return status;
}

ATCA_STATUS atsleep(void)
{
	ATCA_STATUS status = ATCA_TX_TIMEOUT;//�ɲ��س�ʼ��

	uint8_t WordAddress = 0x01;//!< Word Address Value = Sleep

	status = ATSHA_ISendNBytes(&WordAddress,1);
	
	atca_delay_ms(1);
	return status;
}

#endif //#ifdef USE_TWI_INTERFACE


