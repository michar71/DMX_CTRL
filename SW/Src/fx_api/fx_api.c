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
#include"light_update.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t x = 0, w = 0, s = 0xb5ad4eceda1ce2a9;
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
	return check_custom_frame_delay(&delay_count, delay);
}


//Custom Delay with on Variable
uint8_t check_custom_frame_delay(uint32_t* count, uint32_t delay)
{
	if (*count >= delay)
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

//Set PWM Value direct
void set_pwm_direct(uint8_t id, uint8_t ch, uint16_t val)
{
	setPWMdirect(id, ch, val);
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

uint16_t scale_full(uint16_t in,uint16_t in_max, uint16_t in_min, uint16_t out_max, uint16_t out_min)
{
	return (uint16_t) (((uint32_t)(in-in_min)  * (uint32_t)(out_max - out_min))/ (uint32_t)(in_max - in_min) + (uint32_t)out_min);
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
		if (pix[i].r < 2)
			pix[i].r = 0;
		pix[i].g = scale256(pix[i].g, scale);
		if (pix[i].g < 2)
			pix[i].g = 0;
		pix[i].b = scale256(pix[i].b, scale);
		if (pix[i].b < 2)
			pix[i].b = 0;
	}
}


//Very simple pseudo-random number generator.
//Returns a number 0x00000000..0xFFFFFFFF
//This is a pseudo-random sequence and will always start the same....

uint32_t simple_rnd()
{
   x *= x;
   x += (w += s);
   return x = (x>>32) | (x<<32);
}

void disable_DMX_light_Update(uint8_t val)
{
	disableDMXlightUpdate(val);
}
