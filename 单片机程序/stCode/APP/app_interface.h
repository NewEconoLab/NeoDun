#ifndef APP_INTERFACE_H
#define APP_INTERFACE_H

#include "main_define.h"



uint8_t Update_PowerOn_SYSFLAG(SYSTEM_FLAG *flag);
uint8_t Update_PowerOn_SetFlag(SET_FLAG *Flag,SYSTEM_FLAG *flag);
uint8_t Updata_SYS_Count(uint8_t count);	 
uint8_t Updata_Set_Flag(SET_FLAG *Flag);	 

void Update_Flag_ATSHA(SET_FLAG *Flag,SYSTEM_FLAG *flag);


void 		EmptyWallet(void);
uint8_t Get_Address_ID(uint8_t array_address[25]);
uint8_t Get_Empty_SlotID(void);
void	Hide_address(uint8_t SlotID,uint8_t state);

 
	 
#endif




