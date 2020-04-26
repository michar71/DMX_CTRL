/*
 * light_update.h
 *
 *  Created on: Feb 18, 2020
 *      Author: Michael Hartmann
 */

#ifndef LIGHT_UPDATE_H_
#define LIGHT_UPDATE_H_

#include <stdint.h>


//Create/Init shadow registers
void init_update_lights(void);

//Scale Input via Max Brightness value to PWM value
uint16_t scale_value( uint8_t val,int16_t offset, int16_t gain, int16_t gamma);

//Check what has changed and update Lights
//We only update parameters that have changed otherwise we might have waveform-artifacts...
void update_pwm_lights(uint8_t force);
void update_fx(void);

#endif /* LIGHT_UPDATE_H_ */
