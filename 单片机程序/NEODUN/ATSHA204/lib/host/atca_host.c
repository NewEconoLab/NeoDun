/**
 * \file
 * \brief Host side methods to support CryptoAuth computations
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

#include "atca_host.h"
#include "crypto/atca_crypto_sw_sha2.h"


/** \brief This function copies otp and sn data into a command buffer.
 *
 * \param[in, out] param pointer to parameter structure
 * \return pointer to command buffer byte that was copied last
 */
uint8_t *atcah_include_data(struct atca_include_data_in_out *param)
{
    if (param->mode & MAC_MODE_INCLUDE_OTP_88)
    {
        memcpy(param->p_temp, param->otp, 11);            // use OTP[0:10], Mode:5 is overridden
        param->p_temp += 11;
    }
    else
    {
        if (param->mode & MAC_MODE_INCLUDE_OTP_64)
            memcpy(param->p_temp, param->otp, 8);         // use 8 bytes OTP[0:7] for (6)
        else
            memset(param->p_temp, 0, 8);                  // use 8 zeros for (6)
        param->p_temp += 8;

        memset(param->p_temp, 0, 3);                     // use 3 zeros for (7)
        param->p_temp += 3;
    }

    // (8) 1 byte SN[8]
    *param->p_temp++ = param->sn[8];

    // (9) 4 bytes SN[4:7] or zeros
    if (param->mode & MAC_MODE_INCLUDE_SN)
        memcpy(param->p_temp, &param->sn[4], 4);           //use SN[4:7] for (9)
    else
        memset(param->p_temp, 0, 4);                       //use zeros for (9)
    param->p_temp += 4;

    // (10) 2 bytes SN[0:1]
    *param->p_temp++ = param->sn[0];
    *param->p_temp++ = param->sn[1];

    // (11) 2 bytes SN[2:3] or zeros
    if (param->mode & MAC_MODE_INCLUDE_SN)
        memcpy(param->p_temp, &param->sn[2], 2);           //use SN[2:3] for (11)
    else
        memset(param->p_temp, 0, 2);                       //use zeros for (9)
    param->p_temp += 2;

    return param->p_temp;
}

/** \brief
    \param[in, out] param pointer to parameter structure
    \return status of the operation
 */
ATCA_STATUS atcah_nonce(struct atca_nonce_in_out *param)
{
    uint8_t temporary[ATCA_MSG_SIZE_NONCE];
    uint8_t *p_temp;

    // Check parameters
    if (!param->temp_key || !param->num_in || (param->mode > NONCE_MODE_PASSTHROUGH) || (param->mode == NONCE_MODE_INVALID)
        || (((param->mode == NONCE_MODE_SEED_UPDATE || (param->mode == NONCE_MODE_NO_SEED_UPDATE)) && !param->rand_out)))
        return ATCA_BAD_PARAM;

    // Calculate or pass-through the nonce to TempKey->Value
    if ((param->mode == NONCE_MODE_SEED_UPDATE) || (param->mode == NONCE_MODE_NO_SEED_UPDATE))
    {
        // Calculate nonce using SHA-256 (refer to data sheet)
        p_temp = temporary;

        memcpy(p_temp, param->rand_out, NONCE_RSP_SIZE_LONG - ATCA_PACKET_OVERHEAD);
        p_temp += NONCE_RSP_SIZE_LONG - ATCA_PACKET_OVERHEAD;

        memcpy(p_temp, param->num_in, NONCE_NUMIN_SIZE);
        p_temp += NONCE_NUMIN_SIZE;

        *p_temp++ = ATCA_NONCE;
        *p_temp++ = param->mode;
        *p_temp++ = 0x00;

        // Calculate SHA256 to get the nonce
        atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_NONCE, param->temp_key->value);

        // Update TempKey->SourceFlag to 0 (random)
        param->temp_key->source_flag = 0;
    }
    else if (param->mode == NONCE_MODE_PASSTHROUGH)
    {
        // Pass-through mode
        memcpy(param->temp_key->value, param->num_in, NONCE_NUMIN_SIZE_PASSTHROUGH);

        // Update TempKey->SourceFlag to 1 (not random)
        param->temp_key->source_flag = 1;
    }

    // Update TempKey fields
    param->temp_key->key_id = 0;
    param->temp_key->gen_dig_data = 0;
    param->temp_key->no_mac_flag = 0;
    param->temp_key->valid = 1;

    return ATCA_SUCCESS;
}


