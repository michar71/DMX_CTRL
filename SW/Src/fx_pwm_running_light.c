#include "fx_api.h"
#include "fx_pwm_running_light.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t* data;

void fx_pwm_running_light(void)
{
	//Register effect
	s_fx_param param;
	param.duration = 0;
	param.next_fx = 0;
	param.mode = MODE_CONTINOUS;
	param.type = PWM_FX;
	param.fx_run_pointer = fx_pwm_running_light_run;
	register_fx(param);
}

void fx_pwm_running_pulse_light(void)
{
	//Register effect
	s_fx_param param;
	param.duration = 10;
	param.next_fx = 0;
	param.mode = MODE_SINGLE_SHOT;
	param.type = PWM_FX;
	param.fx_run_pointer = fx_pwm_running_light_run;
	register_fx(param);
}

t_fx_result fx_pwm_running_light_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
{
	uint8_t ii = 0;
	int16_t val;

	switch(state)
	{
		case FX_INIT:
			//Allocate variables from stack. We'll do that dynamically in effects otherwise we cut away more and more static memory.
			//If needed we can increase the stack-size.
			//We allocate one more field at the end to store the current index.
			data = malloc(11);
			memset(data, 0, 11);
			reset_frame_delay();
			return FX_OK;
		case FX_RUN:
			//Add Delay
			if (!check_frame_delay(get_DMX_variable(DMX_STRIP1_SPEED))) //Hmmm.... Speed determines the length of the effect but that's fixed for Single Shot... Problem....
				return FX_RUNNING;

			//Dim Lights
			for (ii=0;ii<10;ii++)
			{
				val = data[ii] - (255 - get_DMX_variable(DMX_STRIP1_COMPLEXITY));
				if (val<0)
					val = 0;
				data[ii] = (uint8_t)val;
			}

			//Get number of lights to use
			ii = get_DMX_variable(DMX_STRIP1_SIZE);
			if (ii>9)
			   ii = 9;

			//Calculate next index
			if (get_DMX_variable(DMX_STRIP1_PATTERN))	//Reverse
			{
				if (data[10] == 0)
					data[10] = ii + 1;
				data[10]--;
			}
			else										//Forward
			{
				if (data[10] == ii)
					data[10] = 255;
				data[10]++;
			}
			//Turn next index to full
			data[data[10]] = 255;

			//Copy to DMX Registers
			for (ii=0;ii<10;ii++)
				set_pwm_light(ii, data[ii]);
			return FX_RUNNING;
		case FX_END:
			free(data);
			return FX_COMPLETED;
		case FX_DONE:
			break;
	}
	return FX_ERROR;
}
