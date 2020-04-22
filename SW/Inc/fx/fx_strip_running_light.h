/*
 * fx_pwm_running_light.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Michael Hartmann
 */

#ifndef FX_STRIP_RUNNING_LIGHT_H_
#define FX_STRIP_RUNNING_LIGHT_H_

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
//STRIP1_SIZE 		Stepsize
//STRIP1_PATTERN    0 = Forward, 1 = Reverse
//STRIP1_COMPLEXITY Afterglow Length
//STRIP1_V1         R Color
//STRIP1_V2         G Color
//STRIP1_V3         B Color
//STRIP2_SPEED		Overall Speed
//STRIP2_SIZE 		Stepsize
//STRIP2_PATTERN    0 = Forward, 1 = Reverse
//STRIP2_COMPLEXITY Afterglow Length
//STRIP2_V1         R Color
//STRIP2_V2         G Color
//STRIP2_V3         B Color

void fx_strip_running_light(uint8_t fx_num);
t_fx_result fx_strip_running_light_run(t_fx_state state,uint32_t framecount,const uint32_t duration);

#endif /* FX_STRIP_RUNNING_LIGHT_H_ */