/** \brief This function generates an SHA-256 digest (MAC) of a key, challenge, and other information.

   The resulting digest will match with the one generated by the device when executing a MAC command.
   The TempKey (if used) should be valid (temp_key.valid = 1) before executing this function.

 * \param[in, out] param pointer to parameter structure
 * \return status of the operation
 */
ATCA_STATUS atcah_mac(struct atca_mac_in_out *param)
{
    uint8_t temporary[ATCA_MSG_SIZE_MAC];
    uint8_t *p_temp;
    struct atca_include_data_in_out include_data;

    // Initialize struct
    include_data.otp = param->otp;
    include_data.sn = param->sn;
    include_data.mode = param->mode;

    // Check parameters
    if (!param->response
        || (param->mode & ~MAC_MODE_MASK)
        || (!(param->mode & MAC_MODE_BLOCK1_TEMPKEY) && !param->key)
        || (!(param->mode & MAC_MODE_BLOCK2_TEMPKEY) && !param->challenge)
        || ((param->mode & MAC_MODE_USE_TEMPKEY_MASK) && !param->temp_key)
        || (((param->mode & MAC_MODE_INCLUDE_OTP_64) || (param->mode & MAC_MODE_INCLUDE_OTP_88)) && !param->otp)
        || ((param->mode & MAC_MODE_INCLUDE_SN) && !param->sn)
        )
        return ATCA_BAD_PARAM;

    // Check TempKey fields validity if TempKey is used
    if (((param->mode & MAC_MODE_USE_TEMPKEY_MASK) != 0)
        // TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
        && (param->temp_key->no_mac_flag || (param->temp_key->valid != 1)
        // If either mode parameter bit 0 or bit 1 are set, mode parameter bit 2 must match temp_key.source_flag.
        // Logical not (!) is used to evaluate the expression to TRUE / FALSE first before comparison (!=).
            || (!(param->mode & MAC_MODE_SOURCE_FLAG_MATCH) != !(param->temp_key->source_flag)))
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }

    // Start calculation
    p_temp = temporary;

    // (1) first 32 bytes
    memcpy(p_temp, param->mode & MAC_MODE_BLOCK1_TEMPKEY ? param->temp_key->value : param->key, ATCA_KEY_SIZE);                // use Key[KeyID]
    p_temp += ATCA_KEY_SIZE;

    // (2) second 32 bytes
    memcpy(p_temp, param->mode & MAC_MODE_BLOCK2_TEMPKEY ? param->temp_key->value : param->challenge, ATCA_KEY_SIZE);          // use Key[KeyID]
    p_temp += ATCA_KEY_SIZE;

    // (3) 1 byte opcode
    *p_temp++ = ATCA_MAC;

    // (4) 1 byte mode parameter
    *p_temp++ = param->mode;

    // (5) 2 bytes keyID
    *p_temp++ = param->key_id & 0xFF;
    *p_temp++ = (param->key_id >> 8) & 0xFF;

    include_data.p_temp = p_temp;
    atcah_include_data(&include_data);

    // Calculate SHA256 to get the MAC digest
    atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_MAC, param->response);

    // Update TempKey fields
    if (param->temp_key)
        param->temp_key->valid = 0;

    return ATCA_SUCCESS;
}


/** \brief
 * \param[inout] param  Input and output parameters
 * \return status of the operation
 */
