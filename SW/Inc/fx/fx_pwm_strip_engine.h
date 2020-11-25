/*
 * fx_pwm_running_light.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Michael Hartmann
 */

#ifndef FX_PWM_STRIP_ENGINE_H_
#define FX_PWM_STRIP_ENGINE_H_

//FX Description
//--------------
//turns on  Ch3 W to max (Front Effect)
//Creates Sinoidal wave on Ch 1,2,3)


//Variables Used
//--------------
//STRIP1_SPEED		Overall Speed
//DMX_STRIP1_SIZE   Sinus-Length
//DMX_STRIP1_COMPLEXITY   Sinus-offset
//STRIP1_PATTERN    0 = Forward, 1 = Reverse
//STRIP1_V1         Hue
//STRIP1_V2         Color Shift/Lag
//STRIP1_V3         AMP_DIV


void fx_pwm_strip_engine(uint8_t fx_num);
t_fx_result fx_pwm_strip_engine_run(t_fx_state state,uint32_t framecount,const uint32_t duration);

#endif /* FX_PWM_STRIP_ENGINE_H_ */
