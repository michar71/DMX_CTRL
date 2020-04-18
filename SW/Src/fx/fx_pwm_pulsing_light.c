#include "fx_api/fx_api.h"
#include "fx/fx_pwm_pulsing_light.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t data;
uint8_t state; //0= off, 1 = ramp up, 2 = on, 3 = ramp down
void fx_pwm_pulsing_light(void)
{
	//Register effect
	s_fx_param param;
	param.duration = 0;
	param.next_fx = 0;
	param.mode = MODE_CONTINOUS;
	param.type = PWM_FX;
	param.fx_run_pointer = fx_pwm_pulsing_light_run;
	register_fx(param);
}

void fx_pwm_pulsing_pulse_light(void)
{
	//Register effect
	s_fx_param param;
	param.duration = 10;
	param.next_fx = 0;
	param.mode = MODE_SINGLE_SHOT;
	param.type = PWM_FX;
	param.fx_run_pointer = fx_pwm_pulsing_light_run;
	register_fx(param);
}

t_fx_result fx_pwm_pulsing_light_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
{
	uint8_t ii = 0;
	int16_t val;

	switch(state)
	{
		case FX_INIT:
			data = 0;
			state = 0;
			reset_frame_delay();
			return FX_OK;
		case FX_RUN:


			//Go through states
			if (state == 0)  //Off
			{
				data = 0;
				//Add Delay
				if (!check_frame_delay(get_DMX_variable(DMX_STRIP1_SPEED)))
					state++;
			}
			else if (state == 1)	//Ramp Up
			{
				val = data + get_DMX_variable(DMX_STRIP1_COMPLEXITY);
				if (val>255)
				{
					state++;
					data = 255;
				}
				else
				{
					data = val;
				}
			}
			else if (state == 2)  //On
			{
				data = 255;
				//Add Delay
				if (!check_frame_delay(get_DMX_variable(DMX_STRIP1_SPEED)))
					state++;
			}
			else	//Ramp Down
			{
				val = data - get_DMX_variable(DMX_STRIP1_V1);
				if (val<0)
				{
					state=0;
					data = 0;
				}
				else
				{
					data = val;
				}
			}

			//Get number of lights to use
			val = get_DMX_variable(DMX_STRIP1_SIZE);
			if (val>9)
				val = 9;

			//Copy to DMX Registers
			for (ii=0;ii<val+1;ii++)
				set_pwm_light(ii, data);
			return FX_RUNNING;
		case FX_END:
			return FX_COMPLETED;
		case FX_DONE:
			break;
	}
	return FX_ERROR;
}
