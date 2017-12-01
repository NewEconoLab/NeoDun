#ifndef _CW2015_H
#define _CW2015_H

#include "sys.h"


#define AW_AUTO_CALI
//#define Printf_Debug
/**********************************************************
 AW9136 initial register @ mobile active AW_NormalMode()
**********************************************************/
#define N_LER1					0x001c			//LED enable
#define N_LER2					0x0000			//LED enable
#define N_IMAX1 				0x3300			//LED MAX light setting
#define N_IMAX2 				0x0003			//LED MAX light setting
#define N_IMAX3 				0x0000			//LED MAX light setting
#define N_IMAX4 				0x0000			//LED MAX light setting
#define N_IMAX5 				0x0000			//LED MAX light setting
#define N_LCR						0x00a1			//LED effect control
#define N_IDLECR 				0x1800			//IDLE time setting
#define N_SLPR					0x0023			// touch key enable
#define N_SCFG1					0x0084			// scan time setting
#define N_SCFG2					0x0287			// bit0~3 is sense seting
#define N_OFR1					0x1515			// offset
#define N_OFR2					0x1515			// offset
#define N_OFR3					0x1515			// offset
#define N_THR0					0x2328			//S1 press thred setting
#define N_THR1					0x2328			//S2 press thred setting
#define N_THR2					0x2328			//S3 press thred setting
#define N_THR3					0x2328			//S4 press thred setting
#define N_THR4					0x2328			//S5 press thred setting
#define N_THR5					0x2328			//S6 press thred setting
#define N_SETCNT				0x0202			// debounce
#define N_BLCTH					0x0402			// base trace rate 
#define N_AKSR					0x001c			// AKS 
#define N_INTER					0x001c			// signel click interrupt 
#define N_MPTR					0x0005			//
#define N_GDTR					0x0000			// gesture time setting
#define N_GDCFGR				0x0000			//gesture  key select
#define N_TAPR1					0x0000			//double click 1
#define N_TAPR2					0x0000			//double click 2
#define N_TDTR					0x0000			//double click time
#define N_GIER					0x0000			//gesture and double click enable
#define N_GCR						0x0003			// GCR
/**********************************************************
 AW9136 initial register @ mobile sleep AW_SleepMode()
**********************************************************/
#define S_LER1					0x0000			//LED enable
#define S_LER2					0x0000			//LED enable
#define S_IMAX1 				0x0000			//LED MAX light setting
#define S_IMAX2 				0x0000			//LED MAX light setting
#define S_IMAX3 				0x0000			//LED MAX light setting
#define S_IMAX4 				0x0000			//LED MAX light setting
#define S_IMAX5 				0x0000			//LED MAX light setting
#define S_LCR						0x00a1			//LED effect control
#define S_IDLECR 				0x1800			//IDLE time setting
#define S_SLPR					0x0023			// touch key enable
#define S_SCFG1					0x0084			// scan time setting
#define S_SCFG2					0x0287			// bit0~3 is sense seting
#define S_OFR1					0x0000			// offset
#define S_OFR2					0x0000			// offset
#define S_OFR3					0x0000			// offset
#define S_THR0					0x0a0c			//S1 press thred setting
#define S_THR1					0x0a0c			//S2 press thred setting
#define S_THR2					0x0a0c			//S3 press thred setting
#define S_THR3					0x0a0c			//S4 press thred setting
#define S_THR4					0x0a0c			//S5 press thred setting
#define S_THR5					0x0a0c			//S6 press thred setting
#define S_SETCNT				0x0202			// debounce
#define S_BLCTH					0x0402			// base trace rate
#define S_AKSR					0x001c			// AKS 
#define S_INTER					0x0000			// signel click interrupt 
#define S_GDTR					0x0000			// gesture time setting
#define S_GDCFGR				0x025f			//gesture  key select
#define S_TAPR1					0x0022			//double click 1
#define S_TAPR2					0x0000			//double click 2
#define S_TDTR					0x1932			//double click time
#define S_GIER					0x0010			//gesture and double click enable
#define S_GCR						0x0000			// GCR
/**********************************************************
 AW9136 register address
**********************************************************/
#define     RSTR       	0x00
#define     GCR      	 	0x01
#define     SLPR      	0x02
#define     INTER     	0x03
#define     OSR1     	 	0x04
#define     OSR2     	 	0x05
#define     OSR3     	 	0x06
#define     AKSR      	0x07
#define     SLSR  	    0x08
#define     INITTMR     0x09
#define     THR0   	    0x0a
#define     THR1   	    0x0b
#define     THR2   	    0x0c
#define     THR3   	    0x0d
#define     THR4   	    0x0e
#define     THR5  		 	0x0f


