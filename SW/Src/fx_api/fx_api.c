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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


uint8_t scale256(uint8_t val, uint8_t scale)
{
	return (uint8_t)(((uint16_t)val * (uint16_t)scale)>>8);
}

uint8_t create_rgb_buffer(t_rgb_buf* pbuf, uint16_t size)
{
	pbuf->size = 0;
	pbuf->rgb_pixels = calloc(size, sizeof(t_rgb));
	if (NULL != pbuf->rgb_pixels)
	{
		pbuf->size = size;
		return 1;
	}
	return 0;
}

void destroy_rgb_buffer(t_rgb_buf* pbuf)
{
	free(pbuf->rgb_pixels);
}

void set_buffer_pixel(t_rgb_buf* pbuf,uint16_t pos, uint8_t r, uint8_t g, uint8_t b)
{
	t_rgb *pix = pbuf->rgb_pixels;
	pix[pos].r = r;
	pix[pos].g = g;
	pix[pos].b = b;
}

void set_pixels_from_buf(t_stripchannel ch, t_rgb_buf* pbuf)
{
	uint16_t cnt;
	uint16_t maxcnt = WS2812B_numPixels(ch);
	t_rgb *pix = pbuf->rgb_pixels;

	if (maxcnt>pbuf->size)
		maxcnt = pbuf->size;

	for (cnt = 0;cnt<maxcnt;cnt++)
	{
		WS2812B_setPixelColor(ch,cnt,pix[cnt].r,pix[cnt].g,pix[cnt].b);
	}
}

void fade_rgb_buf(t_rgb_buf* pbuf,uint8_t scale)
{
	uint16_t i;
	t_rgb *pix = pbuf->rgb_pixels;

	for (i = 0; i<pbuf->size;i++)
	{
		pix[i].r = scale256(pix[i].r, scale);
		pix[i].g = scale256(pix[i].g, scale);
		pix[i].b = scale256(pix[i].b, scale);
	}
}
