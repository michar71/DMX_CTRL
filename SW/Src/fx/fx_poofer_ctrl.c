#include "fx_api/fx_api.h"
#include "fx/fx_poofer_ctrl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static const s_fx_param param = {STRIP_FX,MODE_CONTINOUS,0,"POOFER",0,fx_pwm_poofer_run};

void fx_pwm_poofer(uint8_t fx_num)
{
	//Register effect
	register_fx(&param,fx_num);
}

//After 10 Sec Poofer will turn off and needs to be reneabled by setting COMPLEXITY to 0...
#define SAFTY_TIMEOUT 10000

t_fx_result fx_pwm_poofer_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
{
	static uint8_t enabled = 0;
	static uint8_t lockout = 0;
	static uint32_t last_ms = 0;
	uint8_t var;

	switch(state)
	{
		case FX_INIT:
			//Stop DMX register updating PWM registers as it interferes
			disable_DMX_light_Update(true);
			last_ms = millisec();
			//Make Sure Poofer is off
			set_pwm_direct(0,0,0);
			return FX_OK;
		case FX_RUN:
			//Check if the poofer is activated
			var = get_DMX_variable(STRIP1_COMPLEXITY);


			//If trigger is 255 turn poofer on
			if (var == 255)
			{
				//Hmm.. .Poofer is already enabled... Check max burn time/safty timeout
					if ((enabled == 1) && (millisec() > (last_ms + SAFTY_TIMEOUT)))
					{
						lockout = 1;
					}
					else
					{
						last_ms = millisec();
						enabled = 1;
					}		
			}
			else
			{
				enabled = 0;
				lockout = 0;
			}


			if ((enabled == 1)&& (lockout == 0))
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