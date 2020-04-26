#include "fx_api/fx_api.h"
#include "fx/fx_strip_rainbow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const s_fx_param param = {STRIP_FX,MODE_CONTINOUS,0,"SRAINBOW",0,fx_strip_rainbow_run};

void fx_strip_rainbow(uint8_t fx_num)
{
	//Register effect
	register_fx(&param,fx_num);
}


t_fx_result fx_strip_rainbow_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
{
	static int16_t pos[CH_MAX] = {0,0};
	static int16_t step[CH_MAX] = {0,0};
	static uint32_t delay[CH_MAX] = {0,0};

	switch(state)
	{
		case FX_INIT:
			//Loop through both channels
			for (uint8_t ii = 0;ii<CH_MAX;ii++)
			{
				step[ii] = get_DMX_variable(DMX_CH_REG[ii][DMX_STRIP_SIZE]);
				if (get_DMX_variable(DMX_CH_REG[ii][DMX_STRIP_PATTERN]))
					step[ii] = -step[ii];
			}

			return FX_OK;
		case FX_RUN:
			//Loop through both channels
			for (uint8_t ii = 0;ii<CH_MAX;ii++)
			{
				//Add Delay
				if (check_custom_frame_delay(&delay[ii],get_DMX_variable(DMX_CH_REG[ii][DMX_STRIP_SPEED]))) //Hmmm.... Speed determines the length of the effect but that's fixed for Single Shot... Problem....
				{
					//Create Rainbow Effect
					WS2812B_rainbow(ii,0, WS2812B_numPixels(ii),pos[ii],step[ii]);

					//Determine next pixel
					pos[ii] = pos[ii] + step[ii];
					if (pos[ii] > 255)
					{
						pos[ii] = pos[ii]-255;
					}
					else if (pos[ii] < 0)
					{
						pos[ii] = pos[ii]+255;
					}
				}
			}

			return FX_RUNNING;
		case FX_END:
			//Clear all Pixel
			WS2812B_clear(CH1);
			WS2812B_clear(CH2);
			return FX_COMPLETED;
		case FX_DONE:
			break;
	}
	return FX_ERROR;
}
