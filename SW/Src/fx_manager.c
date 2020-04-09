/*
 * fx_manager.c
 *
 *  Created on: Apr 7, 2020
 *      Author: Michael
 */

#include "fx_manager.h"
#include "fx_list.h"
#include <stddef.h>

uint8_t current_fx = 0;
uint8_t last_fx = 0;
uint32_t fx_frame_count = 0;
t_fx_state current_fx_state = INIT;

extern s_fx_param fx_list[MAX_FX];
extern uint8_t fxcnt;

void install_fx(void)
{
	//REGISTER FX HERE !!!

}


//Called to start a new effect. Return the effect that was actually enabled
uint8_t start_fx(uint8_t id)
{
	//FX ID Has changed, check if the new ID is bigger then the total list...
	if (id <= fxcnt)
	{
		//Call the last FX once with the END State
		if (current_fx_state != DONE)
		{
			if (NULL != fx_list[current_fx].fx_run_pointer)
			{
				fx_list[current_fx].fx_run_pointer(END,fx_list[current_fx].duration,0);
				current_fx_state = DONE;
			}
		}
		//ID is valid, setup parameters
		fx_frame_count = 0;

		//Store current_fx
		last_fx = current_fx;
		current_fx = id;

		//Call run routine once with INIT state
		if (NULL != fx_list[current_fx].fx_run_pointer)
		{
			fx_list[current_fx].fx_run_pointer(INIT,0,fx_list[current_fx].duration);
			current_fx_state = INIT;
		}
	}
	//return new ID
	return current_fx;
}

//Called every frame
void run_fx(void)
{
	//No FX Active...
	if (current_fx == 0)
		return;

	//Check if FX Has expired
	if (fx_list[current_fx].mode == MODE_SINGLE_SHOT)
	{
		if (fx_list[current_fx].duration == fx_frame_count)
		{
			fx_list[current_fx].fx_run_pointer(END,fx_list[current_fx].duration,0);
			current_fx_state = DONE;
			current_fx = last_fx;
			return;
		}
	}
	//Or needs to be looped...
	else if (fx_list[current_fx].mode == MODE_LOOP)
	{
		if (fx_list[current_fx].duration == fx_frame_count)
		{
			fx_frame_count = 0;
			fx_list[current_fx].fx_run_pointer(INIT,fx_list[current_fx].duration,0);
			current_fx_state = INIT;
		}
	}
	//Call FX
	fx_list[current_fx].fx_run_pointer(RUN,fx_frame_count,fx_list[current_fx].duration);
	current_fx_state = RUN;
	fx_frame_count++;
}


