#ifndef ATSHA204A_H_
#define ATSHA204A_H_

#include <stdint.h>
#include "atca_bool.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	/*******            ATSHA204A加密芯片演示函数               *********/

	bool ATSHA_check_lock_config_zone(bool *isLocked);
	bool ATSHA_check_lock_data_zone(bool *isLocked);

	//读4个字节的版本号
	bool ATSHA_read_rev_number(uint8_t rev_number[4]);

	//读9个字节的序列号
	bool ATSHA_read_serial_number(uint8_t serial_number[9]);

	//产生32字节的随机数，并更新内部种子
	bool ATSHA_get_random(uint8_t randomnum[32]);

	
	//MAC命令验证
	bool ATSHA_mac_key_challenge(uint8_t key_id, uint8_t key[32]);
	bool ATSHA_mac_key_tempkey(uint8_t key_id, uint8_t key[32]);

	bool ATSHA_sleepDown(void);
	
	//普通读写及加密读写，仅公开给：上海扎古网络科技有限公司
	bool ATSHA_write_data_slot(uint8_t slot, uint8_t offset, const uint8_t *data, uint8_t len);
	bool ATSHA_read_data_slot(uint8_t slot, uint8_t slot_data[32]);
	
	bool ATSHA_read_encrypted(uint16_t key_id, uint8_t *data, const uint16_t enckeyid);
	bool ATSHA_write_encrypted(uint16_t key_id, const uint8_t *data, const uint16_t enckeyid);

#ifdef __cplusplus
}
#endif

#endif
