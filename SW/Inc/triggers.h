/*
 * triggers.h
 *
 *  Created on: Mar 5, 2020
 *      Author: Michael Hartmann
 */

#ifndef TRIGGERS_H_
#define TRIGGERS_H_

#include <stdint.h>

void init_trigger(void);
void process_trigger(void);
uint8_t get_trigger(uint8_t ch);
void print_adc_data(void);

#endif /* TRIGGERS_H_ */
