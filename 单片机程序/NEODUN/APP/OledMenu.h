#ifndef OLED_MENU_H_
#define OLED_MENU_H_

#include "stm32f4xx_hal.h"
#include "showsign.h"
#include "main_define.h"

void Display_Triangle(uint8_t direction);
void Display_arrow(uint8_t direction);
void Display_Usb(void);
uint8_t Display_Time_count(void);
void Display_Address(uint8_t state,ADDRESS *address);



void Display_MainPage(void);
void Display_MainPage_0Add(void);
void Display_MainPage_1Add(void);
void Display_MainPage_2Add(void);
void Display_MainPage_3Add(void);
void Display_MainPage_4Add(void);
void Display_MainPage_5Add(void);
void Display_MainPage_judge(void);
void Display_SetCode(void);
uint8_t Display_VerifyCode(void);
uint8_t Display_VerifyCode_PowerOn(void);

void Display_Click_Add(uint8_t AddID);
void Display_Coin_NEO(void);
void Display_ALL_Coin(void);
void Display_Set_About(void);
void Display_Set_Security_ResetWallet(void);
void Display_Set_Security(void);
void Display_Click_Set(void);

uint8_t Display_AddAdd(char *address);
uint8_t Display_DelAdd(uint8_t AddID);
uint8_t Display_Sign_ContractTran(SIGN_Out_Para *data,ADDRESS *address,uint8_t signdata_index,char *dst_address);
uint8_t Display_Sign_Nep5(SIGN_Out_Para *data,ADDRESS *address,char *dst_address);


uint8_t Display_Sign_Data_Type_Identify(void);
uint8_t Display_Updata_Wallet(void);

#endif /* CPP_DISPLAYMEM_H_ */

