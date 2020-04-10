/*
 * fx_api.h
 *
 *  Created on: Apr 7, 2020
 *      Author: Michael
 */

#ifndef FX_API_H_
#define FX_API_H_

#include <stdint.h>
#include "fx_list.h"
#include "dmx512_config.h"


typedef enum{
	DMX_MAX_BRIGHTNESS = 0,
	DMX_CH1_RED,
	DMX_CH1_GREEN,
	DMX_CH1_BLUE,
	DMX_CH2_RED,
	DMX_CH2_GREEN,
	DMX_CH2_BLUE,
	DMX_CH3_RED,
	DMX_CH3_GREEN,
	DMX_CH3_BLUE,
	DMX_CH3_WHITE,
	DMX_FX_SELECT,
	DMX_STRIP1_PATTERN,
	DMX_STRIP1_SPEED,
	DMX_STRIP1_SIZE,
	DMX_STRIP1_COMPLEXITY,
	DMX_STRIP1_V1,
	DMX_STRIP1_V2,
	DMX_STRIP1_V3,
	DMX_STRIP2_PATTERN,
	DMX_STRIP2_SPEED,
	DMX_STRIP2_SIZE,
	DMX_STRIP2_COMPLEXITY,
	DMX_STRIP2_V1,
	DMX_STRIP2_V2,
	DMX_STRIP2_V3
}t_dmx_var;

//Delay Function
//--------------

//Reset the Frame Delay Counter
void reset_frame_delay(void);

//Returns true if "delay" frames have passed, else false
uint8_t check_frame_delay(uint32_t delay);

//PWM Control Functions
//---------------------

//Set Light by index (0..9)
void set_pwm_light(uint8_t index, uint8_t val);

//Set RGB lights by channel (0..2)
void set_pwm_timer_channel(uint8_t ch, uint8_t r, uint8_t g, uint8_t b);

//Set Overall Brightness
void set_pwm_brightness(uint8_t val);

//DMX Specific Functions
//----------------------

//Get a DMX Variable
uint8_t get_DMX_variable(t_dmx_var var);


#endif /* FX_API_H_ */
