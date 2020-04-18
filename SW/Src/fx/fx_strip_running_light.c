#include "fx_api/fx_api.h"
#include "fx/fx_strip_running_light.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void fx_strip_running_light(void)
{
	//Register effect
	s_fx_param param;
	param.duration = 0;
	param.next_fx = 0;
	param.mode = MODE_CONTINOUS;
	param.type = STRIP_FX;
	param.fx_run_pointer = fx_strip_running_light_run;
	register_fx(param);
}


t_fx_result fx_strip_running_light_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
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
				if (0 != get_DMX_variable(DMX_CH_REG[ii][DMX_STRIP_PATTERN]))
				{
				  pos[ii] = WS2812B_numPixels((t_stripchannel)ii);
				  step[ii] = -step[ii];
				}
			}

			return FX_OK;
		case FX_RUN:
			//Loop through both channels
			for (uint8_t ii = 0;ii<CH_MAX;ii++)
			{
				//Add Delay
				if (check_custom_frame_delay(&delay[ii],get_DMX_variable(DMX_CH_REG[ii][DMX_STRIP_SPEED]))) //Hmmm.... Speed determines the length of the effect but that's fixed for Single Shot... Problem....
				{
					//Dim Lights TODO Tricky, No Idea how to do that without readback...
					/*
					for (ii=0;ii<10;ii++)
					{
						val = data[ii] - (255 - get_DMX_variable(DMX_STRIP1_COMPLEXITY));
						if (val<0)
							val = 0;
						data[ii] = (uint8_t)val;
					}
					*/

					//Clear all Pixel
					WS2812B_clear((t_stripchannel)ii);

					//Set Pixel
					WS2812B_setPixelColor((t_stripchannel)ii,pos[ii],get_DMX_variable(DMX_CH_REG[ii][DMX_STRIP_V1]), get_DMX_variable(DMX_CH_REG[ii][DMX_STRIP_V2]), get_DMX_variable(DMX_CH_REG[ii][DMX_STRIP_V3]));

					//
					pos[ii] = pos[ii] + step[ii];
					if (pos[ii] > WS2812B_numPixels((t_stripchannel)ii))
					{
						step[ii] = -step[ii];
						pos[ii] = pos[ii] + step[ii];
					}
					else if (pos[ii] < 0)
					{
						step[ii] = -step[ii];
						pos[ii] = pos[ii] + step[ii];
					}
				}
			}

			return FX_RUNNING;
		case FX_END:
			return FX_COMPLETED;
		case FX_DONE:
			break;
	}
	return FX_ERROR;
}
