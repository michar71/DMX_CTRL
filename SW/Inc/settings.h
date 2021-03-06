/*
 * settings.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Michael Hartmann
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>
#include <stdbool.h>


typedef enum{
	TRIGGERMODE_NONE,
	TRIGGERMODE_MAPPING,
	TRIGGERMODE_SWITCH,

}triggermode_e;

typedef enum{
	UART_MODE_SHELL = 0,
	UART_MODE_DMX
}uartmode_t;


typedef struct{
	//----------------
	//Defaults
	//----------------
	//Preamble
	uint8_t configID[4];   //Config ID has to be DSOS
	uint16_t configFWversion;

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
	uint8_t fx_select;
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
	uint32_t frame_ms_target;  //Target frame length in ms. We'll wait at leaast this long for a frame. Set to 0 for max frametate

	uint8_t fx_multiplier;  //Multiplier applied to fx_select register
							//Some controllers make it really hard to select
							//a Specific value with a slider so this will
							//multiply the FX_Select register spreading it out
	                        //so that one FX occupies multiple codes (0,1,2,3|4,5,6,7...)
	//Strips
	uint16_t strip1_length;
	uint16_t strip2_length;

	//UARTS
	uint8_t UART_Mode_UART;
	uint8_t UART_Mode_USB;

	//Features
	//- Dump Debug Info to UART
	//- Support Virtual UART on USB

	//Triggers
	uint8_t trigger_mode_A0;  		//Trigger Mode
	uint8_t trigger_reg_A0;   		//Value linked to register
	uint8_t trigger_reg_lohi_A0;	//Trigger Low->Hi Register
	uint8_t trigger_val_lohi_A0;	//Trigger Low->Hi Value
	uint8_t trigger_reg_hilo_A0;    //Trigger Hi-Low Register
	uint8_t trigger_val_hilo_A0;    //Trigger Hi->Low Value
	uint8_t trigger_level_A0;		//Trigger Level for Lo/Hi Transition (0..255)

	uint8_t trigger_mode_A1;  		//Trigger Mode
	uint8_t trigger_reg_A1;   		//Value linked to register
	uint8_t trigger_reg_lohi_A1;	//Trigger Low->Hi Register
	uint8_t trigger_val_lohi_A1;	//Trigger Low->Hi Value
	uint8_t trigger_reg_hilo_A1;    //Trigger Hi-Low Register
	uint8_t trigger_val_hilo_A1;    //Trigger Hi->Low Value
	uint8_t trigger_level_A1;		//Trigger Level for Lo/Hi Transition (0..255)

    //Calibration
    //- Color Calib mode (LUT, Params)
    //- LUT 8bit to 16bit RED
    //- LUT 8bit to 16bit GREEN
    //- LUT 8bit to 16bit BLUE
    //- Offset/Gain/Gamma for all PWM Color Channels
	//These values are applied before the PWM is applied so they refer to the
	//maximum range of the PwM settings (Currently 0..MAX_PWM = 1076...
	int16_t offset_red;  //Absolute Offset
	int16_t gain_red;    //Gain * 100
	int16_t gamma_red;   //Gamma * 100
	int16_t offset_green;  //Absolute Offset
	int16_t gain_green;    //Gain * 100
	int16_t gamma_green;   //Gamma * 100
	int16_t offset_blue;  //Absolute Offset
	int16_t gain_blue;    //Gain * 100
	int16_t gamma_blue;   //Gamma * 100
	int16_t gamma_strip;   //Gamma * 100


	//Master Mode
	//- TBD

}settings_s;

settings_s settings;

void init_settings(void);
void apply_settings(void);
bool load_settings(void);
void save_settings(void);
void print_settings(void);

#endif /* SETTINGS_H_ */