ATCA_STATUS atcah_check_mac(struct atca_check_mac_in_out *param)
{
    uint8_t msg[ATCA_MSG_SIZE_MAC];
    bool is_temp_key_req = false;

    // Check parameters
    if (param == NULL || param->other_data == NULL || param->sn == NULL || param->client_resp == NULL)
        return ATCA_BAD_PARAM;

    if ((param->mode & CHECKMAC_MODE_BLOCK1_TEMPKEY) || (param->mode & CHECKMAC_MODE_BLOCK2_TEMPKEY))
        is_temp_key_req = true;  // Message uses TempKey
    else if ((param->mode == 0x01 || param->mode == 0x05) && param->target_key != NULL)
        is_temp_key_req = true;  // CheckMac copy will be performed

    if (is_temp_key_req && param->temp_key == NULL)
        return ATCA_BAD_PARAM;
    if (!(param->mode & CHECKMAC_MODE_BLOCK1_TEMPKEY) && param->slot_key == NULL)
        return ATCA_BAD_PARAM;
    if (!(param->mode & CHECKMAC_MODE_BLOCK2_TEMPKEY) && param->client_chal == NULL)
        return ATCA_BAD_PARAM;
    if ((param->mode & CHECKMAC_MODE_INCLUDE_OTP_64) && param->otp == NULL)
        return ATCA_BAD_PARAM;

    if ((param->mode & CHECKMAC_MODE_BLOCK1_TEMPKEY) || (param->mode & CHECKMAC_MODE_BLOCK2_TEMPKEY))
    {
        // This will use TempKey in message, check validity
        if (!param->temp_key->valid)
            return ATCA_EXECUTION_ERROR;  // TempKey is not valid
        if (((param->mode >> 2) & 0x01) != param->temp_key->source_flag)
            return ATCA_EXECUTION_ERROR;  // TempKey SourceFlag doesn't match bit 2 of the mode
    }

    // Build the message
    memset(msg, 0, sizeof(msg));
    if (param->mode & CHECKMAC_MODE_BLOCK1_TEMPKEY)
        memcpy(&msg[0], param->temp_key->value, 32);
    else
        memcpy(&msg[0], param->slot_key, 32);
    if (param->mode & CHECKMAC_MODE_BLOCK2_TEMPKEY)
        memcpy(&msg[32], param->temp_key->value, 32);
    else
        memcpy(&msg[32], param->client_chal, 32);
    memcpy(&msg[64], &param->other_data[0], 4);
    if (param->mode & CHECKMAC_MODE_INCLUDE_OTP_64)
        memcpy(&msg[68], param->otp, 8);
    memcpy(&msg[76], &param->other_data[4], 3);
    msg[79] = param->sn[8];
    memcpy(&msg[80], &param->other_data[7], 4);
    memcpy(&msg[84], &param->sn[0], 2);
    memcpy(&msg[86], &param->other_data[11], 2);

    // Calculate the client response
    atcac_sw_sha2_256(msg, sizeof(msg), param->client_resp);

    // Update TempKey fields
    if ((param->mode == 0x01 || param->mode == 0x05) && param->target_key != NULL)
    {
        // CheckMac Copy will be performed
        memcpy(param->temp_key->value, param->target_key, ATCA_KEY_SIZE);
        param->temp_key->gen_dig_data = 0;
        param->temp_key->source_flag = 1;
        param->temp_key->valid = 1;
    }

    return ATCA_SUCCESS;
}


/** \brief This function generates an HMAC / SHA-256 hash of a key and other information.

   The resulting hash will match with the one generated in the device by an HMAC command.
   The TempKey has to be valid (temp_key.valid = 1) before executing this function.

 * \param[in, out] param pointer to parameter structure
 * \return status of the operation
 */
