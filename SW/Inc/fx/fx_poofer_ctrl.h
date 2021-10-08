/*
 * fx_pwm_poofer_ctrl.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Michael Hartmann
 */

#ifndef FX_POOFER_CTRL_H_
#define FX_POOFER_CTRL_H_

//FX Description
//--------------
//Control a Relais on Channel 1R hooked up to a gas poofer


//Variables Used
//--------------
//STRIP1_COMPLEXITY 0 = poffrer on, every other code = poofer of

void fx_pwm_poofer(uint8_t fx_num);
t_fx_result fx_pwm_poofer_run(t_fx_state state,uint32_t framecount,const uint32_t duration);

#endif /* FX_PWM_PULSING_LIGHT_H_ */
