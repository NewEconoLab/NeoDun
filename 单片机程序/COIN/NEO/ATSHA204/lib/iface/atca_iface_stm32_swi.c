
#include <string.h>//memcmp
#include "atca_iface.h"
#include "hal/hal_timer.h"

#include "SWI_Atsha204a.h"

/* * * * * * * ����ע�ͽ����ο� * * * * * * */

#ifdef USE_SWI_INTERFACE//ֻ��ʹ��SWI���߽ӿڲŻ��������汾�ĺ�������atca_iface.hͷ�ļ����ñ��뿪��

ATCA_STATUS atsend(uint8_t *txdata, int txlength)//���ͳ���ʵ�ʺ����ﶼǿ��Ϊuint8_t
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
while (retries-- > 0 && status != ATCA_SUCCESS)//ʵ��retries����1�γɹ�
{
	status =  swi_receive_bytes(*rxlength, rxdata);
	if (status == ATCA_RX_FAIL) //������ն���1���ֽڷ���timeout���᷵��������룬��ܿ��ܴ�����ճɹ���һ���������ֽ�������Ӧ������Ԥ�ڵ�count��ƥ��(С��count),�ܿ�����4.
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
	ATCA_STATUS status = ATCA_WAKE_FAILED;//�ɲ��س�ʼ��
	uint8_t response[4] = { 0x00, 0x00, 0x00, 0x00 };
	//����ϴ�ִ�е������Security Commands�����ص����ݿ鳤���ܻ���4�����صĵ�һ���ֽڱ�ʾ���ȣ���
	//count���ȣ�status/error ״ֵ̬��2 �ֽ�CRC У��ֵ
	//After being Waked,Prior to First Command��(�豸���ڸձ�����״̬ʱ,���ص�״̬)0x11 Indicates that the ATSHA204A has received a proper Wake token.
	uint8_t expected_response[4] = { 0x04, 0x11, 0x33, 0x43 };
	uint16_t len = sizeof(response);

  swi_send_wake_token();
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


