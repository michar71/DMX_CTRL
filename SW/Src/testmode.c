/*
 * testmode.c
 *
 *  Created on: Mar 6, 2020
 *      Author: Michael Hartmann
 */

#include "testmode.h"
#include "gpio_control.h"
#include "pwm_control.h"

#define STEP_CNT 600
#define STATE_CNT 10

typedef struct{
	pwmtimerid_t id;
	pwmchid_t ch;
}state_s;


uint8_t state = 0;
state_s states[STATE_CNT] = {{PWM_CH1,CH_RED},{PWM_CH1,CH_GREEN},{PWM_CH1,CH_BLUE},
		                     {PWM_CH2,CH_RED},{PWM_CH2,CH_GREEN},{PWM_CH2,CH_BLUE},
							 {PWM_CH3,CH_RED},{PWM_CH3,CH_GREEN},{PWM_CH3,CH_BLUE},{PWM_CH3,CH_WHITE}};



void lights_off(void)
{
	for (uint8_t ii=0;ii<STATE_CNT;ii++)
		configPWM(states[ii].id,states[ii].ch, 0);
}

uint8_t process_testmode(void)
{
	static uint8_t last_button_state = 1;
	static uint16_t cnt = 0;
	static uint8_t done = 0;
	uint8_t button_state = 1;

	if (done)
	{
		if (cnt == 0)
		{
			done = 0;
			return 0;
		}
		cnt--;
	}
	else
	{
		if (cnt == 0)
		{
			cnt = STEP_CNT;
			//Set PWM's
			lights_off();
			configPWM(states[state].id,states[state].ch, getMaxPWM());
			state++;
			if (state == STATE_CNT)
				state = 0;
		}
		cnt--;

		//Check if user pushed and released button to end test mode
		button_state = check_button();
		if ((last_button_state == 0) && (button_state == 1))
		{
			last_button_state = 1;
			lights_off();
			cnt = STEP_CNT;
			state = 0;
			done = 1;
		}
		last_button_state = button_state;
	}
	return 1;
}