ATCA_STATUS atcah_hmac(struct atca_hmac_in_out *param)
{
    // Local Variables
    struct atca_include_data_in_out include_data;
    uint8_t temporary[HMAC_BLOCK_SIZE + ATCA_MSG_SIZE_HMAC];
    uint8_t i = 0;
    uint8_t *p_temp = NULL;

    // Check parameters
    if (!param->response || !param->key || !param->temp_key
        || (param->mode & ~HMAC_MODE_MASK)
        || (((param->mode & MAC_MODE_INCLUDE_OTP_64) || (param->mode & MAC_MODE_INCLUDE_OTP_88)) && !param->otp)
        || (!param->sn)
        )
        return ATCA_BAD_PARAM;

    // Check TempKey fields validity (TempKey is always used)
    if ( // TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
        param->temp_key->no_mac_flag || (param->temp_key->valid != 1)
        // The mode parameter bit 2 must match temp_key.source_flag.
        // Logical not (!) is used to evaluate the expression to TRUE / FALSE first before comparison (!=).
        || (!(param->mode & MAC_MODE_SOURCE_FLAG_MATCH) != !(param->temp_key->source_flag))
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }

    // Start first calculation (inner)
    p_temp = temporary;

    // XOR key with ipad
    for (i = 0; i < ATCA_KEY_SIZE; i++)
        *p_temp++ = param->key[i] ^ 0x36;

    // zero pad key out to block size
    // Refer to fips-198 , length Key = 32 bytes, Block size = 512 bits = 64 bytes.
    // So the Key must be padded with zeros.
    memset(p_temp, 0x36, HMAC_BLOCK_SIZE - ATCA_KEY_SIZE);
    p_temp += HMAC_BLOCK_SIZE - ATCA_KEY_SIZE;

    // Next append the stream of data 'text'
    memset(p_temp, 0, ATCA_KEY_SIZE);
    p_temp += ATCA_KEY_SIZE;

    memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);
    p_temp += ATCA_KEY_SIZE;

    *p_temp++ = ATCA_HMAC;
    *p_temp++ = param->mode;
    *p_temp++ = (uint8_t)(param->key_id >> 0);
    *p_temp++ = (uint8_t)(param->key_id >> 8);

    include_data.otp = param->otp;
    include_data.sn = param->sn;
    include_data.mode = param->mode;
    include_data.p_temp = p_temp;
    atcah_include_data(&include_data);

    // Calculate SHA256
    // H((K0^ipad):text), use param.response for temporary storage
    atcac_sw_sha2_256(temporary, HMAC_BLOCK_SIZE + ATCA_MSG_SIZE_HMAC, param->response);


    // Start second calculation (outer)
    p_temp = temporary;

    // XOR K0 with opad
    for (i = 0; i < ATCA_KEY_SIZE; i++)
        *p_temp++ = param->key[i] ^ 0x5C;

    // zero pad key out to block size
    // Refer to fips-198 , length Key = 32 bytes, Block size = 512 bits = 64 bytes.
    // So the Key must be padded with zeros.
    memset(p_temp, 0x5C, HMAC_BLOCK_SIZE - ATCA_KEY_SIZE);
    p_temp += HMAC_BLOCK_SIZE - ATCA_KEY_SIZE;

    // Append result from last calculation H((K0 ^ ipad) || text)
    memcpy(p_temp, param->response, ATCA_SHA_DIGEST_SIZE);
    p_temp += ATCA_SHA_DIGEST_SIZE;

    // Calculate SHA256 to get the resulting HMAC
    atcac_sw_sha2_256(temporary, HMAC_BLOCK_SIZE + ATCA_SHA_DIGEST_SIZE, param->response);

    // Update TempKey fields
    param->temp_key->valid = 0;

    return ATCA_SUCCESS;
}


/** \brief This function combines the current TempKey with a stored value.

   The stored value can be a data slot, OTP page, configuration zone, or hardware transport key.
   The TempKey generated by this function will match with the TempKey in the device generated
   when executing a GenDig command.
   The TempKey should be valid (temp_key.valid = 1) before executing this function.
   To use this function, an application first sends a GenDig command with a chosen stored value to the device.
   This stored value must be known by the application and is passed to this GenDig calculation function.
   The function calculates a new TempKey and returns it.

 * \param[in, out] param pointer to parameter structure
 * \return status of the operation
 */
