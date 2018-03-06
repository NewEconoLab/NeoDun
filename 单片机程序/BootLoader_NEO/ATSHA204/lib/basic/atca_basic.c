/**
 * \file
 * \brief CryptoAuthLib Basic API methods.  These methods provide a simpler way to access the core crypto
 * methods.  Their design center is around the most common modes and functions of each command
 * rather than the complete implementation of each possible feature of the chip.  If you need a feature
 * not supplied in the Basic API, you can achieve the feature through the datasheet level command
 * supplied through the ATCADevice and ATCACommand object.
 *
 * One primary assumption to all atcab_ routines is that the caller manages the wake/sleep/idle bracket
 * so prior to calling the atcab_ routine, the chip should be awake; the routine will manage wake/sleep/idle
 * within the function and leave the chip awake upon return.
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


#include "atca_basic.h"
#include "host/atca_host.h"

char atca_version[] = { "20170701" };  // change for each release, yyyymmdd

/** \brief returns a version string for the CryptoAuthLib release.
 *  The format of the version string returned is "yyyymmdd"
 * \param[out] verstr ptr to space to receive version string
 * \return ATCA_STATUS
 */

ATCA_STATUS atcab_version(char *verstr)
{
    strcpy(verstr, atca_version);
    return ATCA_SUCCESS;
}

/** \brief wakeup the CryptoAuth device
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_wakeup(void)
{
    return atwake();
}

/** \brief idle the CryptoAuth device
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_idle(void)
{
    return atidle();
}

/** \brief invoke sleep on the CryptoAuth device
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_sleep(void)
{
    return atsleep();
}


/** \brief common cleanup code which idles the device after any operation
 *  \return ATCA_STATUS
 */
static ATCA_STATUS _atcab_exit(void)
{
    return atcab_idle();
}


/** \brief get the device revision information
 *  \param[out] revision - 4-byte storage for receiving the revision number from the device
 *  \return ATCA_STATUS
 */

ATCA_STATUS atcab_info(uint8_t *revision)
{
    ATCAPacket packet;
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint32_t execution_time;

    // build an info command
    packet.param1 = INFO_MODE_REVISION;
    packet.param2 = 0;

    do
    {
        // Check the inputs
        if (revision == NULL)
        {
            status = ATCA_BAD_PARAM;
            BREAK(status, "atcab_info: Null inputs");
        }
        if ( (status = atInfo( &packet)) != ATCA_SUCCESS)
            BREAK(status, "Failed to construct Info command");

        execution_time = atGetExecTime( CMD_INFO);

        if ( (status = atcab_wakeup()) != ATCA_SUCCESS)
            BREAK(status, "Failed to wakeup");

        // send the command
        if ( (status = atsend( (uint8_t*)&packet, packet.txsize)) != ATCA_SUCCESS)
            BREAK(status, "Failed to send Info command");

        // delay the appropriate amount of time for command to execute
        atca_delay_ms(execution_time);

        // receive the response
        if ( (status = atreceive( &(packet.data[0]), &(packet.rxsize) )) != ATCA_SUCCESS)
            BREAK(status, "Failed to receive Info command");

        // Check response size
        if (packet.rxsize < 4)
        {
            if (packet.rxsize > 0)
                status = ATCA_RX_FAIL;
            else
                status = ATCA_RX_NO_RESPONSE;
            BREAK(status, "Info command returned error code or no resonse");
        }

        if ( (status = isATCAError(packet.data)) != ATCA_SUCCESS)
            BREAK(status, "Failed to construct Info command");

        memcpy(revision, &packet.data[1], 4);    // don't include the receive length, only payload
    }
    while (0);

    if (status != ATCA_COMM_FAIL)     // don't keep shoving more stuff at the chip if there's something wrong with comm
        _atcab_exit();

    return status;
}

/** \brief Get a 32 byte random number from the CryptoAuth device
 *	\param[out] rand_out ptr to 32 bytes of storage for random number
 *	\return status of the operation
 */
ATCA_STATUS atcab_random(uint8_t *rand_out)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    ATCAPacket packet;
    uint16_t execution_time = 0;

    do
    {
        // build an random command
        packet.param1 = RANDOM_SEED_UPDATE;
        packet.param2 = 0x0000;
        if ( (status = atRandom( &packet)) != ATCA_SUCCESS)
            break;

        execution_time = atGetExecTime( CMD_RANDOM);

        if ( (status = atcab_wakeup()) != ATCA_SUCCESS)
            break;

        // send the command
        if ( (status = atsend( (uint8_t*)&packet, packet.txsize)) != ATCA_SUCCESS)
            break;

        // delay the appropriate amount of time for command to execute
        atca_delay_ms(execution_time);

        // receive the response
        if ( (status = atreceive( packet.data, &packet.rxsize)) != ATCA_SUCCESS)
            break;

        // Check response size
        if (packet.rxsize < 4)
        {
            if (packet.rxsize > 0)
                status = ATCA_RX_FAIL;
            else
                status = ATCA_RX_NO_RESPONSE;
            break;
        }

        if ( (status = isATCAError(packet.data)) != ATCA_SUCCESS)
            break;

        if (packet.rxsize < packet.data[ATCA_COUNT_IDX] || packet.data[ATCA_COUNT_IDX] != RANDOM_RSP_SIZE)
        {
            status = ATCA_RX_FAIL;
            break;
        }

        if (rand_out)
            memcpy(rand_out, &packet.data[ATCA_RSP_DATA_IDX], RANDOM_NUM_SIZE);
    }
    while (0);

    _atcab_exit();
    return status;
}