#define     NOISTHR   	0x10
#define     SCFG1  	    0x11
#define     SCFG2  	    0x12
#define     OFR1 	     	0x13
#define     OFR2 	     	0x14
#define     OFR3 	     	0x15
#define     DOFCF0  	 	0x16
#define     DOFCF1  	 	0x17
#define     IDLECR  	 	0x18
#define     MPTR    	 	0x19
#define     DISMAX   	 	0x1a
#define     SETCNT    	0x1b
#define     BLCTH    	 	0x1c
#define     BLDTH       0x1d
#define     MCR      	 	0x1e
#define     ANAR     	 	0x1f

#define     GDCFGR   	 	0x20
#define     GDTR   	    0x21
#define     TDTR   	    0x22
#define     GESTR1    	0x23
#define     GESTR2    	0x24
#define     GESTR3    	0x25
#define     GESTR4    	0x26
#define     TAPR1   	 	0x27
#define     GESTR5    	0x28
#define     GESTR6    	0x29
#define     GESTR7    	0x2a
#define     GESTR8    	0x2b
#define     TAPR2   	 	0x2c
#define     GIER     	 	0x2d
#define     GISR     	 	0x2e
#define     GTIMR     	0x2f

#define     RAWST    	 	0x30
#define     AKSST      	0x31
#define     ISR	    	 	0x32
#define     COR      	 	0x33
#define     FGPRSR    	0x34
#define     MOVCNTR     0x35
#define     KDATA0   	 	0x36
#define     KDATA1   	 	0x37
#define     KDATA2   	 	0x38
#define     KDATA3   	 	0x39
#define     KDATA4   	 	0x3a
#define     KDATA5   	 	0x3b
#define     DUM0     	 	0x3c
#define     DUM1     	 	0x3d
#define     SA_COX	    0x3f

#define     LER1   	    0x50
#define     LER2   	    0x51
#define     LCR   	    0x52
#define     PMR 	     	0x53
#define     RMR  	     	0x54
#define     CTRS1  	    0x55
#define     CTRS2  	    0x56
#define     IMAX1    	 	0x57
#define     IMAX2    	 	0x58
#define     IMAX3    	 	0x59
#define     IMAX4    	 	0x5a
#define     IMAX5    	 	0x5b
#define     TIER     	 	0x5c
#define     INTVEC   	 	0x5d
#define     LISR2     	0x5e
#define     SADDR	     	0x5f

#define     PCR      	 	0x60
#define     CMDR     	 	0x61
#define     RA       	 	0x62
#define     RB       	 	0x63
#define     RC       	 	0x64
#define     RD       	 	0x65
#define     R1       	 	0x66
#define     R2       	 	0x67
#define     R3       	 	0x68
#define     R4       	 	0x69
#define     R5       	 	0x6a
#define     R6       	 	0x6b
#define     R7       	 	0x6c
#define     R8       	 	0x6d
#define     GRPMSK1  	 	0x6e
#define     GRPMSK2  	 	0x6f

#define     TCR      	 	0x70
#define     TAR      	 	0x71
#define     TDR      	 	0x72
#define     TDATA      	0x73
#define     TANA     	 	0x74
#define     TKST     	 	0x75
#define     FEXT   	    0x76
#define     WP       	 	0x7d
#define     WADDR    	 	0x7e
#define     WDATA    	 	0x7f

#ifdef __cplusplus
 extern "C" {
#endif

u16 I2C_read_reg(u8 ReadAddr);
void I2C_write_reg(u8 WriteAddr,u16 DataToWrite);

void AW9136_Init(void);
void AW9136_pwron(void);
void AW9136_pwroff(void);

void AW_NormalMode(void);
void AW_SleepMode(void);
void AW9136_LED_ON(void);
void AW9136_LED_OFF(void);
unsigned char AW91xx_Auto_Cali(void);
void AW_SleepMode_Proc(void);
int AW9136_clear_intr(void);
void AW9136_eint_work(void);

void AW_left_press(void);
void AW_center_press(void);
void AW_right_press(void);
void AW_left_release(void);
void AW_center_release(void);
void AW_right_release(void);

void AW_center_double(void);
void AW_right_slip(void);
void AW_left_slip(void);

void Home_Key_press(void);

void AW_LedReleaseTouch(void);

#ifdef __cplusplus
 }
#endif

#endif