ATCA_STATUS atcah_gen_dig(struct atca_gen_dig_in_out *param)
{
    uint8_t temporary[ATCA_MSG_SIZE_GEN_DIG];
    uint8_t *p_temp;

    // Check parameters
    if (param->sn == NULL || param->temp_key == NULL)
        return ATCA_BAD_PARAM;
    if (param->zone != GENDIG_ZONE_SHARED_NONCE && param->stored_value == NULL)
        return ATCA_BAD_PARAM;  // Stored value can only be null with the shared_nonce mode
    if ((param->zone == GENDIG_ZONE_SHARED_NONCE || (param->zone == GENDIG_ZONE_DATA && param->is_key_nomac)) && param->other_data == NULL)
        return ATCA_BAD_PARAM;  // Other data is required in these cases
    if (param->zone > 5)
        return ATCA_BAD_PARAM;  // Unknown zone

    // Start calculation
    p_temp = temporary;

    // (1) 32 bytes inputKey
    if (param->zone == GENDIG_ZONE_SHARED_NONCE && param->key_id & 0x8000)
        memcpy(p_temp, param->other_data, ATCA_KEY_SIZE);
    else
        memcpy(p_temp, param->stored_value, ATCA_KEY_SIZE);
    p_temp += ATCA_KEY_SIZE;

    if (param->zone == GENDIG_ZONE_DATA && param->is_key_nomac)
    {
        // If a key has the SlotConfig.NoMac bit set, then opcode and parameters come from OtherData
        memcpy(p_temp, param->other_data, 4);
        p_temp += 4;
    }
    else
    {
        // (2) 1 byte Opcode
        *p_temp++ = ATCA_GENDIG;

        // (3) 1 byte Param1 (zone)
        *p_temp++ = param->zone;

        // (4) 2 bytes Param2 (keyID)
        *p_temp++ = (uint8_t)(param->key_id & 0xFF);
        *p_temp++ = (uint8_t)(param->key_id >> 8);
    }

    // (5) 1 byte SN[8]
    *p_temp++ = param->sn[8];

    // (6) 2 bytes SN[0:1]
    *p_temp++ = param->sn[0];
    *p_temp++ = param->sn[1];

    // (7) 25 zeros
    memset(p_temp, 0, ATCA_GENDIG_ZEROS_SIZE);
    p_temp += ATCA_GENDIG_ZEROS_SIZE;

    if (param->zone == GENDIG_ZONE_SHARED_NONCE && !(param->key_id & 0x8000))
        memcpy(p_temp, param->other_data, ATCA_KEY_SIZE);       // (8) 32 bytes OtherData
    else
        memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);  // (8) 32 bytes TempKey

    // Calculate SHA256 to get the new TempKey
    atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_GEN_DIG, param->temp_key->value);

    // Update TempKey fields
    param->temp_key->valid = 1;

    if ((param->zone == GENDIG_ZONE_DATA) && (param->key_id <= 15))
    {
        param->temp_key->gen_dig_data = 1;
        param->temp_key->key_id = (param->key_id & 0xF);    // mask lower 4-bit only
    }
    else
    {
        param->temp_key->gen_dig_data = 0;
        param->temp_key->key_id = 0;
    }

    return ATCA_SUCCESS;
}

/** \brief This function combines the session key with a plain text.
 * \param[in, out] param pointer to parameter structure
 * \return status of the operation
 */
