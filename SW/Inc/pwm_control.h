/*
 * pwm_control.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Michael Hartmann
 */

#ifndef PWM_CONTROL_H_
#define PWM_CONTROL_H_

#include <stdint.h>




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
void PWM_Timer_Init(pwmtimerid_t ID,uint32_t prescaler,uint32_t pwm_max, uint32_t clockdiv, uint32_t reinit);
void configPWM(pwmtimerid_t ID,pwmchid_t channel, uint16_t duty);
void setPWMdirect(uint8_t id, uint8_t ch, uint16_t val);
uint32_t getMaxPWM(void);

#endif /* PWM_CONTROL_H_ */
