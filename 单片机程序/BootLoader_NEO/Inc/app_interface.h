#ifndef APP_INTERFACE_H
#define APP_INTERFACE_H

#include "main_define.h"

uint8_t ReadAT204Flag(BOOT_SYS_FLAG *flag);
void EmptyWallet(void);
uint8_t Have_App(void);	 
uint8_t NEO_Test(void);
void SYS_TEST(void);

#endif




