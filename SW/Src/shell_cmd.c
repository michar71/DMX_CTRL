#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "shell.h"
#include "shell_cmd.h"
#include "dmx512_config.h"
#include "pwm_control.h"
#include "settings.h"
#include "triggers.h"
#include "gammactrl.h"


extern int UART_mode_SERIAL;
extern int UART_mode_USB;

static int shell_cmd_test(int argc, char ** argv);
static int shell_cmd_getaddr(int argc, char ** argv);
static int shell_cmd_getmode(int argc, char ** argv);
static int shell_cmd_dumpregs(int argc, char ** argv);
static int shell_cmd_dumpsettings(int argc, char ** argv);
static int shell_cmd_setpwm(int argc, char ** argv);
static int shell_cmd_setbrightness(int argc, char ** argv);
static int shell_cmd_savesettings(int argc, char ** argv);
static int shell_cmd_setgain(int argc, char ** argv);
static int shell_cmd_setoffset(int argc, char ** argv);
static int shell_cmd_setgamma(int argc, char ** argv);
static int shell_cmd_dumpadc(int argc, char ** argv);
static int shell_cmd_setreg(int argc, char ** argv);
static int shell_cmd_setfxmultiplier(int argc, char ** argv);
static int shell_cmd_storedefaultregs(int argc, char ** argv);
static int shell_cmd_settriggerconfig(int argc, char ** argv);
static int shell_cmd_settriggerctrl(int argc, char ** argv);
static int shell_cmd_setstriplength(int argc, char ** argv);
static int shell_cmd_setuartmode(int argc, char ** argv);
static int shell_cmd_switchuartmode(int argc, char ** argv);

/* to be consumed in shell.c only */
const shell_cmd_t shell_cmd_list[] = {
	{"test",   "test [msg]\n\r",shell_cmd_test},
	{"getaddr",   "getaddr\n\r",shell_cmd_getaddr},
	{"getmode",   "getmode\n\r",shell_cmd_getmode},
	{"dumpregs",   "dumpregs\n\r",shell_cmd_dumpregs},
	{"dumpsettings",   "dumpsettings\n\r",shell_cmd_dumpsettings},
	{"setpwm",   "setpwm [id (0..2)] [ch (0..3)] [duty (0..65535)]\n\r",shell_cmd_setpwm},
	{"setbrightness",   "setbrightness [0..255]\n\r",shell_cmd_setbrightness},
	{"savesettings",   "savesettings\n\r",shell_cmd_savesettings},
	{"setgain",   "setgain [ch (0..2)] [gain (-32766...32766)]\n\r",shell_cmd_setgain},
	{"setoffset",   "setoffset [ch (0..2)] [offset (-32766...32766)]\n\r",shell_cmd_setoffset},
	{"setgamma",   "setgamma [ch (0=R, 1=G, 2=B,3=STRIP)] [gamma (-32766...32766)]\n\r",shell_cmd_setgamma},
	{"setreg",   "setreg [reg] [val]\n\r",shell_cmd_setreg},
	{"setfxmultiplier",   "setfxmultiplier  [val (1,2,4)]\n\r",shell_cmd_setfxmultiplier},
	{"settriggerconfig",   "settriggerconfig  [trigger 0|1] [mode(0=None, 1=mapping, 2=switch)] [register (0..25] [level (0..255)]\n\r",shell_cmd_settriggerconfig},
	{"settriggerctrl",   "settriggerctrl  [trigger 0|1] [Reg HiLo (0..25)] [Val HiLo (0..255)] [Reg LoHi (0..25)] [Val LoHi (0..255)]\n\r",shell_cmd_settriggerctrl},
	{"storedefaultregs",   "storedefaultregs\n\r",shell_cmd_storedefaultregs},
	{"dumpadc",   "dumpadc\n\r",shell_cmd_dumpadc},
	{"setstriplength",   "setstriplength [ch (0|1)] [length (0..1024)]\n\r",shell_cmd_setstriplength},
	{"setuartmode",   "setuartmode [0 = Serial, 1 = USB] [0 = Shell, 1 = Serial DMX]\n\r",shell_cmd_setuartmode},
	{"switchuartmode",   "switchuartmode [0 = Serial, 1 = USB]\n\r",shell_cmd_switchuartmode},
};

const int SHELL_CMD_NUM = sizeof(shell_cmd_list)/sizeof(shell_cmd_t);


/* functions */