ATCA_STATUS atcah_gen_mac(struct atca_gen_dig_in_out *param)
{
    uint8_t temporary[ATCA_MSG_SIZE_GEN_DIG];
    uint8_t *p_temp;

    // Check parameters
    if (!param->stored_value || !param->temp_key)
        return ATCA_BAD_PARAM;

    // Check TempKey fields validity (TempKey is always used)
    if ( // TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
        param->temp_key->no_mac_flag || (param->temp_key->valid != 1)
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }

    // Start calculation
    p_temp = temporary;

    // (1) 32 bytes SessionKey
    //     (Config[KeyID] or OTP[KeyID] or Data.slot[KeyID] or TransportKey[KeyID])
    memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);
    p_temp += ATCA_KEY_SIZE;

    // (2) 1 byte Opcode
    *p_temp++ = ATCA_WRITE;

    // (3) 1 byte Param1 (zone)
    *p_temp++ = param->zone;

    // (4) 2 bytes Param2 (keyID)
    *p_temp++ = param->key_id & 0xFF;
    *p_temp++ = (param->key_id >> 8) & 0xFF;

    // (5) 1 byte SN[8]
    *p_temp++ = param->sn[8];

    // (6) 2 bytes SN[0:1]
    *p_temp++ = param->sn[0];
    *p_temp++ = param->sn[1];

    // (7) 25 zeros
    memset(p_temp, 0, ATCA_GENDIG_ZEROS_SIZE);
    p_temp += ATCA_GENDIG_ZEROS_SIZE;

    // (8) 32 bytes PlainText
    memcpy(p_temp, param->stored_value, ATCA_KEY_SIZE);

    // Calculate SHA256 to get the new TempKey
    atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_GEN_DIG, param->temp_key->value);

    // Update TempKey fields
    param->temp_key->valid = 1;

    if ((param->zone == GENDIG_ZONE_DATA) && (param->key_id <= 15))
    {
        param->temp_key->gen_dig_data = 1;
        param->temp_key->key_id = (param->key_id & 0xF);    // mask lower 4-bit only
    }
    else
    {
        param->temp_key->gen_dig_data = 0;
        param->temp_key->key_id = 0;
    }

    return ATCA_SUCCESS;
}

/** \brief This function calculates the input MAC for the Write command.

   The Write command will need an input MAC if SlotConfig.WriteConfig.Encrypt is set.

 * \param[in, out] param pointer to parameter structure
 * \return status of the operation
 */
ATCA_STATUS atcah_write_auth_mac(struct atca_write_mac_in_out *param)
{
    uint8_t mac_input[ATCA_MSG_SIZE_ENCRYPT_MAC];
    uint8_t i;
    uint8_t *p_temp;

    // Check parameters
    if (!param->input_data || !param->temp_key)
        return ATCA_BAD_PARAM;

    // Check TempKey fields validity (TempKey is always used)
    if ( // TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
        param->temp_key->no_mac_flag || (param->temp_key->valid != 1)
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }
    // Encrypt by XOR-ing Data with the TempKey
    for (i = 0; i < 32; i++)
        param->encrypted_data[i] = param->input_data[i] ^ param->temp_key->value[i];

    // If the pointer *mac is provided by the caller then calculate input MAC
    if (param->auth_mac)
    {
        // Start calculation
        p_temp = mac_input;

        // (1) 32 bytes TempKey
        memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);
        p_temp += ATCA_KEY_SIZE;

        // (2) 1 byte Opcode
        *p_temp++ = ATCA_WRITE;

        // (3) 1 byte Param1 (zone)
        *p_temp++ = param->zone;

        // (4) 2 bytes Param2 (keyID)
        *p_temp++ = param->key_id & 0xFF;
        *p_temp++ = (param->key_id >> 8) & 0xFF;

        // (5) 1 byte SN[8]
        *p_temp++ = param->sn[8];

        // (6) 2 bytes SN[0:1]
        *p_temp++ = param->sn[0];
        *p_temp++ = param->sn[1];

        // (7) 25 zeros
        memset(p_temp, 0, ATCA_WRITE_MAC_ZEROS_SIZE);
        p_temp += ATCA_WRITE_MAC_ZEROS_SIZE;

        // (8) 32 bytes PlainText
        memcpy(p_temp, param->input_data, ATCA_KEY_SIZE);

        // Calculate SHA256 to get MAC
        atcac_sw_sha2_256(mac_input, sizeof(mac_input), param->auth_mac);
    }

    return ATCA_SUCCESS;
}


