/*
 * serial_tracer.h
 *
 *  Created on: 12. 3. 2013
 *      Author: honza
 */

#ifndef SERIAL_TRACER_H_
#define SERIAL_TRACER_H_

#include <stdint.h>

void tracer_putchar(uint8_t ch);
void tracer_puts(uint8_t *str,const uint16_t size);
uint8_t tracer_getchar(void);

#endif /* SERIAL_TRACER_H_ */
