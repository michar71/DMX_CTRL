/*
 * fx_pwm_poofer_ctrl.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Michael Hartmann
 */

#ifndef FX_INCANDESCENT_CTRL_H_
#define FX_INCANDESCENT_CTRL_H_

//FX Description
//--------------
//Control a Relais on Channel 1R hooked up to a incandescent light to make it flicker...


//Variables Used
//--------------
//STRIP1_PATTERN
//STRIP1_SPEED		Overall rate of glitches
//STRIP1_COMPLEXITY Length of Glitches
//STRIP1_V1
//STRIP1_V2
//STRIP1_V3

void fx_pwm_incandescent(uint8_t fx_num);
t_fx_result fx_pwm_incandescent_run(t_fx_state state,uint32_t framecount,const uint32_t duration);

#endif /* FX_INCANDESCENT_CTRL_H_ */
