/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef PCA10001_H
#define PCA10001_H

#include "nrf_gpio.h"

#define LED_START      24
#define LED_0          24
#define LED_1          25
#define LED_STOP       25

#define BUTTON_START   16
#define BUTTON_0       16
#define BUTTON_1       17
#define BUTTON_STOP    17
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define RX_PIN_NUMBER  21
#define TX_PIN_NUMBER  22
#define CTS_PIN_NUMBER 1
#define RTS_PIN_NUMBER 2
#define HWFC           false

#endif
