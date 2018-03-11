#ifndef APP_INTERFACE_H
#define APP_INTERFACE_H

#include "main_define.h"

unsigned char ReadPinCode(unsigned char *pin);
unsigned char WritePinCode(unsigned char *pin);
unsigned char ReadAT204Flag(BOOT_FLAG *flag);
void EmptyWallet(void);
void ChangeWallet(void);



#endif




