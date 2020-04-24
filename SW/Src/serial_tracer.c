
#include <string.h>
#include "serial_tracer.h"
#include "stm32f1xx_hal.h"
#include "ring_buffer.h"

extern rb_att_t rx_buff;
extern UART_HandleTypeDef huart3;

void tracer_putchar(uint8_t ch)
{
	HAL_UART_Transmit(&huart3, &ch, 1, 5000);
}

void tracer_puts(uint8_t *str,const uint16_t size)
{
	HAL_UART_Transmit(&huart3,str,size, 5000);
}

#ifdef USE_UART_IRQ
uint8_t tracer_getchar(void)
{
	uint8_t data;
	if (RB_OK == ring_buffer_get(&rx_buff, &data, 1))
		return data;
	else
		return 0;
}
#else
uint8_t tracer_getchar(void)
{
	uint8_t data;
	if (HAL_OK == HAL_UART_Receive(&huart3, &data, 1, 0))
		return data;
	else
		return 0;
}
#endif


