#include <stdlib.h>
#include <stdio.h>

#include "cryptoauthlib.h"

#include "host\atca_host.h"

//#ifdef _WINDOWS
//#include "F:\RSN-1601\_Software\Atsha204aBurner2013 - macSample\ATsha204aBurner\tools.h"
//#define DISPLAY_DEBUG_INFO(displayMsg, bAlert)  displayGeneralMsg(displayMsg, bAlert)
//#else
#define DISPLAY_DEBUG_INFO(displayMsg, bAlert)//输出调试信息，用户可自行定义
//#endif

void display_total_info(bool bAlert, ATCA_STATUS status, const char *info, const uint8_t *data, uint8_t length)
{
	static char displayMsg[512];
	int i;
	char cell[16];

	sprintf(displayMsg, "atca_status=%02x ", (uint8_t)status);

	//info信息
	if (info != NULL)
		strcat(displayMsg, info);

	//数组
	if ((data != NULL) && (length != 0))
	{
		for (i = 0; i < length; i++)
		{
			sprintf(cell, "%02x ", data[i]);
			strcat(displayMsg, cell);
		}
	}

	//最终显示字符串
	DISPLAY_DEBUG_INFO(displayMsg, bAlert);
}

static bool test_assert_data_is_locked(void)
{
	bool is_locked = false;
	ATCA_STATUS status = atcab_is_locked(LOCK_ZONE_DATA, &is_locked);
	if (status != ATCA_SUCCESS)
	{
		display_total_info(true, status, "数据区检查锁定失败，退出......", NULL, 0);
		return false;
	}
	if (!is_locked)
	{
		DISPLAY_DEBUG_INFO("数据区须锁定方可继续......", true);
		return false;
	}
	return true;
}

static bool test_assert_config_is_locked(void)
{
	bool is_locked = false;
	ATCA_STATUS status = atcab_is_locked(LOCK_ZONE_CONFIG, &is_locked);
	if (status != ATCA_SUCCESS)
	{
		display_total_info(true, status, "配置区检查锁定失败，退出......", NULL, 0);
		return false;
	}
	if (!is_locked)
	{
		DISPLAY_DEBUG_INFO("配置区须锁定方可继续......", true);
		return false;
	}
	return true;
}

bool ATSHA_check_lock_config_zone(bool *isLocked)
{
	bool ret = false;
	ATCA_STATUS status;

	status = atcab_is_locked(LOCK_ZONE_CONFIG, isLocked);
	if (status == ATCA_SUCCESS)
	{
		if ((*isLocked) == false)
			display_total_info(false, status, "配置区未锁定.", NULL, 0);
		else
			display_total_info(false, status, "配置区已锁定.", NULL, 0);
		ret = true;
	}
	else
	{
		display_total_info(true, status, "配置区锁定状态检查失败.", NULL, 0);
		ret = false;
	}

	return ret;
}

bool  ATSHA_check_lock_data_zone(bool *isLocked)
{
	bool ret = false;
	ATCA_STATUS status;

	status = atcab_is_locked(LOCK_ZONE_DATA, isLocked);
	if (status == ATCA_SUCCESS)
	{
		if (!(*isLocked))
			display_total_info(false, status, "数据区未锁定.", NULL, 0);
		else
			display_total_info(false, status, "数据区已锁定.", NULL, 0);
		ret = true;
	}
	else
	{
		display_total_info(true, status, "数据区锁定状态检查失败.", NULL, 0);
		ret = false;
	}

	return ret;
}

//读4个字节的版本号
bool ATSHA_read_rev_number(uint8_t rev_number[4])
{
	bool ret = false;

	ATCA_STATUS status = ATCA_SUCCESS;

	status = atcab_info(rev_number);
	if (status == ATCA_SUCCESS)
	{
		  ret = true;
	}
	else
	{
			printf("status:%d\r\n",status);
		  ret = false;
	}

	return ret;
}

//读9个字节的序列号
bool ATSHA_read_serial_number(uint8_t serial_number[9])
{
	bool ret = false;
	ATCA_STATUS status = ATCA_SUCCESS;

	status = atcab_read_serial_number(serial_number);
	if (status == ATCA_SUCCESS)
	{
		display_total_info(false, status, "sn=", serial_number, 9);
		ret = true;
	}
	else
	{
		display_total_info(true, status, "读序列号失败.", NULL, 0);
		ret = false;
	}

	return ret;
}

//产生32字节的随机数，并更新内部种子
bool ATSHA_get_random(uint8_t randomnum[32])
{
	bool ret = false;
	ATCA_STATUS status = ATCA_GEN_FAIL;

	status = atcab_random(randomnum);
	if (status == ATCA_SUCCESS)
	{
		display_total_info(false, status, "rand=", randomnum, 32);
		ret = true;
	}
	else
	{
		display_total_info(true, status, "取随机数失败.", NULL, 0);
		ret = false;
	}

	return ret;
}

