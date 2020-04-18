#include "fx_api/fx_api.h"
#include "fx/fx_strip_running_light.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int16_t pos[CH_MAX];
int16_t step[CH_MAX];

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
	switch(state)
	{
		case FX_INIT:
			//Allocate variables from stack. We'll do that dynamically in effects otherwise we cut away more and more static memory.
			//If needed we can increase the stack-size.
			//We allocate one more field at the end to store the current index.
			step[CH1] = get_DMX_variable(DMX_STRIP1_SIZE);
			if (0 == get_DMX_variable(DMX_STRIP1_PATTERN))
			{
			  pos[CH1] = 0;
			}
			else
			{
			  pos[CH1] = WS2812B_numPixels(CH1);
			  step[CH1] = -step[CH1];
			}

			step[CH2] = get_DMX_variable(DMX_STRIP2_SIZE);
			if (0 == get_DMX_variable(DMX_STRIP2_PATTERN))
			{
				pos[CH2] = 0;
			}
			else
			{
			  pos[CH2] = WS2812B_numPixels(CH2);
			  step[CH2] = -step[CH2];
			}
			reset_frame_delay();
			return FX_OK;
		case FX_RUN:
			//Add Delay
			if (check_frame_delay(get_DMX_variable(DMX_STRIP1_SPEED))) //Hmmm.... Speed determines the length of the effect but that's fixed for Single Shot... Problem....
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
				WS2812B_clear(CH1);

				//Set Pixel
				WS2812B_setPixelColor(CH1,pos[CH1],get_DMX_variable(DMX_STRIP1_V1), get_DMX_variable(DMX_STRIP1_V2), get_DMX_variable(DMX_STRIP1_V3));

				//
				pos[CH1] = pos[CH1] + step[CH1];
				if (pos[CH1] > WS2812B_numPixels(CH1))
				{
					step[CH1] = -step[CH1];
					pos[CH1] = pos[CH1] + step[CH1];
				}
				else if (pos[CH1] < 0)
				{
					step[CH1] = -step[CH1];
					pos[CH1] = pos[CH1] + step[CH1];
				}
			}

			//Add Delay
			if (check_frame_delay(get_DMX_variable(DMX_STRIP2_SPEED))) //Hmmm.... Speed determines the length of the effect but that's fixed for Single Shot... Problem....
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
				WS2812B_clear(CH2);

				//Set Pixel
				WS2812B_setPixelColor(CH2,pos[CH2],get_DMX_variable(DMX_STRIP2_V1), get_DMX_variable(DMX_STRIP2_V2), get_DMX_variable(DMX_STRIP2_V3));

				//
				pos[CH2] = pos[CH2] + step[CH2];
				if (pos[CH2] > WS2812B_numPixels(CH2))
				{
					step[CH2] = -step[CH2];
					pos[CH2] = pos[CH2] + step[CH2];
				}
				else if (pos[CH2] < 0)
				{
					step[CH2] = -step[CH2];
					pos[CH2] = pos[CH2] + step[CH2];
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
