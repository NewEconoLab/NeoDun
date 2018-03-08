#ifndef ATCA_IFACE_H
#define ATCA_IFACE_H

/** \defgroup interface ATCAIface (atca_)
*  \brief Abstract interface to all CryptoAuth device types.  This interface
*  connects to the HAL implementation and abstracts the physical details of the
*  device communication from all the upper layers of CryptoAuthLib
@{ */

#define  USE_TWI_INTERFACE  //如果使用I2C接口，使能这个宏定义，使用SWI接口需要屏蔽这个定义
//#define  USE_SWI_INTERFACE    //如果使用单线SWII2C接口，使能这个宏定义，使用I2C接口需要屏蔽这个定义


#ifdef __cplusplus
extern "C" {
#endif

#include "atca_command.h"

	/* ATCAIfaceCfg is a mediator object between a completely abstract notion of a physical interface and an actual physical interface.

	The main purpose of it is to keep hardware specifics from bleeding into the higher levels - hardware specifics could include
	things like framework specific items (ASF SERCOM) vs a non-Atmel I2C library constant that defines an I2C port.   But I2C has
	roughly the same parameters regardless of architecture and framework.  I2C
	*/

	ATCA_STATUS atsend(uint8_t *txdata, int txlength);
	ATCA_STATUS atreceive(uint8_t *rxdata, uint16_t *rxlength);
	ATCA_STATUS atwake(void);
	ATCA_STATUS atidle(void);
	ATCA_STATUS atsleep(void);

#ifdef _WINDOWS
	void ataddress(uint8_t addri2c);
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif


