
#include <string.h>
#include "serial_tracer.h"
#include "stm32f1xx_hal.h"


extern UART_HandleTypeDef huart3;


void tracer_putchar(uint8_t ch)
{
	HAL_UART_Transmit(&huart3, &ch, 1, 5000);
}

void tracer_puts(uint8_t *str,const uint16_t size)
{
	HAL_UART_Transmit(&huart3,str,size, 5000);
}


uint8_t tracer_getchar(void)
{
	uint8_t data;
	if (HAL_OK == HAL_UART_Receive(&huart3, &data, 1, 0))
		return data;
	else
		return 0;
}



