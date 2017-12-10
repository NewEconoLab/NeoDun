#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"


void HardFault_Handler(void)
{
    printf("HardFault_Handler\r\n");	
    while (1)
    {
        NVIC_SystemReset();
    }
}





#include <stdio.h>
#include "stm32f4xx_hal.h"
#include <rt_misc.h>
#pragma import(__use_no_semihosting_swi)


#define UART  USART1

struct __FILE
{
    int handle;
};
 
FILE __stdout;


int fputc(int ch, FILE *f)
{

	UART->DR = ch;
    while( !(UART->SR & USART_SR_TXE));
    
    
    return (ch);
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch) {
    UART->DR = ch;
        while( !(UART->SR & USART_SR_TXE));
}


void _sys_exit(int return_code) {
    label:  goto label;  /* endless loop */
}