/** \brief This function encrypts 32-byte plain text data to be written using Write opcode, and optionally calculates input MAC.

   To use this function, first the nonce must be valid and synchronized between device and application.
   The application sends a GenDig command to the device, using a parent key. If the Data zone has been locked, this is
   specified by SlotConfig.WriteKey. The device updates its TempKey when executing the command.
   The application then updates its own TempKey using the GenDig calculation function, using the same key.
   The application passes the plain text data to the encryption function.\n
   If input MAC is needed the application must pass a valid pointer to buffer in the "mac" command parameter.
   If input MAC is not needed the application can pass a NULL pointer in the "mac" command parameter.
   The function encrypts the data and optionally calculates the input MAC, and returns it to the application.
   Using these encrypted data and the input MAC, the application sends a Write command to the Device. The device
   validates the MAC, then decrypts and writes the data.\n
   The encryption function does not check whether the TempKey has been generated by the correct ParentKey for the
   corresponding zone. Therefore, to get a correct result after the Data and OTP zones have been locked, the application
   has to make sure that prior GenDig calculation was done using the correct ParentKey.

 * \param[in, out] param pointer to parameter structure
 * \return status of the operation
 */
/*
   // Not sure this one is being used
   ATCA_STATUS atcah_encrypt(struct atca_encrypt_in_out *param)
   {
    uint8_t temporary[ATCA_MSG_SIZE_ENCRYPT_MAC];
    uint8_t i;
    uint8_t *p_temp;

    // Check parameters
    if (!param->crypto_data || !param->temp_key || (param->zone & ~WRITE_ZONE_MASK))
        return ATCA_BAD_PARAM;

    // Check TempKey fields validity, and illegal address
    // Note that if temp_key.key_id is not checked,
    //   we cannot make sure if the key used in previous GenDig IS equal to
    //   the key pointed by SlotConfig.WriteKey in the device.
    if (    // TempKey.CheckFlag must be 0
        param->temp_key->no_mac_flag
            // TempKey.Valid must be 1
 || (param->temp_key->valid != 1)
            // TempKey.GenData must be 1
 || (param->temp_key->gen_dig_data != 1)
            // TempKey.SourceFlag must be 0 (random)
 || param->temp_key->source_flag
            // Illegal address
 || (param->address & ~ATCA_ADDRESS_MASK)
        ) {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }

    // If the pointer *mac is provided by the caller then calculate input MAC
    if (param->mac) {
        // Start calculation
        p_temp = temporary;

        // (1) 32 bytes parent key
        memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);
        p_temp += ATCA_KEY_SIZE;

        // (2) 1 byte Opcode
 ||*p_temp++ = ATCA_WRITE;

        // (3) 1 byte Param1 (zone)
 ||*p_temp++ = param->zone;

        // (4) 2 bytes Param2 (address)
 ||*p_temp++ = param->address & 0xFF;
 ||*p_temp++ = (param->address >> 8) & 0xFF;

        // (5) 1 byte SN[8] = 0xEE
 ||*p_temp++ = ATCA_SN_8;

        // (6) 2 bytes SN[0:1] = 0x0123
 ||*p_temp++ = ATCA_SN_0;
 ||*p_temp++ = ATCA_SN_1;

        // (7) 25 zeros
        memset(p_temp, 0, ATCA_GENDIG_ZEROS_SIZE);
        p_temp += ATCA_GENDIG_ZEROS_SIZE;

        // (8) 32 bytes data
        memcpy(p_temp, param->crypto_data, ATCA_KEY_SIZE);

        // Calculate SHA256 to get the input MAC
        atcah_sha256(ATCA_MSG_SIZE_ENCRYPT_MAC, temporary, param->mac);
    }

    // Encrypt by XOR-ing Data with the TempKey
    for (i = 0; i < ATCA_KEY_SIZE; i++)
        param->crypto_data[i] ^= param->temp_key->value[i];

    // Update TempKey fields
    param->temp_key->valid = 0;

    return ATCA_SUCCESS;
   }
 */