/** \brief Execute a pass-through Nonce command to initialize TempKey to the specified value
 *  \param[in] tempkey - pointer to 32 bytes of data which will be used to initialize TempKey
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_nonce(const uint8_t *tempkey)
{
    return atcab_challenge(tempkey);
}

/** \brief Initialize TempKey with a random Nonce
 *  \param[in] seed - pointer to 20 bytes of data which will be used to calculate TempKey
 *  \param[out] rand_out - pointer to 32 bytes of data that is the output of the Nonce command
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_nonce_rand(const uint8_t *seed, uint8_t* rand_out)
{
    return atcab_challenge_seed_update(seed, rand_out);
}

/** \brief send a challenge to the device (a pass-through nonce)
 *  \param[in] challenge - pointer to 32 bytes of data which will be sent as the challenge
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_challenge(const uint8_t *challenge)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    ATCAPacket packet;
    uint16_t execution_time = 0;

    do
    {
        // Verify the inputs
        if (challenge == NULL)
        {
            status = ATCA_BAD_PARAM;
            break;
        }

        // build a nonce command (pass through mode)
        packet.param1 = NONCE_MODE_PASSTHROUGH;
        packet.param2 = 0x0000;
        memcpy(packet.data, challenge, 32);

        if ((status = atNonce( &packet)) != ATCA_SUCCESS)
            break;

        execution_time = atGetExecTime( CMD_NONCE);

        if ((status = atcab_wakeup()) != ATCA_SUCCESS)
            break;

        // send the command
        if ((status = atsend( (uint8_t*)&packet, packet.txsize)) != ATCA_SUCCESS)
            break;

        // delay the appropriate amount of time for command to execute
        atca_delay_ms(execution_time);

        // receive the response
        if ((status = atreceive( packet.data, &(packet.rxsize))) != ATCA_SUCCESS)
            break;

        // Check response size
        if (packet.rxsize < 4)
        {
            if (packet.rxsize > 0)
                status = ATCA_RX_FAIL;
            else
                status = ATCA_RX_NO_RESPONSE;
            break;
        }

        if ( (status = isATCAError(packet.data)) != ATCA_SUCCESS)
            break;

    }
    while (0);

    _atcab_exit();
    return status;
}

/** \brief send a challenge to the device (a seed update nonce)
 *  \param[in] seed - pointer to 32 bytes of data which will be sent as the challenge
 *  \param[out] rand_out - points to space to receive random number
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_challenge_seed_update(const uint8_t *seed, uint8_t* rand_out)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    ATCAPacket packet;
    uint16_t execution_time = 0;

    do
    {
        // Verify the inputs
        if (seed == NULL || rand_out == NULL)
        {
            status = ATCA_BAD_PARAM;
            break;
        }

        // build a nonce command (pass through mode)
        packet.param1 = NONCE_MODE_SEED_UPDATE;
        packet.param2 = 0x0000;
        memcpy(packet.data, seed, 20);

        if ((status = atNonce( &packet)) != ATCA_SUCCESS)
            break;

        execution_time = atGetExecTime( CMD_NONCE);

        if ((status = atcab_wakeup()) != ATCA_SUCCESS)
            break;

        // send the command
        if ( (status = atsend( (uint8_t*)&packet, packet.txsize)) != ATCA_SUCCESS)
            break;

        // delay the appropriate amount of time for command to execute
        atca_delay_ms(execution_time);

        // receive the response
        if ((status = atreceive( packet.data, &(packet.rxsize))) != ATCA_SUCCESS)
            break;

        // Check response size
        if (packet.rxsize < 4)
        {
            if (packet.rxsize > 0)
                status = ATCA_RX_FAIL;
            else
                status = ATCA_RX_NO_RESPONSE;
            break;
        }

        if ((status = isATCAError(packet.data)) != ATCA_SUCCESS)
            break;

        memcpy(&rand_out[0], &packet.data[ATCA_RSP_DATA_IDX], 32);

    }
    while (0);

    _atcab_exit();
    return status;
}

/** \brief The Nonce command generates a nonce for use by subsequent commands.
 *
 * \param[in]  mode      Controls the mechanism of the internal RNG and seed
 *                       update.
 * \param[in]  num_in    Input value to either be included in the nonce
 *                       calculation in random modes (20 bytes) or to be
 *                       written to TempKey directly (32 bytes) in
 *                       pass-through mode.
 * \param[out] rand_out  If using a a random mode, the internally generated
 *                       32-byte random number that was used in the nonce
 *                       calculation is returned here.
 *                       Can be NULL if not needed.
 * \return ATCA_SUCCESS
 */
