#include "fx_api/fx_api.h"
#include "fx/fx_pwm_strip_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "settings.h"


float cnt = 0;
uint32_t step = 0;
static const s_fx_param param1 = {PWM_FX,MODE_CONTINOUS,0,"PSENG",0,fx_pwm_strip_engine_run};
const uint32_t colval[6] = {0x00FF0000,0x0000FF00,0x000000FF,0x00FFFF00, 0x0000FFFF,0x00FF00FF};

void fx_pwm_strip_engine(uint8_t fx_num)
{
	//Register effect
	register_fx(&param1, fx_num);
}



t_fx_result fx_pwm_strip_engine_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
{
	float s1,s2,s3;
	uint32_t rbcol;
	static uint32_t lcount =0;
	uint32_t loc;
	uint32_t colsel;
	static t_rgb_buf buf;

	switch(state)
	{
		case FX_INIT:
			reset_frame_delay();
			//Boldly hardcode the number of LED's in the strips...
			settings.strip1_length = 8 * 14;
			settings.strip2_length = 0;
			WS2812B_init(CH1,settings.strip1_length);
			WS2812B_init(CH2,settings.strip2_length);

			//Create a buffer for RGB Data
			if (0 ==create_rgb_buffer(&buf, WS2812B_numPixels(CH1)))
				return FX_ERROR;

			return FX_OK;
		case FX_RUN:
			//Toggle front light
			if (lcount>330)
				set_pwm_light(9, 0);
			else
				set_pwm_light(9, 255);

			lcount++;
			if (lcount>350)
				lcount = 0;

			//Add Delay
			if (!check_frame_delay(get_DMX_variable(DMX_STRIP1_SPEED))) //Hmmm.... Speed determines the length of the effect but that's fixed for Single Shot... Problem....
				return FX_RUNNING;

			//Calculate 3 sin values...
			s1 = sin(cnt);
			s1 = s1 + 1;
			s1 = s1 / 2;

			s2 = sin(cnt + ((float)get_DMX_variable(DMX_STRIP1_COMPLEXITY) / 100));
			s2 = s2 + 1;
			s2 = s2 / 2;

			s3 = sin(cnt + (((float)get_DMX_variable(DMX_STRIP1_COMPLEXITY) / 100) * 2));
			s3 = s3 + 1;
			s3 = s3 / 2;

			//increase count
			cnt = cnt + ((float)get_DMX_variable(DMX_STRIP1_SIZE) / 1000);
			if (cnt > (2.0 * M_PI))
				cnt = cnt - (2.0 * M_PI);

			//Copy to DMX Registers
			rbcol = WS2812B_colorWheel(step);
			set_pwm_light(0, (uint8_t)(s1 * (float)(rbcol>>16&0xFF)));
			set_pwm_light(1, (uint8_t)(s1 * (float)(rbcol>>8&0xFF)));
			set_pwm_light(2, (uint8_t)(s1 * (float)(rbcol&0xFF)));

			rbcol = WS2812B_colorWheel(step+20);
			set_pwm_light(3, (uint8_t)(s2 * (float)(rbcol>>16&0xFF)));
			set_pwm_light(4, (uint8_t)(s2 * (float)(rbcol>>8&0xFF)));
			set_pwm_light(5, (uint8_t)(s2 * (float)(rbcol&0xFF)));

			rbcol = WS2812B_colorWheel(step+40);
			set_pwm_light(6, (uint8_t)(s3 * (float)(rbcol>>16&0xFF)));
			set_pwm_light(7, (uint8_t)(s3 * (float)(rbcol>>8&0xFF)));
			set_pwm_light(8, (uint8_t)(s3 * (float)(rbcol&0xFF)));



			step++;
			if (step==256)
				step = 0;

			//Set random pixel to random color
			loc = simple_rnd();
			loc = scale_full(loc>>16,0xFFFF, 0, 8*14, 0);
			colsel = simple_rnd();
			colsel = scale_full(colsel>>16,0xFFFF, 0, 5, 0);

			set_buffer_pixel(&buf,loc, colval[colsel]>>16 & 0xFF, colval[colsel]>>8 & 0xFF, colval[colsel] & 0xFF);
			fade_rgb_buf(&buf,200);
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
