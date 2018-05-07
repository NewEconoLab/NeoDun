/**
 * \file
 * \brief Atmel CryptoAuthentication device command builder - this is the main object that builds the command
 * byte strings for the given device.  It does not execute the command.  The basic flow is to call
 * a command method to build the command you want given the parameters and then send that byte string
 * through the device interface.
 *
 * The primary goal of the command builder is to wrap the given parameters with the correct packet size and CRC.
 * The caller should first fill in the parameters required in the ATCAPacket parameter given to the command.
 * The command builder will deal with the mechanics of creating a valid packet using the parameter information.
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

#include <stdlib.h>
#include <string.h>
#include "atca_command.h"


// full superset of commands goes here

/** \brief ATCACommand CheckMAC method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atCheckMAC( ATCAPacket *packet)
{
    // Set the opcode & parameters
    packet->opcode = ATCA_CHECKMAC;
    packet->txsize = CHECKMAC_COUNT;
    packet->rxsize = CHECKMAC_RSP_SIZE;

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand DeriveKey method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \param[in] hasMAC  hasMAC determines if MAC data is present in the packet input
 * \return ATCA_STATUS
 */
ATCA_STATUS atDeriveKey( ATCAPacket *packet, bool hasMAC)
{
    // Set the opcode & parameters
    packet->opcode = ATCA_DERIVE_KEY;

    // hasMAC must be given since the packet does not have any implicit information to
    // know if it has a mac or not unless the size is preset
    if (hasMAC)
        packet->txsize = DERIVE_KEY_COUNT_LARGE;
    else
        packet->txsize = DERIVE_KEY_COUNT_SMALL;

    packet->rxsize = DERIVE_KEY_RSP_SIZE;
    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand Generate Digest method
 * \param[in] cacmd     instance
 * \param[in] packet    pointer to the packet containing the command being built
 * \param[in] isNoMacKey Should be true if GenDig is being run on a slot that has its SlotConfig.NoMac bit set
 * \return ATCA_SUCCESS
 */
ATCA_STATUS atGenDig( ATCAPacket *packet, bool isNoMacKey)
{
    // Set the opcode & parameters
    packet->opcode = ATCA_GENDIG;

    if (packet->param1 == GENDIG_ZONE_SHARED_NONCE) // shared nonce mode
        packet->txsize = GENDIG_COUNT + 32;
    else if (isNoMacKey)
        packet->txsize = GENDIG_COUNT + 4;  // noMac keys use 4 bytes of OtherData in calculation
    else
        packet->txsize = GENDIG_COUNT;

    packet->rxsize = GENDIG_RSP_SIZE;

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand HMAC method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atHMAC( ATCAPacket *packet)
{

    // Set the opcode & parameters
    packet->opcode = ATCA_HMAC;
    packet->txsize = HMAC_COUNT;
    packet->rxsize = HMAC_RSP_SIZE;

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand Info method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atInfo( ATCAPacket *packet)
{

    // Set the opcode & parameters
    packet->opcode = ATCA_INFO;
    packet->txsize = INFO_COUNT;
    packet->rxsize = INFO_RSP_SIZE;

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand Lock method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atLock( ATCAPacket *packet)
{

    // Set the opcode & parameters
    packet->opcode = ATCA_LOCK;
    packet->txsize = LOCK_COUNT;
    packet->rxsize = LOCK_RSP_SIZE;

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand MAC method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atMAC( ATCAPacket *packet)
{

    // Set the opcode & parameters
    // variable packet size
    packet->opcode = ATCA_MAC;
    if (!(packet->param1 & MAC_MODE_BLOCK2_TEMPKEY))
        packet->txsize = MAC_COUNT_LONG;
    else
        packet->txsize = MAC_COUNT_SHORT;

    packet->rxsize = MAC_RSP_SIZE;

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand Nonce method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atNonce( ATCAPacket *packet)
{
    // Set the opcode & parameters
    // variable packet size
    int mode = packet->param1 & 0x03;

    packet->opcode = ATCA_NONCE;

    if ( (mode == 0 || mode == 1) )         // mode[0:1] == 0 | 1 then NumIn is 20 bytes
    {
        packet->txsize = NONCE_COUNT_SHORT; // 20 byte challenge
        packet->rxsize = NONCE_RSP_SIZE_LONG;
    }
    else if (mode == 0x03)                  // NumIn is 32 bytes
    {
        packet->txsize = NONCE_COUNT_LONG;  // 32 byte challenge
        packet->rxsize = NONCE_RSP_SIZE_SHORT;
    }
    else
    {
        return ATCA_BAD_PARAM;
    }

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand Pause method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atPause( ATCAPacket *packet)
{
    // Set the opcode & parameters
    packet->opcode = ATCA_PAUSE;
    packet->txsize = PAUSE_COUNT;
    packet->rxsize = PAUSE_RSP_SIZE;

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand Random method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atRandom( ATCAPacket *packet)
{

    // Set the opcode & parameters
    packet->opcode = ATCA_RANDOM;
    packet->txsize = RANDOM_COUNT;
    packet->rxsize = RANDOM_RSP_SIZE;

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand Read method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atRead( ATCAPacket *packet)
{

    // Set the opcode & parameters
    packet->opcode = ATCA_READ;
    packet->txsize = READ_COUNT;

    // variable response size based on read type
    if ((packet->param1 & 0x80) == 0)
        packet->rxsize = READ_4_RSP_SIZE;
    else
        packet->rxsize = READ_32_RSP_SIZE;

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand SHA method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atSHA( ATCAPacket *packet)
{
    if (packet->param2 >  ATCA_SHA256_BLOCK_SIZE)
        return ATCA_BAD_PARAM;

    // Set the opcode & parameters
    packet->opcode = ATCA_SHA;

    switch (packet->param1)
    {
    case SHA_MODE_SHA256_START: // START
    case SHA_MODE_HMAC_START:
    case SHA_MODE_SHA256_PUBLIC:
        packet->rxsize = SHA_RSP_SIZE_SHORT;
        packet->txsize = SHA_COUNT_LONG;
        break;
    case SHA_MODE_SHA256_UPDATE:                          // UPDATE
        //slm if (cacmd->dt == ATSHA204A)
		//if (g_devType == ATSHA204A)
            packet->rxsize = (ATCA_SHA_DIGEST_SIZE + 3);  // ATSHA devices return the digest with this command
        //else
        //    packet->rxsize = SHA_RSP_SIZE_SHORT;
        packet->txsize = SHA_COUNT_LONG + packet->param2;
        break;
    case SHA_MODE_SHA256_END: // END
    case SHA_MODE_HMAC_END:
        packet->rxsize = SHA_RSP_SIZE_LONG;
        // check the given packet for a size variable in param2.  If it is > 0, it should
        // be 0-63, incorporate that size into the packet
        packet->txsize = SHA_COUNT_LONG + packet->param2;
        break;
    default:
        return ATCA_BAD_PARAM;
    }

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand UpdateExtra method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atUpdateExtra( ATCAPacket *packet)
{

    // Set the opcode & parameters
    packet->opcode = ATCA_UPDATE_EXTRA;
    packet->txsize = UPDATE_COUNT;
    packet->rxsize = UPDATE_RSP_SIZE;

    atCalcCrc(packet);
    return ATCA_SUCCESS;
}

/** \brief ATCACommand Write method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atWrite( ATCAPacket *packet, bool hasMAC)
{
    // Set the opcode & parameters
    packet->opcode = ATCA_WRITE;

    packet->txsize = 7;
    if (packet->param1 & ATCA_ZONE_READWRITE_32)
        packet->txsize += ATCA_BLOCK_SIZE;
    else
        packet->txsize += ATCA_WORD_SIZE;
    if (hasMAC)
        packet->txsize += WRITE_MAC_SIZE;

    packet->rxsize = WRITE_RSP_SIZE;
    atCalcCrc(packet);
    return ATCA_SUCCESS;
}


/** \brief execution times for 204a, these are based on the typical value from the datasheet
 */
uint16_t exectimes_204a[] = {
    3,  // WAKE_TWHI
    38, // CMD_CHECKMAC
    0,
    62, // CMD_DERIVEKEY
    0,
    43, // CMD_GENDIG
    0,
    69, // CMD_HMAC
    2,  // CMD_DevRev
    24, // CMD_LOCK
    35, // CMD_MAC
    60, // CMD_NONCE
    2,  // CMD_PAUSE
    0,
    50, // CMD_RANDOM
    5,  // CMD_READMEM
    22, // CMD_SHA
    0,
    12, // CMD_UPDATEEXTRA
    0,
    42  // CMD_WRITEMEM
};

/** \brief return the typical execution type for the given command
 *
 * \param[in] cacmd the command object for which the execution times are associated
 * \param[in] cmd - the specific command for which to lookup the execution time
 * \return typical execution time in milleseconds for the given command
 */

uint16_t atGetExecTime(ATCA_CmdMap cmd)
{
	return exectimes_204a[cmd];
}


/** \brief Calculates CRC over the given raw data and returns the CRC in
 *         little-endian byte order.
 *
 * \param[in]  length  Size of data not including the CRC byte positions
 * \param[in]  data    Pointer to the data over which to compute the CRC
 * \param[out] crc_le  Pointer to the place where the two-bytes of CRC will be
 *                     returned in little-endian byte order.
 */
void atCRC(size_t length, const uint8_t *data, uint8_t *crc_le)
{
    size_t counter;
    uint16_t crc_register = 0;
    uint16_t polynom = 0x8005;
    uint8_t shift_register;
    uint8_t data_bit, crc_bit;

    for (counter = 0; counter < length; counter++)
    {
        for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1)
        {
            data_bit = (data[counter] & shift_register) ? 1 : 0;
            crc_bit = crc_register >> 15;
            crc_register <<= 1;
            if (data_bit != crc_bit)
                crc_register ^= polynom;
        }
    }
    crc_le[0] = (uint8_t)(crc_register & 0x00FF);
    crc_le[1] = (uint8_t)(crc_register >> 8);
}


/** \brief This function calculates CRC and adds it to the correct offset in the packet data
 * \param[in] packet Packet to calculate CRC data for
 */

void atCalcCrc(ATCAPacket *packet)
{
    uint8_t length, *crc;

    length = packet->txsize - ATCA_CRC_SIZE;
    // computer pointer to CRC in the packet
    crc = &(packet->txsize) + length;

    // stuff CRC into packet
    atCRC(length, &(packet->txsize), crc);
}


/** \brief This function checks the consistency of a response.
 * \param[in] response pointer to response
 * \return status of the consistency check
 */

uint8_t atCheckCrc(const uint8_t *response)
{
    uint8_t crc[ATCA_CRC_SIZE];
    uint8_t count = response[ATCA_COUNT_IDX];

    if (count < ATCA_CRC_SIZE)
        return ATCA_BAD_PARAM;

    count -= ATCA_CRC_SIZE;
    atCRC(count, response, crc);

    return (crc[0] == response[count] && crc[1] == response[count + 1]) ? ATCA_SUCCESS : ATCA_BAD_CRC;
}


/** \brief checks for basic error frame in data
 * \param[in] data pointer to received data - expected to be in the form of a CA device response frame
 * \return ATCA_STATUS indicating type of error or no error
 */

ATCA_STATUS isATCAError(uint8_t *data)
{
    uint8_t good[4] = { 0x04, 0x00, 0x03, 0x40 };

    if (memcmp(data, good, 4) == 0)
        return ATCA_SUCCESS;

    if (data[0] == 0x04)        // error packets are always 4 bytes long
    {
        switch (data[1])
        {
        case 0x01:              // checkmac or verify failed
            return ATCA_CHECKMAC_VERIFY_FAILED;
            //slm break;
        case 0x03: // command received byte length, opcode or parameter was illegal
            return ATCA_PARSE_ERROR;
			//slm break;
        case 0x0f: // chip can't execute the command
            return ATCA_EXECUTION_ERROR;
			//slm break;
        case 0x11: // chip was successfully woken up
            return ATCA_WAKE_SUCCESS;
			//slm break;
        case 0xff: // bad crc found or other comm error
            return ATCA_STATUS_CRC;
			//slm break;
        default:
            return ATCA_GEN_FAIL;
			//slm break;
        }
    }
    else
    {
        return ATCA_SUCCESS;
    }
}

/** @} */
