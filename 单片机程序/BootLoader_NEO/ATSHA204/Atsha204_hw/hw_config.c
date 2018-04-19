
/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "bitBang_I2c.h"

void ATSHA204_Init(void)
{
		__HAL_RCC_GPIOB_CLK_ENABLE();
	  bitBang_I2c_Init();
}
