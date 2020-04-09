/*
 * settings.c
 *
 *  Created on: Feb 14, 2020
 *      Author: Michael Hartmann
 */
#include "settings.h"
#include "eeprom.h"
#include "dmx512_config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include "shell.h"


#define PAGE_SETTING_START 123
#define PAGE_SETTING_COUNT 4


void init_settings(void)
{
	//Set everything to 0....
	memset(&settings,0,sizeof(settings_s));

	//Set deviations from 0...
	settings.max_brightness = 255;
	settings.strip1_speed = 10;
	settings.strip1_size = 10;
	settings.strip1_complexity = 10;
	settings.strip2_speed = 10;
	settings.strip2_size = 10;
	settings.strip2_complexity = 10;


	settings.strip1_length = 300;
	settings.strip2_length = 300;

	settings.offset_red = -100;
	settings.gain_red = 100;    //Gain * 100
	settings.gamma_red = 220;   //Gamma * 100
	settings.offset_green = -80;
	settings.gain_green =100;    //Gain * 100
	settings.gamma_green = 220;   //Gamma * 100
	settings.offset_blue = -80;
	settings.gain_blue = 100;    //Gain * 100
	settings.gamma_blue = 220;   //Gamma * 100
}

void apply_settings(void)
{
	//Apply settings to registers
	set_reg(MAX_BRIGHTNESS,settings.max_brightness);
	set_reg(CH1_RED,settings.pwm_ch1_r);
	set_reg(CH1_GREEN,settings.pwm_ch1_g);
	set_reg(CH1_BLUE,settings.pwm_ch1_b);
	set_reg(CH2_RED,settings.pwm_ch2_r);
	set_reg(CH2_GREEN,settings.pwm_ch2_g);
	set_reg(CH2_BLUE,settings.pwm_ch2_b);
	set_reg(CH3_RED,settings.pwm_ch3_r);
	set_reg(CH3_GREEN,settings.pwm_ch3_g);
	set_reg(CH3_BLUE,settings.pwm_ch3_b);
	set_reg(CH3_WHITE,settings.pwm_ch3_w);

	//Deal with LED Strip Registers
	if (DMX_MODE2 == get_mode())
	{
		set_reg(FX_SELECT,settings.fx_select);

		set_reg(STRIP1_PATTERN,settings.strip1_pattern);
		set_reg(STRIP1_SPEED,settings.strip1_speed);
		set_reg(STRIP1_SIZE,settings.strip1_size);
		set_reg(STRIP1_COMPLEXITY,settings.strip1_complexity);
		set_reg(STRIP1_V1,settings.strip1_v1);
		set_reg(STRIP1_V2,settings.strip1_v2);
		set_reg(STRIP1_V3,settings.strip1_v3);

		set_reg(STRIP2_PATTERN,settings.strip2_pattern);
		set_reg(STRIP2_SPEED,settings.strip2_speed);
		set_reg(STRIP2_SIZE,settings.strip2_size);
		set_reg(STRIP2_COMPLEXITY,settings.strip2_complexity);
		set_reg(STRIP2_V1,settings.strip2_v1);
		set_reg(STRIP2_V2,settings.strip2_v2);
		set_reg(STRIP2_V3,settings.strip2_v3);
	}
}

void load_settings(void)
{
	EE_Reads(0,sizeof(settings_s),(uint32_t*)&settings);
}

void save_settings(void)
{
	EE_Writes(0,256,(uint32_t*)&settings);
}

void print_settings(void)
{
	print("Settings");
	print("-------");
	//Strips
	print("Strip 1 Length: %d",settings.strip1_length);
	print("Strip 2 Length: %d",settings.strip2_length);

	//Triggers
	print("Triggers");
	print("--------");
	print("Trigger A0 Mode",settings.trigger_mode_A0);
	print("Trigger A0 Reg: %d",settings.trigger_reg_A0);
	print("Trigger A0 Reg Low->Hi: %d",settings.trigger_reg_lohi_A0);
	print("Trigger A0 Val Low->Hi: %d",settings.trigger_val_lohi_A0);
	print("Trigger A0 Reg Hi->Low: %d",settings.trigger_reg_hilo_A0);
	print("Trigger A0 Val Hi-Low: %d",settings.trigger_val_hilo_A0);
	print("Trigger A0 Level: %d",settings.trigger_level_A0);

	print("Trigger A1 Mode",settings.trigger_mode_A1);
	print("Trigger A1 Reg: %d",settings.trigger_reg_A1);
	print("Trigger A1 Reg Low->Hi: %d",settings.trigger_reg_lohi_A1);
	print("Trigger A1 Val Low->Hi: %d",settings.trigger_val_lohi_A1);
	print("Trigger A1 Reg Hi->Low: %d",settings.trigger_reg_hilo_A1);
	print("Trigger A1 Val Hi-Low: %d",settings.trigger_val_hilo_A1);
	print("Trigger A1 Level: %d",settings.trigger_level_A1);

    //Calibration
	print("Calibration");
	print("-----------");
	print("Offset Red: %d",settings.offset_red);
	print("Gain Red: %d",settings.gain_red);
	print("Gamma Red: %d",settings.gamma_red);
	print("Offset Green: %d",settings.offset_green);
	print("Gain Green: %d",settings.gain_green);
	print("Gamma Green: %d",settings.gamma_green);
	print("Offset Blue: %d",settings.offset_blue);
	print("Gain Blue: %d",settings.gain_blue);
	print("Gamma Blue: %d",settings.gamma_blue);
}