ATCA_STATUS atcab_nonce_base(uint8_t mode, const uint8_t *num_in, uint8_t* rand_out)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    ATCAPacket packet;
    uint16_t execution_time = 0;

    do
    {

        // build a nonce command (pass through mode)
        packet.param1 = mode;
        packet.param2 = 0x0000;
        if (mode == NONCE_MODE_PASSTHROUGH)
            memcpy(packet.data, num_in, 32);
        else
            memcpy(packet.data, num_in, 20);


        if ((status = atNonce( &packet)) != ATCA_SUCCESS)
            break;

        execution_time = atGetExecTime( CMD_NONCE);

        if ((status = atcab_wakeup()) != ATCA_SUCCESS)
            break;

        // send the command
        if ((status = atsend( (uint8_t*)&packet, packet.txsize)) != ATCA_SUCCESS)
            break;

        // delay the appropriate amount of time for command to execute
        atca_delay_ms(execution_time);

        // receive the response
        if ((status = atreceive( packet.data, &(packet.rxsize))) != ATCA_SUCCESS)
            break;

        // Check response size
        if (packet.rxsize < 4)
        {
            if (packet.rxsize > 0)
                status = ATCA_RX_FAIL;
            else
                status = ATCA_RX_NO_RESPONSE;
            break;
        }

        if ((status = isATCAError(packet.data)) != ATCA_SUCCESS)
            break;

        if ((rand_out != NULL) && (packet.rxsize >= 35))
            memcpy(&rand_out[0], &packet.data[ATCA_RSP_DATA_IDX], 32);

    }
    while (0);

    _atcab_exit();
    return status;
}
/** \brief read the serial number of the device
 *  \param[out] serial_number  pointer to space to receive serial number. This space should be 9 bytes long
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_read_serial_number(uint8_t* serial_number)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t read_buf[ATCA_BLOCK_SIZE];

    if (!serial_number)
        return ATCA_BAD_PARAM;

    do
    {
        if ( (status = atcab_read_zone(ATCA_ZONE_CONFIG, 0, 0, 0, read_buf, ATCA_BLOCK_SIZE)) != ATCA_SUCCESS)
            break;
        memcpy(&serial_number[0], &read_buf[0], 4);
        memcpy(&serial_number[4], &read_buf[8], 5);
    }
    while (0);

    return status;
}

/** \brief Compute the address given the zone, slot, block, and offset
 *  \param[in] zone
 *  \param[in] slot      ---如果操作的不是数据区，它被忽略
 *  \param[in] block     ---如果操作的是数据区，  它被忽略
 *  \param[in] offset
 *  \param[in] addr
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_get_addr(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint16_t* addr)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t memzone = zone & 0x03;

    if (addr == NULL)
        return ATCA_BAD_PARAM;
    if ((memzone != ATCA_ZONE_CONFIG) && (memzone != ATCA_ZONE_DATA) && (memzone != ATCA_ZONE_OTP))
        return ATCA_BAD_PARAM;
    do
    {
        // Initialize the addr to 00
        *addr = 0;
        // Mask the offset
        offset = offset & (uint8_t)0x07;//只取最后3位offset
        if ((memzone == ATCA_ZONE_CONFIG) || (memzone == ATCA_ZONE_OTP))
        {
            *addr = block << 3;
            *addr |= offset;
        }
        else     // ATCA_ZONE_DATA
        {
            *addr = slot << 3;  //移入block，相当于Slot号，0-15
            *addr  |= offset;   //或上偏移offset，0-7，以4字节为一个单元
            *addr |= block << 8;//根据优先级，本行等同于 *addr |= (block << 8);block左移8位相当于清零啊，所以在这里它被忽略了。
        }
    }
    while (0);

    return status;
}

/** \brief Gets the size of the specified zone in bytes.
 *
 * \param[in]  zone  Zone to get size information from. Config(0), OTP(1), or
 *                   Data(2) which requires a slot.
 * \param[in]  slot  If zone is Data(2), the slot to query for size.
 * \param[out] size  Zone size is returned here.
 *
 * \return ATCA_SUCCESS on success
 */
ATCA_STATUS atcab_get_zone_size(uint8_t zone, uint16_t slot, size_t* size)
{
    ATCA_STATUS status = ATCA_SUCCESS;

    if (size == NULL)
        return ATCA_BAD_PARAM;

	switch (zone)
	{
	case ATCA_ZONE_CONFIG: *size = 88; break;
	case ATCA_ZONE_OTP:    *size = 64; break;
	case ATCA_ZONE_DATA:   *size = 32; break;
	default: status = ATCA_BAD_PARAM; break;
	}
  
    return status;
}

/** \brief Query to see if the specified zone is locked
 *  \param[in]  zone      The zone to query for locked (use LOCK_ZONE_CONFIG or LOCK_ZONE_DATA)
 *  \param[out] is_locked  true if the specified zone is locked
 *  \return ATCA_SUCCESS
 */
ATCA_STATUS atcab_is_locked(uint8_t zone, bool *is_locked)
{
    ATCA_STATUS ret = ATCA_GEN_FAIL;
    uint8_t data[ATCA_WORD_SIZE];

    do
    {
        if (is_locked == NULL)
            return ATCA_BAD_PARAM;

        // Read the word with the lock bytes (UserExtra, Selector, LockValue, LockConfig) (config block = 2, word offset = 5)
        if ( (ret = atcab_read_zone(ATCA_ZONE_CONFIG, 0, 2 /*block*/, 5 /*offset*/, data, ATCA_WORD_SIZE)) != ATCA_SUCCESS)
            break;

        // Determine the index into the word_data based on the zone we are querying for
        switch (zone)
        {
        case LOCK_ZONE_CONFIG: *is_locked = (data[3] != 0x55); break;
        case LOCK_ZONE_DATA:   *is_locked = (data[2] != 0x55); break;
        default: ret = ATCA_BAD_PARAM; break;
        }
    }
    while (0);

    return ret;
}

