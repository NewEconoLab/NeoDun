#ifndef APP_INTERFACE_H
#define APP_INTERFACE_H

#ifdef __cplusplus
 extern "C" {
#endif

unsigned char ReadPinCode(unsigned char *pin);
unsigned char WritePinCode(unsigned char *pin);
unsigned char ReadAT204Flag(unsigned char *flag);
unsigned char Get_StatusOfWallet(void);

#ifdef __cplusplus
 }
#endif	 
	 
#endif




