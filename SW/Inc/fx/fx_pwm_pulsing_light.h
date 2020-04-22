/*
 * fx_pwm_pulsing_light.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Michael Hartmann
 */

#ifndef FX_PWM_PULSING_LIGHT_H_
#define FX_PWM_PULSING_LIGHT_H_

//FX Description
//--------------
//Creates a pulsing light effect for a selectable numbers of lights.
//Overall Speed, ramp in and ramp out can bew controlled.


//Variables Used
//--------------
//STRIP1_SPEED		Overall Speed
//STRIP1_COMPLEXITY Ramp-In Speed
//STRIP1_SIZE 		Number of Channels Used
//STRIP1_PATTERN    Ramp-Out Speed

void fx_pwm_pulsing_light(uint8_t fx_num);
void fx_pwm_pulsing_pulse_light(uint8_t fx_num);
t_fx_result fx_pwm_pulsing_light_run(t_fx_state state,uint32_t framecount,const uint32_t duration);

#endif /* FX_PWM_PULSING_LIGHT_H_ */
