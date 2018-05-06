/**
 * \file
 * \brief CryptoAuthLib Basic API methods - a simple crypto authentication api.
 * These methods manage a global ATCADevice object behind the scenes.  They also
 * manage the wake/idle state transitions so callers don't need to.
 *
 * \copyright Copyright (c) 2017 Microchip Technology Inc. and its subsidiaries (Microchip). All rights reserved.
 *
 * \page License
 *
 * You are permitted to use this software and its derivatives with Microchip
 * products. Redistribution and use in source and binary forms, with or without
 * modification, is permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Microchip may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with a
 *    Microchip integrated circuit.
 *
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL MICROCHIP BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "cryptoauthlib.h"
#include "crypto/atca_crypto_sw_sha2.h"

#ifndef ATCA_BASIC_H_
#define ATCA_BASIC_H_

#define TBD   void

/** \defgroup atcab_ Basic Crypto API methods (atcab_)
 *
 * \brief
 * These methods provide the most convenient, simple API to CryptoAuth chips
 *
   @{ */

#ifdef __cplusplus
extern "C" {
#endif

// basic global device object methods
ATCA_STATUS atcab_version(char *verstr);

ATCA_STATUS atcab_wakeup(void);
ATCA_STATUS atcab_idle(void);
ATCA_STATUS atcab_sleep(void);

// basic crypto API
ATCA_STATUS atcab_info(uint8_t *revision);
ATCA_STATUS atcab_challenge(const uint8_t *challenge);
ATCA_STATUS atcab_challenge_seed_update(const uint8_t *seed, uint8_t* rand_out);
ATCA_STATUS atcab_nonce_base(uint8_t mode, const uint8_t *num_in, uint8_t* rand_out);
ATCA_STATUS atcab_nonce(const uint8_t *tempkey);
ATCA_STATUS atcab_nonce_rand(const uint8_t *seed, uint8_t* rand_out);
ATCA_STATUS atcab_random(uint8_t *rand_out);

ATCA_STATUS atcab_is_locked(uint8_t zone, bool *is_locked);
//ATCA_STATUS atcab_is_slot_locked(uint8_t slot, bool *is_locked);

ATCA_STATUS atcab_get_addr(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint16_t* addr);
ATCA_STATUS atcab_get_zone_size(uint8_t zone, uint16_t slot, size_t* size);
ATCA_STATUS atcab_read_zone(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint8_t *data, uint8_t len);
ATCA_STATUS atcab_write(uint8_t zone, uint16_t address, const uint8_t *value, const uint8_t *mac);
ATCA_STATUS atcab_write_zone(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, const uint8_t *data, uint8_t len);
ATCA_STATUS atcab_write_bytes_zone(uint8_t zone, uint16_t slot, size_t offset_bytes, const uint8_t *data, size_t length);
ATCA_STATUS atcab_read_bytes_zone(uint8_t zone, uint16_t slot, size_t offset_bytes, uint8_t *data, size_t length);

ATCA_STATUS atcab_read_serial_number(uint8_t* serial_number);


ATCA_STATUS atcab_read_enc(uint16_t key_id, uint8_t block, uint8_t *data, const uint8_t* enckey, const uint16_t enckeyid);
ATCA_STATUS atcab_write_enc(uint16_t key_id, uint8_t block, const uint8_t *data, const uint8_t* enckey, const uint16_t enckeyid);

ATCA_STATUS atcab_gendig(uint8_t zone, uint16_t key_id, const uint8_t *other_data, uint8_t other_data_size);
ATCA_STATUS atcab_mac(uint8_t mode, uint16_t key_id, const uint8_t* challenge, uint8_t* digest);
ATCA_STATUS atcab_checkmac(uint8_t mode, uint16_t key_id, const uint8_t *challenge, const uint8_t *response, const uint8_t *other_data);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* ATCA_BASIC_H_ */
