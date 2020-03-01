/*
 * gpio_control.c
 *
 *  Created on: Feb 11, 2020
 *      Author: Michael Hartmann
 */


#include "gpio_control.h"
#include <main.h>
#include <stm32f103xb.h>
#include <stm32f1xx.h>
#include <stm32f1xx_hal_gpio.h>

uint8_t get_mode_from_pins(void)
{
	uint8_t mode = 0;
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(MODE_0_GPIO_Port, MODE_0_Pin))
		mode = mode | 0x01;
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(MODE_1_GPIO_Port, MODE_1_Pin))
		mode = mode | 0x02;
	return mode;
}

uint8_t get_addr_from_pins(void)
{
	uint8_t addr = 0;
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ADDR_0_GPIO_Port, ADDR_0_Pin))
		addr = addr | 0x01;
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ADDR_1_GPIO_Port, ADDR_1_Pin))
		addr = addr | 0x02;
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ADDR_2_GPIO_Port, ADDR_2_Pin))
		addr = addr | 0x04;
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ADDR_3_GPIO_Port, ADDR_3_Pin))
		addr = addr | 0x08;
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ADDR_4_GPIO_Port, ADDR_4_Pin))
		addr = addr | 0x10;
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ADDR_5_GPIO_Port, ADDR_5_Pin))
		addr = addr | 0x20;
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ADDR_6_GPIO_Port, ADDR_6_Pin))
		addr = addr | 0x40;
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ADDR_7_GPIO_Port, ADDR_7_Pin))
		addr = addr | 0x80;
	return addr;
}

uint8_t check_button(void)
{
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin))
		return 1;

	return 0;
}
