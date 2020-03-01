/*
 * pwm_control.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Michael Hartmann
 */

#ifndef PWM_CONTROL_H_
#define PWM_CONTROL_H_

#include <stdint.h>

#define MAX_PWM 1076

typedef enum{
	PWM_CH1 = 0,
	PWM_CH2,
	PWM_CH3
}pwmtimerid_t;


typedef enum{
	CH_RED,
	CH_GREEN,
	CH_BLUE,
	CH_WHITE
}pwmchid_t;

void init_timers(void);
void PWM_Timer_Init(pwmtimerid_t ID);
void configPWM(pwmtimerid_t ID,pwmchid_t channel, uint16_t duty);
void configRGB(uint8_t ch,uint8_t r, uint8_t g, uint8_t b);

#endif /* PWM_CONTROL_H_ */
