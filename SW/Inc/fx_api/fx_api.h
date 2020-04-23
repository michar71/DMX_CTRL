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
#include "WS2812B/WS2812B.h"


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


typedef enum{
	DMX_STRIP_PATTERN,
	DMX_STRIP_SPEED,
	DMX_STRIP_SIZE,
	DMX_STRIP_COMPLEXITY,
	DMX_STRIP_V1,
	DMX_STRIP_V2,
	DMX_STRIP_V3,
	DMX_STRIP_MAX
}t_dmx_ch_var;

//Use these to Grab access to a whole strip register block
typedef enum{
	DMX_STRIP1_REG_START = DMX_STRIP1_PATTERN,
	DMX_STRIP2_REG_START = DMX_STRIP2_PATTERN
}t_dmx_strip_var;

extern const t_dmx_var DMX_CH_REG[CH_MAX][DMX_STRIP_MAX];


//Delay Function
//--------------


//Reset the Frame Delay Counter
void reset_frame_delay(void);

//Returns true if "delay" frames have passed, else false
uint8_t check_frame_delay(uint32_t delay);

//Custom Delay with own variable
uint8_t check_custom_frame_delay(uint32_t* count, uint32_t delay);

//PWM Control Functions
//---------------------

//Set Light by index (0..9)
void set_pwm_light(uint8_t index, uint8_t val);

//Set RGB lights by channel (0..2)
void set_pwm_timer_channel(uint8_t ch, uint8_t r, uint8_t g, uint8_t b);

//Set Overall Brightness
void set_pwm_brightness(uint8_t val);
void set_strip_brightness(uint8_t val);

//DMX Specific Functions
//----------------------

//Get a DMX Variable
uint8_t get_DMX_variable(t_dmx_var var);

//Set a DMX Variable
void set_DMX_variable(t_dmx_var var, uint8_t val);

//System Specific Functions
//--------------------------

uint8_t get_tigger_val(uint8_t ch);
uint32_t millisec(void);


#endif /* FX_API_H_ */