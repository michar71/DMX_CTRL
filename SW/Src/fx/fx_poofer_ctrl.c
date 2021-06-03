#include "fx_api/fx_api.h"
#include "fx/fx_poofer_ctrl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define STATE_OFF 0
#define STATE_START 1
#define STATE_T1 2
#define STATE_T2 3
#define STATE_T3 4
#define STATE_WAIT_RESET 5


static const s_fx_param param = {STRIP_FX,MODE_CONTINOUS,0,"POOFER",0,fx_pwm_poofer_run};

void fx_pwm_poofer(uint8_t fx_num)
{
	//Register effect
	register_fx(&param,fx_num);
}



t_fx_result fx_pwm_poofer_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
{
	static uint8_t running = 0;
	static uint8_t v1 = 0;
	static uint8_t v2 = 0;
    static uint8_t v3 = 0;
	static uint32_t last_ms = 0;
	static uint8_t poofer_state = STATE_OFF;
	uint8_t var;


	switch(state)
	{
		case FX_INIT:
			last_ms = millisec();

			//Stop DMX register updating PWM registers as it interferes
			disable_DMX_light_Update(true);
			//Mak Sure Poofer is off
			set_pwm_direct(0,0,0);
			return FX_OK;
		case FX_RUN:
			//Check if the poofer is activated
			var = get_DMX_variable(STRIP1_COMPLEXITY);
			//If trigger is set to 0 turn poofer off
			if (var == 0)
			{
				running = 0;
				set_pwm_direct(0,0,0);
				poofer_state = STATE_OFF;
				return FX_RUNNING;
			}

			//So we have been triggered...
			//Check mode
			var = get_DMX_variable(STRIP1_PATTERN);
			//Manual Mode
			if (var == 0)
			{
				running = 1;
				set_pwm_direct(0,0,65535);
				return FX_RUNNING;
			}
			else if ((var == 1) || (var == 2))  //Short/Long
			{
				if (poofer_state == STATE_OFF)
				{
					set_pwm_direct(0,0,0);
					if (running == 1)
					{
						return FX_RUNNING;
					}
					poofer_state = STATE_START;

				}
				else if (poofer_state == STATE_START)
				{
										//Load Variables
					v1 = get_DMX_variable(STRIP1_V1);
					v2 = get_DMX_variable(STRIP1_V2);
					v3 = get_DMX_variable(STRIP1_V3);
					//Get Time in MS
					last_ms = millisec();
					//Tuirn on poofer
					set_pwm_direct(0,0,65535);
					//Set Next State
					poofer_state = STATE_T1;
				}
				else if (poofer_state == STATE_T1)
				{
					set_pwm_direct(0,0,0);
					if (millisec() >= (last_ms + (v1*10)))
					{
						//Turn off poofer					
						poofer_state = STATE_T2;
					}
				}
				else if (poofer_state == STATE_T2)
				{
					set_pwm_direct(0,0,65535);
					if (millisec() >= (last_ms + (v1*10) + (v2*10)))
					{
						//Turn off poofer		
						if (var == 1)
							poofer_state = STATE_T3;
						else
							poofer_state = STATE_WAIT_RESET;
					}
				}
				else if (poofer_state == STATE_T3)
				{
					set_pwm_direct(0,0,0);
					if (millisec() >= (last_ms + (v1*10) + (v2*10) + (v3*10)))
					{
						//Turn off poofer			
						poofer_state = STATE_WAIT_RESET;
					}
				}
				else if (poofer_state == STATE_WAIT_RESET)
				{
					//Turn off poofer
					set_pwm_direct(0,0,0);
				}
			}

			return FX_RUNNING;
		case FX_END:
			running = 0;
			poofer_state = STATE_OFF;
			set_pwm_direct(0,0,0);
			//enable DMX register updating PWM registers 
			disable_DMX_light_Update(false);			
			return FX_COMPLETED;
		case FX_DONE:
			break;
	}
	return FX_ERROR;
}