bool ATSHA_mac_key_challenge(uint8_t key_id, uint8_t key[32])
{
	bool ret = false;

	ATCA_STATUS status = ATCA_GEN_FAIL;
	uint8_t sn[9];
	atca_temp_key_t temp_key;
	atca_mac_in_out_t mac_params;
	uint8_t challenge[ATCA_KEY_SIZE];
	uint8_t host_response[ATCA_KEY_SIZE];
	uint8_t client_response[ATCA_KEY_SIZE];
	do {
		if (!test_assert_data_is_locked())
		{
			ret = false;
			break;
		}
		
		// Read serial number for host-side MAC calculations
		status = atcab_read_serial_number(sn);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "read_serial_number失败.", NULL, 0);
			ret = false;
			break;
		}

		// Use a random challenge
		status = atcab_random(challenge);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "取随机数失败.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "challenge  =", challenge, 32);
		}

		// Setup MAC command
		memset(&temp_key, 0, sizeof(temp_key));
		mac_params.mode = MAC_MODE_CHALLENGE | MAC_MODE_INCLUDE_SN; // Block 1 is a key, block 2 is a challenge
		mac_params.key_id = key_id;// 4;
		mac_params.challenge = challenge;
		mac_params.key = key;// g_slot4_key;
		mac_params.otp = NULL;
		mac_params.sn = sn;
		mac_params.response = host_response;
		mac_params.temp_key = &temp_key;

		display_total_info(false, ATCA_SUCCESS, "key_id  =", (uint8_t*)&(mac_params.key_id), 2);

		// Run MAC command
		status = atcab_mac(mac_params.mode, mac_params.key_id, mac_params.challenge, client_response);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "mac命令失败.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "mac命令响应client_response=", client_response, 32);
		}

		// Calculate expected MAC
		status = atcah_mac(&mac_params);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "主机计算host_response失败.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "host_response  =", host_response, 32);
		}
		//TEST_ASSERT_EQUAL_MEMORY(host_response, client_response, sizeof(host_response));
		if (memcmp(host_response, client_response, sizeof(host_response)) != 0)
		{
			display_total_info(true, status, "host_response<->client_response对比不正确.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "结果正确.", NULL, 0);
			ret = true;
		}
		
	}while (0);

	return ret;
}
bool ATSHA_mac_key_tempkey(uint8_t key_id, uint8_t key[32])
{
	bool ret = false;

	ATCA_STATUS status = ATCA_GEN_FAIL;
	uint8_t sn[9];
	atca_temp_key_t temp_key;
	uint8_t num_in[NONCE_NUMIN_SIZE];
	uint8_t rand_out[RANDOM_NUM_SIZE];
	atca_nonce_in_out_t nonce_params;
	atca_mac_in_out_t mac_params;
	uint8_t host_response[ATCA_KEY_SIZE];
	uint8_t client_response[ATCA_KEY_SIZE];
	do {
		if (!test_assert_data_is_locked())
		{
			ret = false;
			break;
		}
		
		// Read serial number for host-side MAC calculations
		status = atcab_read_serial_number(sn);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "read_serial_number失败.", NULL, 0);
			ret = false;
			break;
		}

		// Setup nonce command
		memset(&temp_key, 0, sizeof(temp_key));
		memset(num_in, 0, sizeof(num_in));
		memset(&nonce_params, 0, sizeof(nonce_params));
		nonce_params.mode = NONCE_MODE_SEED_UPDATE;
		nonce_params.num_in = num_in;
		nonce_params.rand_out = rand_out;
		nonce_params.temp_key = &temp_key;

		// Create random nonce
		status = atcab_nonce_base(nonce_params.mode, nonce_params.num_in, rand_out);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "nonce命令失败.", NULL, 0);
			ret = false;
			break;
		}

		// Calculate nonce
		status = atcah_nonce(&nonce_params);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "主机计算temp_key失败.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "主机计算temp_key  =", temp_key.value, 32);
		}

		// Setup MAC command
		memset(&mac_params, 0, sizeof(mac_params));
		mac_params.mode = MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_INCLUDE_SN; // Block 1 is a key, block 2 is TempKey
		mac_params.key_id = key_id;// 4;
		mac_params.challenge = NULL;
		mac_params.key = key;// g_slot4_key;
		mac_params.otp = NULL;
		mac_params.sn = sn;
		mac_params.response = host_response;
		mac_params.temp_key = &temp_key;

		display_total_info(false, ATCA_SUCCESS, "key_id  =", (uint8_t*)&(mac_params.key_id), 2);

		// Run MAC command
		status = atcab_mac(mac_params.mode, mac_params.key_id, mac_params.challenge, client_response);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "mac命令失败.", NULL, 0);
			ret = false;
			break;
		}

		// Calculate expected MAC
		status = atcah_mac(&mac_params);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "主机计算host_response失败.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "host_response  =", host_response, 32);
			display_total_info(false, status, "client_response=", client_response, 32);
		}
		//TEST_ASSERT_EQUAL_MEMORY(host_response, client_response, sizeof(host_response));
		if (memcmp(host_response, client_response, sizeof(host_response)) != 0)
		{
			display_total_info(true, status, "host_response<->client_response对比不正确.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "结果正确.", NULL, 0);
			ret = true;
		}
	} while (0);

	return ret;
}
bool ATSHA_sleepDown(void)
{
	bool ret = false;
	ATCA_STATUS status = ATCA_GEN_FAIL;

	do {
		status = atwake();
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "wake命令失败.", NULL, 0);
			ret = false;
			break;
		}
		status = atsleep();
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "sleep命令失败.", NULL, 0);
			ret = false;
			break;
		}

		ret = true;
	} while (0);

	if (status == ATCA_SUCCESS)
	{
		display_total_info(false, status, "wake-sleep测试成功.", NULL, 0);
	}

	return ret;
}


