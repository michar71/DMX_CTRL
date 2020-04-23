/*
 * fx_list.h
 *
 *  Created on: Apr 7, 2020
 *      Author: Michael
 */

#ifndef FX_LIST_H_
#define FX_LIST_H_

#include <stdint.h>

#define MAX_FX 64 //Limit list to 64 Entries for the moment to save RAM...

//Type of Return Values
typedef enum{
	FX_OK,
	FX_COMPLETED,
	FX_RUNNING,
	FX_ERROR
}t_fx_result;

//State for the current function call. It is guaranteed that
//FX_INIT will be executed once on start and FX_END once effect ends.
typedef enum{
	FX_INIT,
	FX_RUN,
	FX_END,
	FX_DONE
}t_fx_state;

//Types of Effects
typedef enum{
	PWM_FX,			//An effect that controls the PWM outputs
	STRIP_FX,		//An Effect that controls the LED Strips
	PWM_STRIP_FX	//An Effect that controls both
}t_fx_type;


//Type of Modes
typedef enum{
	MODE_CONTINOUS,  	//Effect runs until some other effect is selected
	MODE_SINGLE_SHOT,	//Effect Runs once for duration and returns to the last effect
	MODE_LOOP 			//Effect Runs for duration and then INIT State is called again once
}t_fx_mode;


//Function Prototype for Effect
//Parameters are
//- State
//- Current Frame
//- Duration
typedef t_fx_result (*fx_run_t)(t_fx_state,uint32_t,const uint32_t);

//Effect Parameters
typedef struct{
	t_fx_type type;
	t_fx_mode mode;
	uint8_t	  next_fx;	//Next effect if duration != 0. If set to 0 turns off effects
	char      fxname[9];
	uint32_t  duration;   //Duration in Frames. If 0 runs continuously otherwise advances to next effect after x frames
	fx_run_t fx_run_pointer;
}s_fx_param;



//Register an effect. Returns effect number
uint8_t register_fx(const s_fx_param* pfx_param, uint8_t fx_num);



#endif /* FX_LIST_H_ */
