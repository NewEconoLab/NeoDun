#ifndef  SWI_ATSHA204A_H
#define  SWI_ATSHA204A_H


//数值与原库一致
//for swi_receive_bytes()
#define ATCA_STATUS    uint8_t
#define ATCA_SUCCESS          0x00
#define ATCA_RX_FAIL          0xE6
#define ATCA_RX_TIMEOUT       0xEB

#define ATCA_INVALID_SIZE      0xE4
#define ATCA_RX_NO_RESPONSE    0xE7

/**
 * \brief This enumeration lists flags for SWI.
 */
/*
enum swi_flag//改为define会省flash，试了，没节省，但仍用define
{
    SWI_FLAG_CMD   = (uint8_t)0x77,     //!< flag preceding a command
    SWI_FLAG_TX    = (uint8_t)0x88,     //!< flag requesting a response
    SWI_FLAG_IDLE  = (uint8_t)0xBB,     //!< flag requesting to go into Idle mode
    SWI_FLAG_SLEEP = (uint8_t)0xCC      //!< flag requesting to go into Sleep mode
};
 */

#define SWI_FLAG_CMD    (uint8_t)0x77     //!< flag preceding a command
#define SWI_FLAG_TX     (uint8_t)0x88     //!< flag requesting a response
#define SWI_FLAG_IDLE   (uint8_t)0xBB     //!< flag requesting to go into Idle mode
#define SWI_FLAG_SLEEP  (uint8_t)0xCC     //!< flag requesting to go into Sleep mode


void swi_send_wake_token(void);

/**
 * \brief Send a number of bytes.This function should not be called directly ,instead should use hal_swi_send() which call this function.
 *
 * \param[in] count   number of bytes to send.
 * \param[in] buffer  pointer to buffer containing bytes to send
 */
void swi_send_bytes(uint8_t count, uint8_t *buffer);

/**
 * \brief Send one byte.
 *
 * \param[in] byte  byte to send
 */
//void swi_send_byte(uint8_t byte);

/**
 * \brief Receive a number of bytes.This function should not be called directly ,instead should use hal_swi_receive() which call this function.
 *
 * \param[in]  count   number of bytes to receive
 * \param[out] buffer  pointer to receive buffer
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS swi_receive_bytes(uint8_t count, uint8_t *buffer);

#endif