//Shell Test
static int shell_cmd_test(int argc, char ** argv)
{
	if (argc == 1)
	{
		print("Loopback Data: %s",argv[1]);
		return 1;
	}
	return 0;
}


//getaddr
static int shell_cmd_getaddr(int argc, char ** argv)
{
		print("DMX512 Address: %d",get_addr());
		return 1;
}


//getmode
static int shell_cmd_getmode(int argc, char ** argv)
{
	switch (get_mode())
	{
	case DMX_MODE1:
		print("Mode 1 (PWM)");
		break;
	case DMX_MODE2:
		print("Mode 2 (PWM + Strip)");
		break;
	}

	return 1;
}

//dumpregs
static int shell_cmd_dumpregs(int argc, char ** argv)
{
		print("DMX512 Address: %d",get_addr());
		print("DMX512 Blocklength: %d",get_reg_length());
		print("Mode: %d",(uint8_t)get_mode());
		print("---------------------------");
		for(int ii=0;ii< get_reg_length();ii++)
		{
			print("Reg: %d\t Addr:%d  \t (%s)\t Val: %d",ii,get_addr()+ii,get_reg_name(ii),get_reg(ii));
		}
		return 1;
}

//dumpsettings
static int shell_cmd_dumpsettings(int argc, char ** argv)
{
		print_settings();
		return 1;
}

//setpwm
static int shell_cmd_setpwm(int argc, char ** argv)
{
	if (argc == 3)
	{
		configPWM((pwmtimerid_t)atoi(argv[1]),(pwmchid_t)atoi(argv[2]), (uint16_t)atoi(argv[3]));
	}
	else
	{
		return 0;
	}
	return 1;
}

//setbrightness
static int shell_cmd_setbrightness(int argc, char ** argv)
{
	if (argc == 1)
	{
		set_reg(MAX_BRIGHTNESS,(uint8_t) atoi(argv[1]));
	}
	else
	{
		return 0;
	}
	return 1;
}

//savesettings
static int shell_cmd_savesettings(int argc, char ** argv)
{
		//TBD Yes/No Confirmation Check
		save_settings();
		return 1;
}


