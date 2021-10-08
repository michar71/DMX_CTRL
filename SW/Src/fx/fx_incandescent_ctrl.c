#include "fx_api/fx_api.h"
#include "fx/fx_incandescent_ctrl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static const s_fx_param param = {PWM_FX,MODE_CONTINOUS,0,"INCAN",0,fx_pwm_incandescent_run};

void fx_pwm_incandescent(uint8_t fx_num)
{
	//Register effect
	register_fx(&param,fx_num);
}


t_fx_result fx_pwm_incandescent_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
{
	static uint32_t event_cnt = 0;
	static uint32_t dur_cnt = 0;
	uint8_t enabled = 0;

	switch(state)
	{
		case FX_INIT:
			//Stop DMX register updating PWM registers as it interferes
			disable_DMX_light_Update(true);
			//Make Sure Poofer is off
			set_pwm_direct(0,0,0);
			return FX_OK;
		case FX_RUN:
			//Check if there is no pending event
			if ((event_cnt == 0) && (dur_cnt == 0))
			{
				//generate duration
				dur_cnt = simple_rnd();
				dur_cnt = dur_cnt / (get_DMX_variable(STRIP1_COMPLEXITY)*255);
				enabled = 1;

				//generate next event count
			}
			else if ((event_cnt == 0) && (dur_cnt != 0))
			{
				enabled = 1;
				dur_cnt--;
				if (dur_cnt == 0)
				{
					event_cnt = simple_rnd();
					event_cnt = event_cnt / get_DMX_variable(STRIP1_SPEED);
					enabled = 1;
				}
			}
			else if ((event_cnt != 0) && (dur_cnt == 0))
			{
				enabled = 0;
				event_cnt--;
			}


			if (enabled == 0)
			{
				set_pwm_direct(0,0,0);
			}
			else
			{
				//Turn Poofer On...
				set_pwm_direct(0,0,65535);
			}
			return FX_RUNNING;

		case FX_END:
			set_pwm_direct(0,0,0);
			//enable DMX register updating PWM registers 
			disable_DMX_light_Update(false);			
			return FX_COMPLETED;
		case FX_DONE:
			break;
	}
	return FX_ERROR;
}