/**
 * \brief The Write command writes either one four byte word or an 8-word block of 32 bytes to one of the EEPROM
 * zones on the device. Depending upon the value of the WriteConfig byte for this slot, the data may be required
 * to be encrypted by the system prior to being sent to the device. This command cannot be used to write slots
 * configured as ECC private keys.
 *
 * \param[in] zone     Zone/Param1 for the write command.
 * \param[in] address  Addres/Param2 for the write command.
 * \param[in] value    Plain-text data to be written or cipher-text for encrypted writes. 32 or 4 bytes depending
 *                     on bit 7 in the zone.
 * \param[in] mac      MAC required for encrypted writes (32 bytes). Set to NULL if not required.
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS atcab_write(uint8_t zone, uint16_t address, const uint8_t *value, const uint8_t *mac)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    ATCAPacket packet;
    uint16_t execution_time = 0;

    if (value == NULL)
        return ATCA_BAD_PARAM;

    do
    {
        // Build the write command
        packet.param1 = zone;
        packet.param2 = address;
        if (zone & ATCA_ZONE_READWRITE_32)
        {
            // 32-byte write
            memcpy(packet.data, value, 32);
            // Only 32-byte writes can have a MAC
            if (mac)
                memcpy(&packet.data[32], mac, 32);
        }
        else
        {
            // 4-byte write
            memcpy(packet.data, value, 4);
        } if ((status = atWrite( &packet, mac && (zone & ATCA_ZONE_READWRITE_32))) != ATCA_SUCCESS)
            break;

        execution_time = atGetExecTime( CMD_WRITEMEM);

        if ( (status = atcab_wakeup()) != ATCA_SUCCESS)
            break;

        // send the command
        if ( (status = atsend( (uint8_t*)&packet, packet.txsize)) != ATCA_SUCCESS)
            break;

        // delay the appropriate amount of time for command to execute
        atca_delay_ms(execution_time);

        // receive the response
        if ((status = atreceive( packet.data, &(packet.rxsize))) != ATCA_SUCCESS)
            break;

        // Check response size
        if (packet.rxsize < 4)
        {
            if (packet.rxsize > 0)
                status = ATCA_RX_FAIL;
            else
                status = ATCA_RX_NO_RESPONSE;
            break;
        }

        status = isATCAError(packet.data);

    }
    while (0);

    _atcab_exit();
    return status;
}

/** \brief Write either 4 or 32 bytes of data into a device zone.
 *
 *  See ECC108A datasheet, datazone address values, table 9-8
 *
 *  \param[in] zone    Device zone to write to (0=config, 1=OTP, 2=data).
 *  \param[in] slot    If writing to the data zone, whit is the slot to write to, otherwise it should be 0.
 *  \param[in] block   32-byte block to write to.
 *  \param[in] offset  4-byte word within the specified block to write to. If performing a 32-byte write, this should
 *                     be 0.
 *  \param[in] data    Data to be written.
 *  \param[in] len     Number of bytes to be written. Must be either 4 or 32.
 *  \return ATCA_SUCCESS on success
 */
 /** \brief Write either 4 or 32 bytes of data into a device zone.
 *
 *  See ECC108A datasheet, datazone address values, table 9-8
 *
 *  \param[in] zone    Device zone to write to (0=config, 1=OTP, 2=data).
 *  \param[in] slot    If writing to the data zone, whit is the slot to write to, otherwise it should be 0.对数据区有效，表示0-15的slot号，否则置零
 *  \param[in] block   32-byte block to write to.--对于配置区或OTP区有效；对于数据区，它被忽略，置零即可
 *  \param[in] offset  4-byte word within the specified block to write to. If performing a 32-byte write, this should
 *                     be 0.  32字节写置零，4字节写按需设置。
 *  \param[in] data    Data to be written.
 *  \param[in] len     Number of bytes to be written. Must be either 4 or 32.
 *  \return ATCA_SUCCESS on success
 注：数据区未锁定之前只能进行32字节的写操作，锁定后遵循config设置进行操作
 */
ATCA_STATUS atcab_write_zone(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, const uint8_t *data, uint8_t len)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint16_t addr;

    // Check the input parameters
    if (data == NULL)
        return ATCA_BAD_PARAM;

    if (len != 4 && len != 32)
        return ATCA_BAD_PARAM;

    do
    {
        // The get address function checks the remaining variables
        if ( (status = atcab_get_addr(zone, slot, block, offset, &addr)) != ATCA_SUCCESS)
            break;

        // If there are 32 bytes to write, then xor the bit into the mode
        if (len == ATCA_BLOCK_SIZE)
            zone = zone | ATCA_ZONE_READWRITE_32;

        status = atcab_write(zone, addr, data, NULL);

    }
    while (0);

    return status;
}

/** \brief read either 4 or 32 bytes of data into given slot
 *
 *  for 32 byte read, offset is ignored
 *  data receives the contents read from the slot
 *
 *  data zone must be locked and the slot configuration must not be secret for a slot to be successfully read
 *
 *  \param[in] zone
 *  \param[in] slot
 *  \param[in] block
 *  \param[in] offset
 *  \param[in] data
 *  \param[in] len  Must be either 4 or 32
 *  returns ATCA_STATUS
 */
ATCA_STATUS atcab_read_zone(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint8_t *data, uint8_t len)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    ATCAPacket packet;
    uint16_t addr;
    uint16_t execution_time = 0;

    do
    {
        // Check the input parameters
        if (data == NULL)
            return ATCA_BAD_PARAM;

        if (len != 4 && len != 32)
            return ATCA_BAD_PARAM;

        // The get address function checks the remaining variables
        if ( (status = atcab_get_addr(zone, slot, block, offset, &addr)) != ATCA_SUCCESS)
            break;

        // If there are 32 bytes to write, then xor the bit into the mode
        if (len == ATCA_BLOCK_SIZE)
            zone = zone | ATCA_ZONE_READWRITE_32;

        // build a read command
        packet.param1 = zone;
        packet.param2 = addr;

        if ( (status = atRead( &packet)) != ATCA_SUCCESS)
            break;

        execution_time = atGetExecTime( CMD_READMEM);

        if ( (status = atcab_wakeup()) != ATCA_SUCCESS)
            break;

        // send the command
        if ( (status = atsend( (uint8_t*)&packet, packet.txsize)) != ATCA_SUCCESS)
            break;

        // delay the appropriate amount of time for command to execute
        atca_delay_ms(execution_time);

        // receive the response
        if ( (status = atreceive( packet.data, &(packet.rxsize) )) != ATCA_SUCCESS)
            break;

        // Check response size
        if (packet.rxsize < 4)
        {
            if (packet.rxsize > 0)
                status = ATCA_RX_FAIL;
            else
                status = ATCA_RX_NO_RESPONSE;
            break;
        }

        if ( (status = isATCAError(packet.data)) != ATCA_SUCCESS)
            break;

        memcpy(data, &packet.data[1], len);
    }
    while (0);

    _atcab_exit();
    return status;
}

