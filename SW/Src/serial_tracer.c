
#include <string.h>
#include "serial_tracer.h"
#include "stm32f1xx_hal.h"
#include "ring_buffer.h"
#include "usbd_cdc_if.h"
#include "main.h"

extern UART_HandleTypeDef huart3;
extern uint8_t USB_Active;
rb_att_t rx_buff_dmx;
rb_att_t rx_buff_shell;
extern uint8_t UART_mode_SERIAL;
extern uint8_t UART_mode_USB;

void tracer_putchar(uint8_t ch)
{
#ifdef SUPPORT_USB
	if (USB_Active)
	{
		if (UART_mode_USB == 0)
			CDC_Transmit_FS(&ch,1);
	}
#endif
	if (UART_mode_SERIAL == 0)
		HAL_UART_Transmit(&huart3, &ch, 1, 5000);
}

void tracer_puts(uint8_t *str,const uint16_t size)
{
#ifdef SUPPORT_USB
	if (USB_Active)
	{
		if (UART_mode_USB == 0)
			CDC_Transmit_FS(str,size);
	}
#endif
	if (UART_mode_SERIAL == 0)
		HAL_UART_Transmit(&huart3,str,size, 5000);
}

#ifdef USE_UART_IRQ
uint8_t tracer_getchar(void)
{
	uint8_t data;
	if (RB_OK == ring_buffer_get(&rx_buff_shell, &data, 1))
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