//setgain
static int shell_cmd_setgain(int argc, char ** argv)
{
	if (argc == 2)
	{
		switch (atoi(argv[1]))
		{
		case 0:
			settings.gain_red = atoi(argv[2]);
			break;
		default:
		case 1:
			settings.gain_green = atoi(argv[2]);
			break;
		case 2:
			settings.gain_blue = atoi(argv[2]);
			break;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}


//setoffset
static int shell_cmd_setoffset(int argc, char ** argv)
{
	if (argc == 2)
	{
		switch (atoi(argv[1]))
		{
		case 0:
			settings.offset_red = atoi(argv[2]);
			break;
		default:
		case 1:
			settings.offset_green = atoi(argv[2]);
			break;
		case 2:
			settings.offset_blue = atoi(argv[2]);
			break;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}

//setgamma
static int shell_cmd_setgamma(int argc, char ** argv)
{
	if (argc == 2)
	{
		switch (atoi(argv[1]))
		{
		case 0:
			settings.gamma_red = atoi(argv[2]);
			break;
		default:
		case 1:
			settings.gamma_green = atoi(argv[2]);
			break;
		case 2:
			settings.gamma_blue = atoi(argv[2]);
			break;
		case 3:
			settings.gamma_strip = atoi(argv[2]);
			break;
		}
		if (false == recalcGamma())
			print("Gamma Table Creation Failed");
	}
	else
	{
		return 0;
	}
	return 1;
}

//setfxmultiplier
static int shell_cmd_setfxmultiplier(int argc, char ** argv)
{
	uint8_t val = atoi(argv[1]);
	if ((val == 4) || (val == 2) || (val == 1))
		settings.fx_multiplier = val;
	else
	{
		print("Invalid Multiplier");
		settings.fx_multiplier = 4;
	}
	return 1;
}


//dumpconfig
static int shell_cmd_dumpadc(int argc, char ** argv)
{
	print_adc_data();
	return 1;
}

//setreg
static int shell_cmd_setreg(int argc, char ** argv)
{
	if (argc == 2)
	{
		set_reg(atoi(argv[1]),atoi(argv[2]));
	}
	else
	{
		return 0;
	}
	return 1;
}


static int shell_cmd_storedefaultregs(int argc, char ** argv)
{
	//Store the current register values to default settings
	settings.max_brightness = get_reg(MAX_BRIGHTNESS);
	settings.pwm_ch1_r = get_reg(CH1_RED);
	settings.pwm_ch1_g = get_reg(CH1_GREEN);
	settings.pwm_ch1_b = get_reg(CH1_BLUE);
	settings.pwm_ch2_r = get_reg(CH2_RED);
	settings.pwm_ch2_g = get_reg(CH2_GREEN);
	settings.pwm_ch2_b = get_reg(CH2_BLUE);
	settings.pwm_ch3_r = get_reg(CH3_RED);
	settings.pwm_ch3_g = get_reg(CH3_GREEN);
	settings.pwm_ch3_b = get_reg(CH3_BLUE);
	settings.pwm_ch3_w = get_reg(CH3_WHITE);

	//Deal with LED Strip Registers
	if (DMX_MODE2 == get_mode())
	{
		settings.fx_select = get_reg(FX_SELECT);
		settings.strip1_pattern = get_reg(STRIP1_PATTERN);
		settings.strip1_speed = get_reg(STRIP1_SPEED);
		settings.strip1_size = get_reg(STRIP1_SIZE);
		settings.strip1_complexity = get_reg(STRIP1_COMPLEXITY);
		settings.strip1_v1 = get_reg(STRIP1_V1);
		settings.strip1_v2 = get_reg(STRIP1_V2);
		settings.strip1_v3 = get_reg(STRIP1_V3);
		settings.strip2_pattern = get_reg(STRIP2_PATTERN);
		settings.strip2_speed = get_reg(STRIP2_SPEED);
		settings.strip2_size = get_reg(STRIP2_SIZE);
		settings.strip2_complexity = get_reg(STRIP2_COMPLEXITY);
		settings.strip2_v1 = get_reg(STRIP2_V1);
		settings.strip2_v2 = get_reg(STRIP2_V2);
		settings.strip2_v3 = get_reg(STRIP2_V3);
	}
	return 1;
}

//settriggerconfig
static int shell_cmd_settriggerconfig(int argc, char ** argv)
{
	if (argc == 4)
	{
		switch (atoi(argv[1]))
		{
		case 0:
			settings.trigger_mode_A0 = atoi(argv[2]);
			settings.trigger_reg_A0 = atoi(argv[3]);
			settings.trigger_level_A0 = atoi(argv[4]);
			break;
		case 1:
			settings.trigger_mode_A1 = atoi(argv[2]);
			settings.trigger_reg_A1 = atoi(argv[3]);
			settings.trigger_level_A1 = atoi(argv[4]);
			break;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}



//settriggerctrl
static int shell_cmd_settriggerctrl(int argc, char ** argv)
{
	if (argc == 5)
	{
		switch (atoi(argv[1]))
		{
		case 0:
			settings.trigger_reg_hilo_A0 = atoi(argv[2]);
			settings.trigger_val_hilo_A0 = atoi(argv[3]);
			settings.trigger_reg_lohi_A0 = atoi(argv[4]);
			settings.trigger_val_lohi_A0 = atoi(argv[5]);
			break;
		case 1:
			settings.trigger_reg_hilo_A1 = atoi(argv[2]);
			settings.trigger_val_hilo_A1 = atoi(argv[3]);
			settings.trigger_reg_lohi_A1 = atoi(argv[4]);
			settings.trigger_val_lohi_A1 = atoi(argv[5]);
			break;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}

//setstriplength
static int shell_cmd_setstriplength(int argc, char ** argv)
{
	if (argc == 2)
	{
		switch (atoi(argv[1]))
		{
		case 0:
			settings.strip1_length = atoi(argv[2]);
			break;
		case 1:
			settings.strip2_length = atoi(argv[2]);
			break;
		default:
			break;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}

//Set UART Mode
static int shell_cmd_setuartmode(int argc, char ** argv)
{
	if (argc == 2)
	{
		switch (atoi(argv[1]))
		{
		case 0:
			settings.UART_Mode_UART = atoi(argv[2]);
			break;
		case 1:
			settings.UART_Mode_USB = atoi(argv[2]);
			break;
		default:
			break;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}


//Switch UART Mode
static int shell_cmd_switchuartmode(int argc, char ** argv)
{
	if (argc == 2)
	{
		switch (atoi(argv[1]))
		{
		case 0:
			UART_mode_SERIAL = (uint8_t)UART_MODE_DMX;
			break;
		case 1:
			UART_mode_USB = (uint8_t)UART_MODE_DMX;
			break;
		default:
			break;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}

