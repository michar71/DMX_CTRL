/*
 * light_update.c
 *
 *  Created on: Feb 18, 2020
 *      Author: Michael Hartmann
 */
#include "light_update.h"
#include "pwm_control.h"
#include "dmx512_config.h"
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

	reg_shadow = malloc(get_reg_length()+1);
	memset(reg_shadow,0,get_reg_length());
	reg_shadow[MAX_BRIGHTNESS] = 255;
	update_pwm_lights(1);
}

uint16_t scale_value( uint8_t val)
{
	uint32_t res = 0;
	//1) Scale input between 0 and max
	res = ((uint32_t)val * (uint32_t) MAX_PWM)/(uint32_t)255;
	res = (res * (uint32_t)brightness_scale) / (uint32_t) MAX_PWM;
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
		configPWM(PWM_CH1,CH_RED,scale_value(get_reg(CH1_RED)));
		reg_shadow[CH1_RED] = get_reg(CH1_RED);
	}

	if ((reg_shadow[CH1_GREEN])!= get_reg(CH1_GREEN) || force)
	{
		configPWM(PWM_CH1,CH_GREEN,scale_value(get_reg(CH1_GREEN)));
		reg_shadow[CH1_GREEN] = get_reg(CH1_GREEN);
	}

	if ((reg_shadow[CH1_BLUE])!= get_reg(CH1_BLUE) || force)
	{
		configPWM(PWM_CH1,CH_BLUE,scale_value(get_reg(CH1_BLUE)));
		reg_shadow[CH1_BLUE] = get_reg(CH1_BLUE);
	}

	if ((reg_shadow[CH2_RED])!= get_reg(CH2_RED) || force)
	{
		configPWM(PWM_CH2,CH_RED,scale_value(get_reg(CH2_RED)));
		reg_shadow[CH2_RED] = get_reg(CH2_RED);
	}

	if ((reg_shadow[CH2_GREEN])!= get_reg(CH2_GREEN) || force)
	{
		configPWM(PWM_CH2,CH_GREEN,scale_value(get_reg(CH2_GREEN)));
		reg_shadow[CH2_GREEN] = get_reg(CH2_GREEN);
	}

	if ((reg_shadow[CH2_BLUE])!= get_reg(CH2_BLUE) || force)
	{
		configPWM(PWM_CH2,CH_BLUE,scale_value(get_reg(CH2_BLUE)));
		reg_shadow[CH2_BLUE] = get_reg(CH2_BLUE);
	}

	if ((reg_shadow[CH3_RED])!= get_reg(CH3_RED) || force)
	{
		configPWM(PWM_CH3,CH_RED,scale_value(get_reg(CH3_RED)));
		reg_shadow[CH3_RED] = get_reg(CH3_RED);
	}

	if ((reg_shadow[CH3_GREEN])!= get_reg(CH3_GREEN) || force)
	{
		configPWM(PWM_CH3,CH_GREEN,scale_value(get_reg(CH3_GREEN)));
		reg_shadow[CH3_GREEN] = get_reg(CH3_GREEN);
	}

	if ((reg_shadow[CH3_BLUE])!= get_reg(CH3_BLUE) || force)
	{
		configPWM(PWM_CH3,CH_BLUE,scale_value(get_reg(CH3_BLUE)));
		reg_shadow[CH3_BLUE] = get_reg(CH3_BLUE);
	}

	if ((reg_shadow[CH3_WHITE])!= get_reg(CH3_WHITE) || force)
	{
		configPWM(PWM_CH3,CH_WHITE,scale_value(get_reg(CH3_WHITE)));
		reg_shadow[CH3_WHITE] = get_reg(CH3_WHITE);
	}
	//Deal with LED Strip Registers
	if (DMX_MODE2 == get_mode())
	{

	}
}
