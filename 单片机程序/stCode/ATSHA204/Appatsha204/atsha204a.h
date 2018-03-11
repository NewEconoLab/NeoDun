#ifndef ATSHA204A_H_
#define ATSHA204A_H_

#include <stdint.h>
#include "atca_bool.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	/*******            ATSHA204A����оƬ��ʾ����               *********/

	bool ATSHA_check_lock_config_zone(bool *isLocked);
	bool ATSHA_check_lock_data_zone(bool *isLocked);

	//��4���ֽڵİ汾��
	bool ATSHA_read_rev_number(uint8_t rev_number[4]);

	//��9���ֽڵ����к�
	bool ATSHA_read_serial_number(uint8_t serial_number[9]);

	//����32�ֽڵ���������������ڲ�����
	bool ATSHA_get_random(uint8_t randomnum[32]);

	
	//MAC������֤
	bool ATSHA_mac_key_challenge(uint8_t key_id, uint8_t key[32]);
	bool ATSHA_mac_key_tempkey(uint8_t key_id, uint8_t key[32]);

	bool ATSHA_sleepDown(void);
	
	//��ͨ��д�����ܶ�д�������������Ϻ���������Ƽ����޹�˾
	bool ATSHA_write_data_slot(uint8_t slot, uint8_t offset, const uint8_t *data, uint8_t len);
	bool ATSHA_read_data_slot(uint8_t slot, uint8_t slot_data[32]);
	
	bool ATSHA_read_encrypted(uint16_t key_id, uint8_t *data, const uint16_t enckeyid);
	bool ATSHA_write_encrypted(uint16_t key_id, const uint8_t *data, const uint16_t enckeyid);

#ifdef __cplusplus
}
#endif

#endif
