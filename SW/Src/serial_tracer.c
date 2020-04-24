
#include <string.h>
#include "serial_tracer.h"
#include "stm32f1xx_hal.h"
#include "ring_buffer.h"
#include "usbd_cdc_if.h"

extern rb_att_t rx_buff;
extern UART_HandleTypeDef huart3;
extern uint8_t USB_Active;

void tracer_putchar(uint8_t ch)
{
	if (USB_Active)
	{
		CDC_Transmit_FS(&ch,1);
	}
	HAL_UART_Transmit(&huart3, &ch, 1, 5000);
}

void tracer_puts(uint8_t *str,const uint16_t size)
{
	if (USB_Active)
	{
		CDC_Transmit_FS(str,size);
	}
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