//数据槽正常读写，仅公开给：上海扎古网络科技有限公司

/******     数据槽访问函数      **********/
//  slot:0-15数据槽号；offset:0-7,槽内4字节为单元的偏移地址；len：只能为4或32，如为32，则offset必须为0
bool ATSHA_write_data_slot(uint8_t slot, uint8_t offset, const uint8_t *data, uint8_t len)
{
	bool ret = false;
	ATCA_STATUS status = ATCA_SUCCESS;
	do {
		if (!test_assert_config_is_locked())
		{
			ret = false;
			break;
		}
		//写数据区的数据槽，32字节
		status = atcab_write_zone(ATCA_ZONE_DATA, slot, 0, offset, data, len);//数据区未锁定之前只能32字节写
		if (status != ATCA_SUCCESS)                                           //锁定后可根据配置，写或禁止写(4,32字节)	                                                 
		{
			display_total_info(true, status, "数据槽写入失败. slot=", &slot, 1);
			ret = false;
		}
		else
		{
			display_total_info(false, status, "数据槽写入成功. slot=", &slot, 1);
			ret = true;
		}
	} while (0);
	return ret;
}

// 读出指定槽的全部32字节。
bool ATSHA_read_data_slot(uint8_t slot, uint8_t slot_data[32])
{
	bool ret = false;

	ATCA_STATUS status = ATCA_SUCCESS;
	do {
		if (!test_assert_data_is_locked())
		{
			ret = false;
			break;
		}
		//读32字节的数据区数据槽
		status = atcab_read_zone(ATCA_ZONE_DATA, slot, 0, 0, slot_data, ATCA_BLOCK_SIZE);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "数据槽读取失败. slot=", &slot, 1);
			ret = false;
		}
		else
		{
			display_total_info(false, status, "数据槽读取成功. slot=", &slot, 1);
			ret = true;
		}
	} while (0);

	return ret;
}


//加密读写，仅公开给：上海扎古网络科技有限公司

bool ATSHA_read_encrypted(uint16_t key_id, uint8_t *data, const uint16_t enckeyid)
{
		uint8_t key_15[32] ={ 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	                      0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF};
	bool ret = false;

	ATCA_STATUS status = ATCA_GEN_FAIL;

	do {
		if (!test_assert_data_is_locked())
		{
			ret = false;
			break;
		}

		status = atcab_read_enc(key_id, 0, data, key_15, enckeyid);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "加密读失败.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "加密读Slot ", (uint8_t*)&(key_id), 2);
			display_total_info(false, status, "加密读结果  =", data, 32);
			ret = true;
		}

	} while (0);

	return ret;
}

bool ATSHA_write_encrypted(uint16_t key_id, const uint8_t *data, const uint16_t enckeyid)
{
		uint8_t key_15[32] ={ 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	                      0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF};
	bool ret = false;

	ATCA_STATUS status = ATCA_GEN_FAIL;

	do {
		if (!test_assert_data_is_locked())
		{
			ret = false;
			break;
		}

		status = atcab_write_enc(key_id, 0, data, key_15, enckeyid);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "加密写失败.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "加密写Slot ", (uint8_t*)&(key_id), 2);
			display_total_info(false, status, "加密写结果  =", data, 32);
			ret = true;
		}

	} while (0);

	return ret;
}
