/*
 * gammactrl.h
 *
 *  Created on: Feb 11, 2020
 *      Author: Michael Hartmann
 */

#ifndef GAMMACTRL_H_
#define GAMMACTRL_H_

#include <stdint.h>
#include  <stdbool.h>

typedef enum{
	GAMMA_RED = 0,
	GAMMA_GREEN,
	GAMMA_BLUE,
	GAMMA_STRIP
}t_gammactrl;

uint8_t calc_gamma_val(int16_t gamma, uint8_t val);
bool recalcGamma(void);
uint8_t getGamma(t_gammactrl ch,uint8_t val);

#endif /* GAMMACTRL_H_ */
