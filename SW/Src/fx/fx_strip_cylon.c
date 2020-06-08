#include "fx_api/fx_api.h"
#include "fx/fx_strip_cylon.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265

static const s_fx_param param = {STRIP_FX,MODE_CONTINOUS,0,"SCYLON",0,fx_strip_cylon_run};

void fx_strip_cylon(uint8_t fx_num)
{
	//Register effect
	register_fx(&param,fx_num);
}


t_fx_result fx_strip_cylon_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
{
	static uint32_t last_ms;
	static uint16_t pos = 0;
	static t_rgb_buf buf;
	double res;
	uint16_t poscalc;
	uint8_t corr_r;
	uint8_t corr_g;
	uint8_t corr_b;

	switch(state)
	{
		case FX_INIT:
			last_ms = millisec();
			if (0 ==create_rgb_buffer(&buf, WS2812B_numPixels(CH1)))
				return FX_ERROR;

			return FX_OK;
		case FX_RUN:
			//Check if we need to step forward
			if ((millisec() - last_ms) < get_DMX_variable(DMX_STRIP1_SPEED))
					return FX_RUNNING;
			last_ms = millisec();

			//Dim all pixels
			fade_rgb_buf(&buf,get_DMX_variable(DMX_STRIP1_SIZE));

			//Calculate position
			res = sin (pos*PI/180);
			pos = pos + get_DMX_variable(DMX_STRIP1_COMPLEXITY);
			if (pos>360)
				pos = pos - 360;

			//res is between -1 and 1 so we need to scale things...
			res = res * (double)WS2812B_numPixels(CH1)/2;
			poscalc = (int16_t)res + (WS2812B_numPixels(CH1)/2);

			//Set Pixel wirg Gamma Correction
			corr_r = getGamma(GAMMA_STRIP,get_DMX_variable(DMX_STRIP1_V1));
			corr_g = getGamma(GAMMA_STRIP,get_DMX_variable(DMX_STRIP1_V2));
			corr_b = getGamma(GAMMA_STRIP,get_DMX_variable(DMX_STRIP1_V3));
			set_buffer_pixel(&buf,poscalc,corr_r,corr_g,corr_b);

			//Transfer buffer to actual pixels
			set_pixels_from_buf(CH1,&buf);

			return FX_RUNNING;
		case FX_END:
			//Clear all Pixel
			WS2812B_clear(CH1);
			WS2812B_clear(CH2);
			destroy_rgb_buffer(&buf);
			return FX_COMPLETED;
		case FX_DONE:
			break;
	}
	return FX_ERROR;
}
