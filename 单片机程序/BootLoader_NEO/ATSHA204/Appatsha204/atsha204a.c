#include <stdlib.h>
#include <stdio.h>

#include "cryptoauthlib.h"

#include "host\atca_host.h"

//#ifdef _WINDOWS
//#include "F:\RSN-1601\_Software\Atsha204aBurner2013 - macSample\ATsha204aBurner\tools.h"
//#define DISPLAY_DEBUG_INFO(displayMsg, bAlert)  displayGeneralMsg(displayMsg, bAlert)
//#else
#define DISPLAY_DEBUG_INFO(displayMsg, bAlert)//���������Ϣ���û������ж���
//#endif

void display_total_info(bool bAlert, ATCA_STATUS status, const char *info, const uint8_t *data, uint8_t length)
{
	static char displayMsg[512];
	int i;
	char cell[16];

	sprintf(displayMsg, "atca_status=%02x ", (uint8_t)status);

	//info��Ϣ
	if (info != NULL)
		strcat(displayMsg, info);

	//����
	if ((data != NULL) && (length != 0))
	{
		for (i = 0; i < length; i++)
		{
			sprintf(cell, "%02x ", data[i]);
			strcat(displayMsg, cell);
		}
	}

	//������ʾ�ַ���
	DISPLAY_DEBUG_INFO(displayMsg, bAlert);
}

static bool test_assert_data_is_locked(void)
{
	bool is_locked = false;
	ATCA_STATUS status = atcab_is_locked(LOCK_ZONE_DATA, &is_locked);
	if (status != ATCA_SUCCESS)
	{
		display_total_info(true, status, "�������������ʧ�ܣ��˳�......", NULL, 0);
		return false;
	}
	if (!is_locked)
	{
		DISPLAY_DEBUG_INFO("���������������ɼ���......", true);
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
		display_total_info(true, status, "�������������ʧ�ܣ��˳�......", NULL, 0);
		return false;
	}
	if (!is_locked)
	{
		DISPLAY_DEBUG_INFO("���������������ɼ���......", true);
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
			display_total_info(false, status, "������δ����.", NULL, 0);
		else
			display_total_info(false, status, "������������.", NULL, 0);
		ret = true;
	}
	else
	{
		display_total_info(true, status, "����������״̬���ʧ��.", NULL, 0);
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
			display_total_info(false, status, "������δ����.", NULL, 0);
		else
			display_total_info(false, status, "������������.", NULL, 0);
		ret = true;
	}
	else
	{
		display_total_info(true, status, "����������״̬���ʧ��.", NULL, 0);
		ret = false;
	}

	return ret;
}

//��4���ֽڵİ汾��
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

//��9���ֽڵ����к�
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
		display_total_info(true, status, "�����к�ʧ��.", NULL, 0);
		ret = false;
	}

	return ret;
}

//����32�ֽڵ���������������ڲ�����
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
		display_total_info(true, status, "ȡ�����ʧ��.", NULL, 0);
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
			display_total_info(true, status, "read_serial_numberʧ��.", NULL, 0);
			ret = false;
			break;
		}

		// Use a random challenge
		status = atcab_random(challenge);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "ȡ�����ʧ��.", NULL, 0);
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
			display_total_info(true, status, "mac����ʧ��.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "mac������Ӧclient_response=", client_response, 32);
		}

		// Calculate expected MAC
		status = atcah_mac(&mac_params);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "��������host_responseʧ��.", NULL, 0);
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
			display_total_info(true, status, "host_response<->client_response�ԱȲ���ȷ.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "�����ȷ.", NULL, 0);
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
			display_total_info(true, status, "read_serial_numberʧ��.", NULL, 0);
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
			display_total_info(true, status, "nonce����ʧ��.", NULL, 0);
			ret = false;
			break;
		}

		// Calculate nonce
		status = atcah_nonce(&nonce_params);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "��������temp_keyʧ��.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "��������temp_key  =", temp_key.value, 32);
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
			display_total_info(true, status, "mac����ʧ��.", NULL, 0);
			ret = false;
			break;
		}

		// Calculate expected MAC
		status = atcah_mac(&mac_params);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "��������host_responseʧ��.", NULL, 0);
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
			display_total_info(true, status, "host_response<->client_response�ԱȲ���ȷ.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "�����ȷ.", NULL, 0);
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
			display_total_info(true, status, "wake����ʧ��.", NULL, 0);
			ret = false;
			break;
		}
		status = atsleep();
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "sleep����ʧ��.", NULL, 0);
			ret = false;
			break;
		}

		ret = true;
	} while (0);

	if (status == ATCA_SUCCESS)
	{
		display_total_info(false, status, "wake-sleep���Գɹ�.", NULL, 0);
	}

	return ret;
}


//���ݲ�������д�������������Ϻ���������Ƽ����޹�˾

/******     ���ݲ۷��ʺ���      **********/
//  slot:0-15���ݲۺţ�offset:0-7,����4�ֽ�Ϊ��Ԫ��ƫ�Ƶ�ַ��len��ֻ��Ϊ4��32����Ϊ32����offset����Ϊ0
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
		//д�����������ݲۣ�32�ֽ�
		status = atcab_write_zone(ATCA_ZONE_DATA, slot, 0, offset, data, len);//������δ����֮ǰֻ��32�ֽ�д
		if (status != ATCA_SUCCESS)                                           //������ɸ������ã�д���ֹд(4,32�ֽ�)	                                                 
		{
			display_total_info(true, status, "���ݲ�д��ʧ��. slot=", &slot, 1);
			ret = false;
		}
		else
		{
			display_total_info(false, status, "���ݲ�д��ɹ�. slot=", &slot, 1);
			ret = true;
		}
	} while (0);
	return ret;
}

// ����ָ���۵�ȫ��32�ֽڡ�
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
		//��32�ֽڵ����������ݲ�
		status = atcab_read_zone(ATCA_ZONE_DATA, slot, 0, 0, slot_data, ATCA_BLOCK_SIZE);
		if (status != ATCA_SUCCESS)
		{
			display_total_info(true, status, "���ݲ۶�ȡʧ��. slot=", &slot, 1);
			ret = false;
		}
		else
		{
			display_total_info(false, status, "���ݲ۶�ȡ�ɹ�. slot=", &slot, 1);
			ret = true;
		}
	} while (0);

	return ret;
}


//���ܶ�д�������������Ϻ���������Ƽ����޹�˾

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
			display_total_info(true, status, "���ܶ�ʧ��.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "���ܶ�Slot ", (uint8_t*)&(key_id), 2);
			display_total_info(false, status, "���ܶ����  =", data, 32);
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
			display_total_info(true, status, "����дʧ��.", NULL, 0);
			ret = false;
			break;
		}
		else
		{
			display_total_info(false, status, "����дSlot ", (uint8_t*)&(key_id), 2);
			display_total_info(false, status, "����д���  =", data, 32);
			ret = true;
		}

	} while (0);

	return ret;
}
