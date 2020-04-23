/*
 * fx_api.c
 *
 *  Created on: Apr 7, 2020
 *      Author: Michael
 */
#include "fx_api/fx_api.h"
#include "pwm_control.h"
#include <stm32f1xx.h>
#include "triggers.h"

uint32_t delay_count = 0;
const t_dmx_var DMX_CH_REG[CH_MAX][DMX_STRIP_MAX] = {{DMX_STRIP1_PATTERN,DMX_STRIP1_SPEED,DMX_STRIP1_SIZE,DMX_STRIP1_COMPLEXITY,DMX_STRIP1_V1,DMX_STRIP1_V2,DMX_STRIP1_V3},
													 {DMX_STRIP2_PATTERN,DMX_STRIP2_SPEED,DMX_STRIP2_SIZE,DMX_STRIP2_COMPLEXITY,DMX_STRIP2_V1,DMX_STRIP2_V2,DMX_STRIP2_V3}};


//Reset the Frame Delay Counter
void reset_frame_delay(void)
{
	 delay_count = 0;
}

//Returns true if "delay" frames have passed, else false
uint8_t check_frame_delay(uint32_t delay)
{
	if (delay == delay_count)
	{
		delay_count = 0;
		return 1;
	}
	delay_count++;
	return 0;
}


//Custom Delay with on Variable
uint8_t check_custom_frame_delay(uint32_t* count, uint32_t delay)
{
	if (delay == *count)
	{
		*count = 0;
		return 1;
	}
	*count = *count + 1;
	return 0;
}


//Set Light by index (0..9)
void set_pwm_light(uint8_t index, uint8_t val)
{
	set_reg(index+1, val);
}

//Set RGB lights by channel (0..2)
void set_pwm_timer_channel(uint8_t ch, uint8_t r, uint8_t g, uint8_t b)
{
	set_reg((ch*3)+1, r);
	set_reg((ch*3)+2, g);
	set_reg((ch*3)+3, b);
}

//Set Overall PWM Brightness
void set_pwm_brightness(uint8_t val)
{
	set_reg(MAX_BRIGHTNESS, val);
}

//Set Overall STRIP Brightness
void set_strip_brightness(uint8_t val)
{
	WS2812B_setBrightness(CH1,val);
	WS2812B_setBrightness(CH2,val);
}

//Get a DMX Variable
uint8_t get_DMX_variable(t_dmx_var var)
{
	return get_reg((uint32_t)var);
}

//Set a DMX Variable
void set_DMX_variable(t_dmx_var var, uint8_t val)
{
	set_reg((uint32_t)var,val);
}



uint8_t get_tigger_val(uint8_t ch)
{
	return get_trigger(ch);
}

uint32_t millisec(void)
{
	return HAL_GetTick();
}
