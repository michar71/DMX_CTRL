/*
 * settings.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Michael Hartmann
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_


typedef struct{
	//----------------
	//Defaults
	//----------------

	//Leds
	uint16_t max_brightness;
	uint16_t pwm_ch1_r;
	uint16_t pwm_ch1_g;
	uint16_t pwm_ch1_b;
	uint16_t pwm_ch2_r;
	uint16_t pwm_ch2_g;
	uint16_t pwm_ch2_b;
	uint16_t pwm_ch3_r;
	uint16_t pwm_ch3_g;
	uint16_t pwm_ch3_b;
	uint16_t pwm_ch3_w;

	//Defaults
	uint8_t strip_bits;
	uint8_t strip1_pattern;
	uint8_t strip1_speed;
	uint8_t strip1_size;
	uint8_t strip1_complexity;
	uint8_t strip1_v1;
	uint8_t strip1_v2;
	uint8_t strip1_v3;

	uint8_t strip2_pattern;
	uint8_t strip2_speed;
	uint8_t strip2_size;
	uint8_t strip2_complexity;
	uint8_t strip2_v1;
	uint8_t strip2_v2;
	uint8_t strip2_v3;

	//----------------
	//Settings
	//----------------

	//Strips
	uint16_t strip1_length;
	uint16_t strip2_length;

	//Features
	//- Start UART in Shell Mode
	//- Dump Debug Info to UART
	//- Support Virtual UART on USB

	//Triggers
	//- A1 Analog or Digital
	//- A2 Analog or Digital
	//- A1 Linked to Param X In Analog Mode
	//- A2 Linked to Param X in Analog Mode
	//- A1 in Digital Mode from Low to High sets Parameter X to Value Y
	//- A1 in Digital Mode from High to Low sets Parameter X to Value Y
	//- A2 in Digital Mode from Low to High sets Parameter X to Value Y
	//- A2 in Digital Mode from High to Low sets Parameter X to Value Y

	//Master Mode
	//- TBD

}settings_s;


#endif /* SETTINGS_H_ */
