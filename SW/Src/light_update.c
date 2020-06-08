/*
 * light_update.c
 *
 *  Created on: Feb 18, 2020
 *      Author: Michael Hartmann
 */
#include "light_update.h"
#include "pwm_control.h"
#include "dmx512_config.h"
#include "settings.h"
#include "fx_manager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint16_t brightness_scale = 0;
static uint8_t* reg_shadow;
//Create/Init shadow registers
void init_update_lights(void)
{
	//allocate memory
	if (reg_shadow != NULL)
		free(reg_shadow);

	reg_shadow = calloc(get_reg_length()+1, sizeof(uint8_t));
	reg_shadow[MAX_BRIGHTNESS] = 255;
	update_pwm_lights(1);
}


//Scales the value to the output range and applies correction


//output = gammatable[((input - offset) * gain)]
uint16_t scale_value( uint8_t val,int16_t offset, int16_t gain, t_gammactrl gamma_ch)
{
	int32_t res = 0;
	int8_t gamma = 0;
	//1) Scale input between 0 and max
	res = ((uint32_t)val * (uint32_t) MAX_PWM)/(uint32_t)255;
	res = (res * (uint32_t)brightness_scale) / (uint32_t) MAX_PWM;

	//Subtract offset
	res = res - offset;

	//Apply Gain
	res = (int16_t) ((float)res * ((float)gain)/100);

	//Gamma Curve
	//-----------

	//Get Gamma Value from Input
	gamma = getGamma(gamma_ch,val);

	//Apply as multiplier to output
	res = ((int16_t)gamma * res)/255;


	//Limit output to valid range
	if (res<0)
		res = 0;
	if (res > MAX_PWM)
		res = MAX_PWM;
	return res;
}

//Check what has changed and update Lights
//We only update parameters that have changed otherwise we might have waveform-artifacts...
void update_pwm_lights(uint8_t force)
{

	//Check if global brightness has changed and scale value
	if ((reg_shadow[MAX_BRIGHTNESS])!= get_reg(MAX_BRIGHTNESS) || force)
	{
		brightness_scale = ((uint32_t)get_reg(MAX_BRIGHTNESS) * (uint32_t) MAX_PWM)/(uint32_t)255;
		//After a brightness update we need to update all the over PWM outputs
		force = 1;
		reg_shadow[MAX_BRIGHTNESS] = get_reg(MAX_BRIGHTNESS);
	}

	//Update PWM Values if needed
	if ((reg_shadow[CH1_RED])!= get_reg(CH1_RED) || force)
	{
		configPWM(PWM_CH1,CH_RED,scale_value(get_reg(CH1_RED),settings.offset_red,settings.gain_red,GAMMA_RED));
		reg_shadow[CH1_RED] = get_reg(CH1_RED);
	}

	if ((reg_shadow[CH1_GREEN])!= get_reg(CH1_GREEN) || force)
	{
		configPWM(PWM_CH1,CH_GREEN,scale_value(get_reg(CH1_GREEN),settings.offset_green,settings.gain_green,GAMMA_GREEN));
		reg_shadow[CH1_GREEN] = get_reg(CH1_GREEN);
	}

	if ((reg_shadow[CH1_BLUE])!= get_reg(CH1_BLUE) || force)
	{
		configPWM(PWM_CH1,CH_BLUE,scale_value(get_reg(CH1_BLUE),settings.offset_blue,settings.gain_blue,GAMMA_BLUE));
		reg_shadow[CH1_BLUE] = get_reg(CH1_BLUE);
	}

	if ((reg_shadow[CH2_RED])!= get_reg(CH2_RED) || force)
	{
		configPWM(PWM_CH2,CH_RED,scale_value(get_reg(CH2_RED),settings.offset_red,settings.gain_red,GAMMA_RED));
		reg_shadow[CH2_RED] = get_reg(CH2_RED);
	}

	if ((reg_shadow[CH2_GREEN])!= get_reg(CH2_GREEN) || force)
	{
		configPWM(PWM_CH2,CH_GREEN,scale_value(get_reg(CH2_GREEN),settings.offset_green,settings.gain_green,GAMMA_GREEN));
		reg_shadow[CH2_GREEN] = get_reg(CH2_GREEN);
	}

	if ((reg_shadow[CH2_BLUE])!= get_reg(CH2_BLUE) || force)
	{
		configPWM(PWM_CH2,CH_BLUE,scale_value(get_reg(CH2_BLUE),settings.offset_blue,settings.gain_blue,GAMMA_BLUE));
		reg_shadow[CH2_BLUE] = get_reg(CH2_BLUE);
	}

	if ((reg_shadow[CH3_RED])!= get_reg(CH3_RED) || force)
	{
		configPWM(PWM_CH3,CH_RED,scale_value(get_reg(CH3_RED),settings.offset_red,settings.gain_red,GAMMA_RED));
		reg_shadow[CH3_RED] = get_reg(CH3_RED);
	}

	if ((reg_shadow[CH3_GREEN])!= get_reg(CH3_GREEN) || force)
	{
		configPWM(PWM_CH3,CH_GREEN,scale_value(get_reg(CH3_GREEN),settings.offset_green,settings.gain_green,GAMMA_GREEN));
		reg_shadow[CH3_GREEN] = get_reg(CH3_GREEN);
	}

	if ((reg_shadow[CH3_BLUE])!= get_reg(CH3_BLUE) || force)
	{
		configPWM(PWM_CH3,CH_BLUE,scale_value(get_reg(CH3_BLUE),settings.offset_blue,settings.gain_blue,GAMMA_BLUE));
		reg_shadow[CH3_BLUE] = get_reg(CH3_BLUE);
	}

	if ((reg_shadow[CH3_WHITE])!= get_reg(CH3_WHITE) || force)
	{
		configPWM(PWM_CH3,CH_WHITE,scale_value(get_reg(CH3_WHITE),0,100,GAMMA_STRIP));  //TBD, no settings?
		reg_shadow[CH3_WHITE] = get_reg(CH3_WHITE);
	}

}

void update_fx(void)
{
	uint8_t val = 0;

	//Deal with LED Strip Registers/Effects
	if (DMX_MODE2 == get_mode())
	{
		val = get_reg(FX_SELECT)/settings.fx_multiplier;
		//Activate FX if valid FX is selected or restore settings...
		if (reg_shadow[FX_SELECT]!= val)
		{
			val = start_fx(val);
			set_reg(FX_SELECT,val);
			reg_shadow[FX_SELECT] = val;
		}
	}
}
