#ifndef APP_INTERFACE_H
#define APP_INTERFACE_H

#include "main_define.h"



uint8_t  Update_PowerOn_SYSFLAG(SYSTEM_NEODUN *flag);
uint8_t  Update_PowerOn_SetFlag(SET_FLAG *Flag,SYSTEM_NEODUN *flag);
uint8_t  Updata_SYS_Count(uint8_t count);	 
uint8_t  Updata_Set_Flag(SET_FLAG *Flag);	 

void 		 PowerOn_PACKInfo_To_Ram(void);
void 		 Update_Flag_ATSHA(SET_FLAG *Flag,SYSTEM_NEODUN *flag);


void 		 EmptyWallet(void);
uint8_t  Get_Address_ID(uint8_t array_address[25]);
uint8_t  Get_Empty_SlotID(void);
uint32_t Get_Plugin_Write_Flash_Sector(uint16_t type);
void		 Hide_address(uint8_t SlotID,uint8_t state);
uint8_t	 Get_Privekey(uint8_t SlotID,uint8_t pin[8],uint8_t Privekey[32]);

uint8_t  Update_AddressInfo_To_Ram(void);
void 		 Sort_wallet_Address(void);
	 
#endif