/** \brief Read 32 bytes of data from the given slot.
 *		The function returns clear text bytes. Encrypted bytes are read over the wire, then subsequently decrypted
 *		Data zone must be locked and the slot configuration must be set to encrypted read for the block to be successfully read
 *  \param[in]  key_id    The slot id for the encrypted read
 *  \param[in]  block     The block id in the specified slot --内部是32字节读，必须设为0？
 *  \param[out] data      The 32 bytes of clear text data that was read encrypted from the slot, then decrypted
 *  \param[in]  enckey    The key to encrypt with for writing
 *  \param[in]  enckeyid  The keyid of the parent encryption key
 *  returns ATCA_STATUS
 */
ATCA_STATUS atcab_read_enc(uint16_t key_id, uint8_t block, uint8_t *data, const uint8_t* enckey, const uint16_t enckeyid)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t zone = ATCA_ZONE_DATA | ATCA_ZONE_READWRITE_32;
    atca_nonce_in_out_t nonceParam;
    atca_gen_dig_in_out_t genDigParam;
    atca_temp_key_t tempkey;
    uint8_t sn[32];
    uint8_t numin[NONCE_NUMIN_SIZE] = { 0 };
    uint8_t randout[RANDOM_NUM_SIZE] = { 0 };
    uint8_t other_data[4] = { 0 };
    int i = 0;

    do
    {
        // Verify inputs parameters
        if (data == NULL || enckey == NULL)
        {
            status = ATCA_BAD_PARAM;
            break;
        }

        // Read the device SN
        if ((status = atcab_read_zone(ATCA_ZONE_CONFIG, 0, 0, 0, sn, 32)) != ATCA_SUCCESS)
            break;
        // Make the SN continuous by moving SN[4:8] right after SN[0:3]
        memmove(&sn[4], &sn[8], 5);

        // Send the random Nonce command
        if ((status = atcab_nonce_rand(numin, randout)) != ATCA_SUCCESS)
            BREAK(status, "Nonce failed");

        // Calculate Tempkey
        memset(&tempkey, 0, sizeof(tempkey));
        memset(&nonceParam, 0, sizeof(nonceParam));
        nonceParam.mode = NONCE_MODE_SEED_UPDATE;
        nonceParam.num_in = (uint8_t*)&numin;
        nonceParam.rand_out = (uint8_t*)&randout;
        nonceParam.temp_key = &tempkey;
        if ((status = atcah_nonce(&nonceParam)) != ATCA_SUCCESS)
            BREAK(status, "Calc TempKey failed");

        // Supply OtherData so GenDig behavior is the same for keys with SlotConfig.NoMac set
        other_data[0] = ATCA_GENDIG;
        other_data[1] = GENDIG_ZONE_DATA;
        other_data[2] = (uint8_t)(enckeyid);
        other_data[3] = (uint8_t)(enckeyid >> 8);

        // Send the GenDig command
        if ((status = atcab_gendig(GENDIG_ZONE_DATA, enckeyid, other_data, sizeof(other_data))) != ATCA_SUCCESS)
            BREAK(status, "GenDig failed");

        // Calculate Tempkey
        // NoMac bit isn't being considered here on purpose to remove having to read SlotConfig.
        // OtherData is built to get the same result regardless of the NoMac bit.
        memset(&genDigParam, 0, sizeof(genDigParam));
        genDigParam.key_id = enckeyid;
        genDigParam.is_key_nomac = false;
        genDigParam.sn = sn;
        genDigParam.stored_value = enckey;
        genDigParam.zone = GENDIG_ZONE_DATA;
        genDigParam.other_data = other_data;
        genDigParam.temp_key = &tempkey;
        if ((status = atcah_gen_dig(&genDigParam)) != ATCA_SUCCESS)
            BREAK(status, "");

        // Read Encrypted
        if ((status = atcab_read_zone(zone, key_id, block, 0, data, ATCA_BLOCK_SIZE)) != ATCA_SUCCESS)
            BREAK(status, "Read encrypted failed");

        // Decrypt
        for (i = 0; i < ATCA_BLOCK_SIZE; i++)
            data[i] = data[i] ^ tempkey.value[i];

        status = ATCA_SUCCESS;

    }
    while (0);

    _atcab_exit();
    return status;
}

/** \brief Write 32 bytes of data into given slot.
 *		The function takes clear text bytes, but encrypts them for writing over the wire
 *		Data zone must be locked and the slot configuration must be set to encrypted write for the block to be successfully written
 *  \param[in] key_id
 *  \param[in] block     --既然是32字节读，必须设为0？
 *  \param[in] data      The 32 bytes of clear text data to be written to the slot
 *  \param[in] enckey    The key to encrypt with for writing
 *  \param[in] enckeyid  The keyid of the parent encryption key
 *  returns ATCA_STATUS
 */
