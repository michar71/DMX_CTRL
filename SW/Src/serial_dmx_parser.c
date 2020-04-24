/*
 * serial_dmx_parser.c
 *
 *  Created on: Apr 20, 2020
 *      Author: michael.hartmann
 */

#include "serial_dmx_parser.h"
#include "stm32f1xx_hal.h"
#include "dmx512_config.h"
#include "settings.h"
#include "ring_buffer.h"

extern rb_att_t rx_buff;
extern int UART_mode_SERIAL;
extern int UART_mode_USB;

/*
 Serial DMX Protocol
 -------------------
 - The protocol parser will wait for the first 0x5C 0x00 escape sequence to start parsing.
 - After each start sequence the values will be consecutivly written to the registers
 - Additional bytes beyond the currently enabled register count will ignored
 - The parser will process as many bytes as it can fetch with each call but not beyond the expected count
   (At that point only one byte per call will be processed
 - If a start sequence is received before the expected number of bytes the register counter will be reset to the first register
 - "\m" will escape to the serial shell...


Escape Sequences
----------------
0x5C	Escape Sequence Start Code ('\')

0x00	Start of packet	(We reset the register counter
0x5C	Substitute with 0x5C (The escape character)
0x6D	Enter config mode (So manually typing '\m' gets one into menu/debug mode

 */

extern UART_HandleTypeDef huart3;

uint8_t syncstart = 0;
uint16_t reg_count = 0;
uint8_t escaped = 0;

#define ESC_CHAR 0x5C

void dmx_serial_process(void)
{
	uint8_t data;
	uint32_t regsize =  get_reg_length();

	while ((RB_OK == ring_buffer_get(&rx_buff, &data, 1)) && (reg_count<regsize) && (syncstart == 1))
	{
		if ((data == ESC_CHAR) && (escaped == 0))
		{
			escaped = 1;
			break;
		}
		//If we are in escaped mode look for either sync start or return to menu
		if (escaped)
		{
			switch (data)
			{
				case 0x00:			//Sync
					reg_count = 0;
					syncstart = 1;
					break;
				case 'm':			//Jump back to menu
					UART_mode_SERIAL = (int)UART_MODE_SHELL;
					syncstart = 0;
					reg_count = 0;
					return;
				case ESC_CHAR:		//Just add Escape Character
					set_reg(reg_count, ESC_CHAR);
					reg_count++;
					break;
				default:			//Everything else we just ignore
					break;
			}
			escaped = 0;
			break;
		}

		//If not escaped store the data
		set_reg(reg_count, data);
		reg_count++;

		//If we receive more bytes then we need we'll have to resync
		if (reg_count >=regsize)
		{
			syncstart = 0;
			return;
		}
	}

	//If we have exceeded the number of registers or haven't synced yet just pull a byte
	if ((reg_count>=regsize) || (syncstart == 0))
	{
		if (RB_OK == ring_buffer_get(&rx_buff, &data, 1))
		{
			//If it happens to be the escape byte we go into escape mode
			if ((data == ESC_CHAR) && (escaped == 0))
			{
				escaped = 1;
				return;
			}

			//If we are in escaped mode look for either sync start or retru nto menu
			if (escaped)
			{
				switch (data)
				{
					case 0x00:
						reg_count = 0;
						syncstart = 1;
						break;
					case 'm':
						UART_mode_SERIAL = (int)UART_MODE_SHELL;
						syncstart = 0;
						reg_count = 0;
						return;
					default:
						break;
				}
				escaped = 0;
			}
		}
		else
		{
			return;
		}
	}
}
