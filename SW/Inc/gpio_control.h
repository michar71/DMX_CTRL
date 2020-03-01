/*
 * gpio_control.h
 *
 *  Created on: Feb 11, 2020
 *      Author: Michael Hartmann
 */

#ifndef GPIO_CONTROL_H_
#define GPIO_CONTROL_H_

#include <stdint.h>

uint8_t get_mode_from_pins(void);
uint8_t get_addr_from_pins(void);
uint8_t check_button(void);

#endif /* GPIO_CONTROL_H_ */