ATCA_STATUS atcab_write_enc(uint16_t key_id, uint8_t block, const uint8_t *data, const uint8_t* enckey, const uint16_t enckeyid)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t zone = ATCA_ZONE_DATA | ATCA_ZONE_READWRITE_32;
    atca_nonce_in_out_t nonceParam;
    atca_gen_dig_in_out_t genDigParam;
    atca_write_mac_in_out_t writeMacParam;
    atca_temp_key_t tempkey;
    uint8_t sn[32];
    uint8_t numin[NONCE_NUMIN_SIZE] = { 0 };
    uint8_t randout[RANDOM_NUM_SIZE] = { 0 };
    uint8_t cipher_text[ATCA_KEY_SIZE] = { 0 };
    uint8_t mac[WRITE_MAC_SIZE] = { 0 };
    uint8_t other_data[4] = { 0 };
    uint16_t addr;

    do
    {
        // Verify inputs parameters
        if (data == NULL || enckey == NULL)
        {
            status = ATCA_BAD_PARAM;
            break;
        }

        // Read the device SN
        if ((status = atcab_read_zone(ATCA_ZONE_CONFIG, 0, 0, 0, sn, 32)) != ATCA_SUCCESS)
            break;
        // Make the SN continuous by moving SN[4:8] right after SN[0:3]
        memmove(&sn[4], &sn[8], 5);


        // Random Nonce inputs
        memset(&tempkey, 0, sizeof(tempkey));
        memset(&nonceParam, 0, sizeof(nonceParam));
        nonceParam.mode = NONCE_MODE_SEED_UPDATE;
        nonceParam.num_in = (uint8_t*)&numin;
        nonceParam.rand_out = (uint8_t*)&randout;
        nonceParam.temp_key = &tempkey;

        // Send the random Nonce command
        if ((status = atcab_nonce_rand(numin, randout)) != ATCA_SUCCESS)
            BREAK(status, "Nonce failed");

        // Calculate Tempkey
        if ((status = atcah_nonce(&nonceParam)) != ATCA_SUCCESS)
            BREAK(status, "Calc TempKey failed");

        // Supply OtherData so GenDig behavior is the same for keys with SlotConfig.NoMac set
        other_data[0] = ATCA_GENDIG;
        other_data[1] = GENDIG_ZONE_DATA;
        other_data[2] = (uint8_t)(enckeyid);
        other_data[3] = (uint8_t)(enckeyid >> 8);

        // Send the GenDig command
        if ((status = atcab_gendig(GENDIG_ZONE_DATA, enckeyid, other_data, sizeof(other_data))) != ATCA_SUCCESS)
            BREAK(status, "GenDig failed");

        // Calculate Tempkey
        // NoMac bit isn't being considered here on purpose to remove having to read SlotConfig.
        // OtherData is built to get the same result regardless of the NoMac bit.
        memset(&genDigParam, 0, sizeof(genDigParam));
        genDigParam.key_id = enckeyid;
        genDigParam.is_key_nomac = false;
        genDigParam.sn = sn;
        genDigParam.stored_value = (uint8_t*)enckey;
        genDigParam.zone = GENDIG_ZONE_DATA;
        genDigParam.other_data = other_data;
        genDigParam.temp_key = &tempkey;
        if ((status = atcah_gen_dig(&genDigParam)) != ATCA_SUCCESS)
            BREAK(status, "atcah_gen_dig() failed");

        // The get address function checks the remaining variables
        if ((status = atcab_get_addr(ATCA_ZONE_DATA, key_id, block, 0, &addr)) != ATCA_SUCCESS)
            BREAK(status, "Get address failed");

        writeMacParam.zone = zone;
        writeMacParam.key_id = addr;
        writeMacParam.sn = sn;
        writeMacParam.input_data = data;
        writeMacParam.encrypted_data = cipher_text;
        writeMacParam.auth_mac = mac;
        writeMacParam.temp_key = &tempkey;

        if ((status = atcah_write_auth_mac(&writeMacParam)) != ATCA_SUCCESS)
            BREAK(status, "Calculate Auth MAC failed");

        status = atcab_write(writeMacParam.zone, writeMacParam.key_id, writeMacParam.encrypted_data, writeMacParam.auth_mac);

    }
    while (0);

    return status;
}


/** \brief Issues a GenDig command, which performs a SHA256 hash on the source data indicated by zone with the
 *  contents of TempKey.  See the CryptoAuth datasheet for your chip to see what the values of zone
 *  correspond to.
 *  \param[in] zone             Designates the source of the data to hash with TempKey.
 *  \param[in] key_id           Indicates the key, OTP block, or message order for shared nonce mode.
 *  \param[in] other_data       Four bytes of data for SHA calculation when using a NoMac key, 32 bytes for
 *                              "Shared Nonce" mode, otherwise ignored (can be NULL).
 *  \param[in] other_data_size  Size of other_data in bytes.
 *  \return ATCA_SUCCESS on success
 */
ATCA_STATUS atcab_gendig(uint8_t zone, uint16_t key_id, const uint8_t *other_data, uint8_t other_data_size)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    ATCAPacket packet;
    uint16_t execution_time = 0;
    bool hasMACKey = 0;

    if (other_data_size > 0 && other_data == NULL)
        return ATCA_BAD_PARAM;

    do
    {

        // build gendig command
        packet.param1 = zone;
        packet.param2 = key_id;

        if (packet.param1 == GENDIG_ZONE_SHARED_NONCE && other_data_size >= ATCA_BLOCK_SIZE)
        {
            memcpy(&packet.data[0], &other_data[0], ATCA_BLOCK_SIZE);
        }
        else if (packet.param1 == GENDIG_ZONE_DATA && other_data_size >= ATCA_WORD_SIZE)
        {
            memcpy(&packet.data[0], &other_data[0], ATCA_WORD_SIZE);
            hasMACKey = true;
        }

        if ( (status = atGenDig( &packet, hasMACKey)) != ATCA_SUCCESS)
            break;

        execution_time = atGetExecTime( CMD_GENDIG);

        if ( (status != atcab_wakeup()) != ATCA_SUCCESS)
            break;

        // send the command
        if ( (status = atsend( (uint8_t*)&packet, packet.txsize)) != ATCA_SUCCESS)
            break;

        // delay the appropriate amount of time for command to execute
        atca_delay_ms(execution_time);

        // receive the response
        if ( (status = atreceive( packet.data, &(packet.rxsize))) != ATCA_SUCCESS)
            break;

        // Check response size
        if (packet.rxsize < 4)
        {
            if (packet.rxsize > 0)
                status = ATCA_RX_FAIL;
            else
                status = ATCA_RX_NO_RESPONSE;
            break;
        }

        // check for response
        if ( (status = isATCAError(packet.data)) != ATCA_SUCCESS)
            break;

    }
    while (0);

    _atcab_exit();
    return status;
}

