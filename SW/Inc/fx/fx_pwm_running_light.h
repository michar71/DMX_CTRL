/*
 * fx_pwm_running_light.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Michael Hartmann
 */

#ifndef FX_PWM_RUNNING_LIGHT_H_
#define FX_PWM_RUNNING_LIGHT_H_

//FX Description
//--------------
//Creates a running light effect that cycles through a selectable number of Channels.
//The starting channel can be selected (so it is possible to shift the pulses around)
//The overall speed and the duration of the afterglow can be selected.
//Afterglow is defined by the amount each light is reduced.
//
//Also demonstrating how the same effect can be used for for single-shot and continuous mode.


//Variables Used
//--------------
//STRIP1_SPEED		Overall Speed
//STRIP1_COMPLEXITY Afterglow Length
//STRIP1_SIZE 		Number of Channels Used
//STRIP1_PATTERN    0 = Forward, 1 = Reverse


void fx_pwm_running_light(uint8_t fx_num);
void fx_pwm_running_pulse_light(uint8_t fx_num);
t_fx_result fx_pwm_running_light_run(t_fx_state state,uint32_t framecount,const uint32_t duration);

#endif /* FX_PWM_RUNNING_LIGHT_H_ */
