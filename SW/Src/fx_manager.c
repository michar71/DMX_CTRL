/*
 * fx_manager.c
 *
 *  Created on: Apr 7, 2020
 *      Author: Michael
 */

#include "fx_manager.h"
#include "fx_list.h"
#include "fx/fx_install.h"
#include <stddef.h>
#include "shell.h"


uint8_t current_fx = 0;
uint8_t last_fx = 0;
uint32_t fx_frame_count = 0;
t_fx_state current_fx_state = FX_INIT;

extern const s_fx_param* pfx_list[MAX_FX];
extern uint8_t fxcnt;

void install_fx(void)
{
	fx_install();
}


//Called to start a new effect. Return the effect that was actually enabled
uint8_t start_fx(uint8_t id)
{

	//Check if FX is installed (We assume at future calls to the routine that the pointer is valid...
	if ((NULL != pfx_list[id]->fx_run_pointer) || (id == 0))
	{
		//Call the last FX once with the END State
		if (current_fx_state != FX_DONE)
		{
			if (NULL != pfx_list[current_fx]->fx_run_pointer)
				pfx_list[current_fx]->fx_run_pointer(FX_END,pfx_list[current_fx]->duration,0);
			print("Completed FX ID:%d\t FX: %s ",current_fx,pfx_list[current_fx]->fxname);
			current_fx_state = FX_DONE;
		}
		//ID is valid, setup parameters
		fx_frame_count = 0;
		last_fx = current_fx;
		current_fx = id;
		if (id == 0)
			return current_fx;

		//Call run routine once with INIT state
		pfx_list[current_fx]->fx_run_pointer(FX_INIT,0,pfx_list[current_fx]->duration);
		current_fx_state = FX_INIT;
		print("Started FX ID:%d\t FX: %s ",current_fx,pfx_list[current_fx]->fxname);
	}
	//return new ID
	return current_fx;
}


void fx_done(void)
{
	pfx_list[current_fx]->fx_run_pointer(FX_END,pfx_list[current_fx]->duration,0);
	print("Completed FX ID:%d\t FX: %s ",current_fx,pfx_list[current_fx]->fxname);
	current_fx_state = FX_DONE;
	if (pfx_list[current_fx]->next_fx != 0)
	{
		//Start Next FX
		start_fx(pfx_list[current_fx]->next_fx);
	}
	else
	{
		//Go back to last FX
		start_fx(last_fx);
	}
	return;
}

//Called every frame
void run_fx(void)
{
	//No FX Active...
	if (current_fx == 0)
		return;

	//Check if FX Has expired
	if (pfx_list[current_fx]->mode == MODE_SINGLE_SHOT)
	{
		if (pfx_list[current_fx]->duration == fx_frame_count)
		{
			fx_done();
			return;
		}
	}
	//Or needs to be looped...
	else if (pfx_list[current_fx]->mode == MODE_LOOP)
	{
		if (pfx_list[current_fx]->duration == fx_frame_count)
		{
			fx_frame_count = 0;
			pfx_list[current_fx]->fx_run_pointer(FX_INIT,pfx_list[current_fx]->duration,0);
			current_fx_state = FX_INIT;
		}
	}

	//Call FX
	if (FX_COMPLETED == pfx_list[current_fx]->fx_run_pointer(FX_RUN,fx_frame_count,pfx_list[current_fx]->duration))
	{
		//FX Self-ended...
		fx_done();
		return;
	}

	current_fx_state = FX_RUN;
	fx_frame_count++;
}