/** \brief Write data into config, otp, or data zone with a given byte offset
 *         and length. Offset and length must be multiples of a word (4 bytes).
 *
 * Config zone must be unlocked for writes to that zone. If data zone is
 * unlocked, only 32-byte writes are allowed to slots and OTP and the offset
 * and length must be multiples of 32 or the write will fail.
 *
 *  \param[in] zone          Zone to write data to: ATCA_ZONE_CONFIG(0),
 *                           ATCA_ZONE_OTP(1), or ATCA_ZONE_DATA(2).
 *  \param[in] slot          If zone is ATCA_ZONE_DATA(2), the slot number to
 *                           write to. Ignored for all other zones.
 *  \param[in] offset_bytes  Byte offset within the zone to write to. Must be
 *                           a multiple of a word (4 bytes).
 *  \param[in] data          Data to be written.
 *  \param[in] length        Number of bytes to be written. Must be a multiple
 *                           of a word (4 bytes).
 *
 *  \return ATCA_SUCCESS on success
 */
ATCA_STATUS atcab_write_bytes_zone(uint8_t zone, uint16_t slot, size_t offset_bytes, const uint8_t *data, size_t length)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    size_t zone_size = 0;
    size_t data_idx = 0;
    size_t cur_block = 0;
    size_t cur_word = 0;

    if (zone != ATCA_ZONE_CONFIG && zone != ATCA_ZONE_OTP && zone != ATCA_ZONE_DATA)
        return ATCA_BAD_PARAM;
    if (zone == ATCA_ZONE_DATA && slot > 15)
        return ATCA_BAD_PARAM;
    if (length == 0)
        return ATCA_SUCCESS;  // Always succeed writing 0 bytes
    if (data == NULL)
        return ATCA_BAD_PARAM;
    if (offset_bytes % ATCA_WORD_SIZE != 0 || length % ATCA_WORD_SIZE != 0)
        return ATCA_BAD_PARAM;

    do
    {
        status = atcab_get_zone_size(zone, slot, &zone_size);
        if (status != ATCA_SUCCESS)
            break;
        if (offset_bytes + length > zone_size)
            return ATCA_BAD_PARAM;

        cur_block = offset_bytes / ATCA_BLOCK_SIZE;
        cur_word = (offset_bytes % ATCA_BLOCK_SIZE) / ATCA_WORD_SIZE;

        while (data_idx < length)
        {
            // The last item makes sure we handle the selector, user extra, and lock bytes in the config properly
            if (cur_word == 0 && length - data_idx >= ATCA_BLOCK_SIZE && !(zone == ATCA_ZONE_CONFIG && cur_block == 2))
            {
                status = atcab_write_zone(zone, slot, (uint8_t)cur_block, 0, &data[data_idx], ATCA_BLOCK_SIZE);
                if (status != ATCA_SUCCESS)
                    break;
                data_idx += ATCA_BLOCK_SIZE;
                cur_block += 1;
            }
            else
            {
                // Skip trying to change UserExtra, Selector, LockValue, and LockConfig which require special values
                if (!(zone == ATCA_ZONE_CONFIG && cur_block == 2 && cur_word == 5))
                {
                    status = atcab_write_zone(zone, slot, (uint8_t)cur_block, (uint8_t)cur_word, &data[data_idx], ATCA_WORD_SIZE);
                    if (status != ATCA_SUCCESS)
                        break;
                }
                data_idx += ATCA_WORD_SIZE;
                cur_word += 1;
                if (cur_word == ATCA_BLOCK_SIZE / ATCA_WORD_SIZE)
                {
                    cur_block += 1;
                    cur_word = 0;
                }
            }
        }
    }
    while (false);

    return status;
}

/** \brief Read data from config, otp, or data zone with a given byte offset
 *         and length.
 *
 *  \param[in]  zone          Zone to read data from: Config(0), OTP(1), or
 *                            Data(2).
 *  \param[in]  slot          If zone is Data(2), the slot number to read from.
 *                            Ignored for all other zones.
 *  \param[in]  offset_bytes  Byte offset within the zone to read from.
 *  \param[out] data          Buffer to read data into.
 *  \param[in]  length        Number of bytes to read.
 *
 *  \return ATCA_SUCCESS on success
 */
ATCA_STATUS atcab_read_bytes_zone(uint8_t zone, uint16_t slot, size_t offset_bytes, uint8_t *data, size_t length)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    size_t zone_size = 0;
    uint8_t read_buf[32];
    size_t data_idx = 0;
    size_t cur_block = 0;
    size_t cur_offset = 0;
    uint8_t read_size = ATCA_BLOCK_SIZE;
    size_t read_buf_idx = 0;
    size_t copy_length = 0;
    size_t read_offset = 0;

    if (zone != ATCA_ZONE_CONFIG && zone != ATCA_ZONE_OTP && zone != ATCA_ZONE_DATA)
        return ATCA_BAD_PARAM;
    if (zone == ATCA_ZONE_DATA && slot > 15)
        return ATCA_BAD_PARAM;
    if (length == 0)
        return ATCA_SUCCESS;  // Always succeed reading 0 bytes
    if (data == NULL)
        return ATCA_BAD_PARAM;

    do
    {
        status = atcab_get_zone_size(zone, slot, &zone_size);
        if (status != ATCA_SUCCESS)
            break;
        if (offset_bytes + length > zone_size)
            return ATCA_BAD_PARAM;

        cur_block = offset_bytes / ATCA_BLOCK_SIZE;

        while (data_idx < length)
        {
            if (read_size == ATCA_BLOCK_SIZE && zone_size - cur_block * ATCA_BLOCK_SIZE < ATCA_BLOCK_SIZE)
            {
                // We have less than a block to read and can't read past the end of the zone, switch to word reads
                read_size = ATCA_WORD_SIZE;
                cur_offset = ((data_idx + offset_bytes) / ATCA_WORD_SIZE) % (ATCA_BLOCK_SIZE / ATCA_WORD_SIZE);
            }

            status = atcab_read_zone(
                zone,
                slot,
                (uint8_t)cur_block,
                (uint8_t)cur_offset,
                read_buf,
                read_size);
            if (status != ATCA_SUCCESS)
                break;

            read_offset = cur_block * ATCA_BLOCK_SIZE + cur_offset * ATCA_WORD_SIZE;
            if (read_offset < offset_bytes)
                read_buf_idx = offset_bytes - read_offset;
            else
                read_buf_idx = 0;

            if (length - data_idx < read_size - read_buf_idx)
                copy_length = length - data_idx;
            else
                copy_length = read_size - read_buf_idx;

            memcpy(&data[data_idx], &read_buf[read_buf_idx], copy_length);
            data_idx += copy_length;
            if (read_size == ATCA_BLOCK_SIZE)
                cur_block += 1;
            else
                cur_offset += 1;
        }
    }
    while (false);

    return status;
}


