/*
 * fx_install.h
 *
 *  Created on: Apr 15, 2020
 *      Author: Micha
 */

#ifndef FX_INSTALL_H_
#define FX_INSTALL_H_

//FX Includes
#include "fx_pwm_running_light.h"
#include "fx_pwm_pulsing_light.h"
#include "fx_strip_running_light.h"
#include "fx_strip_rainbow.h"
#include "fx_strip_cylon.h"
#include "fx_pwm_strip_engine.h"
#include "fx_poofer_ctrl.h"

void fx_install(void)
{
	//REGISTER FX HERE !!!
    fx_pwm_running_light(0);
    fx_pwm_running_pulse_light(0);
    fx_pwm_pulsing_light(0);
    fx_pwm_pulsing_pulse_light(0);
    fx_strip_running_light(0);
    fx_strip_rainbow(0);
    fx_strip_cylon(0);
    fx_pwm_strip_engine(0);
    fx_pwm_poofer(0);
}

#endif /* FX_INSTALL_H_ */