/** \brief Get a 32 byte MAC from the CryptoAuth device given a key ID and a challenge
 *	\param[in]  mode       Controls which fields within the device are used in the message
 *	\param[in]  key_id     The key in the CryptoAuth device to use for the MAC
 *	\param[in]  challenge  The 32 byte challenge number
 *	\param[out] digest     The response of the MAC command using the given challenge
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_mac(uint8_t mode, uint16_t key_id, const uint8_t* challenge, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    ATCAPacket packet;
    uint16_t execution_time = 0;

    do
    {

        // Verify the inputs
        if (digest == NULL)
        {
            status = ATCA_BAD_PARAM;
            break;
        }

        // build mac command
        packet.param1 = mode;
        packet.param2 = key_id;
        if (!(mode & MAC_MODE_BLOCK2_TEMPKEY))
        {
            if (challenge == NULL)
                return ATCA_BAD_PARAM;
            memcpy(&packet.data[0], challenge, 32);  // a 32-byte challenge
        }

        if ( (status = atMAC( &packet)) != ATCA_SUCCESS)
            break;

        execution_time = atGetExecTime( CMD_MAC);

        if ( (status != atcab_wakeup()) != ATCA_SUCCESS)
            break;

        // send the command
        if ( (status = atsend( (uint8_t*)&packet, packet.txsize)) != ATCA_SUCCESS)
            break;

        // delay the appropriate amount of time for command to execute
        atca_delay_ms(execution_time);

        // receive the response
        if ( (status = atreceive( packet.data, &(packet.rxsize))) != ATCA_SUCCESS)
            break;

        // Check response size
        if (packet.rxsize < 4)
        {
            if (packet.rxsize > 0)
                status = ATCA_RX_FAIL;
            else
                status = ATCA_RX_NO_RESPONSE;
            break;
        }

        // check for response
        if ( (status = isATCAError(packet.data)) != ATCA_SUCCESS)
            break;

        memcpy(digest, &packet.data[ATCA_RSP_DATA_IDX], MAC_SIZE);

    }
    while (0);

    _atcab_exit();
    return status;
}

/** \brief Compares a MAC response with input values
 *	\param[in] mode Controls which fields within the device are used in the message
 *	\param[in] key_id The key in the CryptoAuth device to use for the MAC
 *	\param[in] challenge The 32 byte challenge number
 *	\param[in] response The 32 byte mac response number
 *	\param[in] other_data The 13 byte other data number
 *  \return ATCA_STATUS
 */
ATCA_STATUS atcab_checkmac(uint8_t mode, uint16_t key_id, const uint8_t *challenge, const uint8_t *response, const uint8_t *other_data)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    ATCAPacket packet;
    uint16_t execution_time = 0;

    // Verify the inputs
    if (response == NULL || other_data == NULL)
        return ATCA_BAD_PARAM;
    if (!(mode & CHECKMAC_MODE_BLOCK2_TEMPKEY) && challenge == NULL)
        return ATCA_BAD_PARAM;

    do
    {
        // build checkmac command
        packet.param1 = mode;
        packet.param2 = key_id;
        if (challenge != NULL)
            memcpy(&packet.data[0], challenge, CHECKMAC_CLIENT_CHALLENGE_SIZE);
        else
            memset(&packet.data[0], 0, CHECKMAC_CLIENT_CHALLENGE_SIZE);
        memcpy(&packet.data[32], response, CHECKMAC_CLIENT_RESPONSE_SIZE);
        memcpy(&packet.data[64], other_data, CHECKMAC_OTHER_DATA_SIZE);

        if ( (status = atCheckMAC( &packet)) != ATCA_SUCCESS)
            break;

        execution_time = atGetExecTime( CMD_CHECKMAC);

        if ( (status != atcab_wakeup()) != ATCA_SUCCESS)
            break;

        // send the command
        if ( (status = atsend( (uint8_t*)&packet, packet.txsize)) != ATCA_SUCCESS)
            break;

        // delay the appropriate amount of time for command to execute
        atca_delay_ms(execution_time);

        // receive the response
        if ( (status = atreceive( packet.data, &(packet.rxsize))) != ATCA_SUCCESS)
            break;

        // Check response size
        if (packet.rxsize < 4)
        {
            if (packet.rxsize > 0)
                status = ATCA_RX_FAIL;
            else
                status = ATCA_RX_NO_RESPONSE;
            break;
        }

        // check for response
        if ( (status = isATCAError(packet.data)) != ATCA_SUCCESS)
            break;
    }
    while (0);

    _atcab_exit();
    return status;
}

