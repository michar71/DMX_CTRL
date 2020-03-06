#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "shell.h"
#include "shell_cmd.h"
#include "dmx512_config.h"
#include "pwm_control.h"
#include "settings.h"

static int shell_cmd_test(int argc, char ** argv);
static int shell_cmd_getaddr(int argc, char ** argv);
static int shell_cmd_getmode(int argc, char ** argv);
static int shell_cmd_dumpregs(int argc, char ** argv);
static int shell_cmd_dumpconfig(int argc, char ** argv);
static int shell_cmd_setpwm(int argc, char ** argv);
static int shell_cmd_setbrightness(int argc, char ** argv);
static int shell_cmd_savesettings(int argc, char ** argv);
static int shell_cmd_setgain(int argc, char ** argv);
static int shell_cmd_setoffset(int argc, char ** argv);
static int shell_cmd_setgamma(int argc, char ** argv);

/* to be consumed in shell.c only */
const shell_cmd_t shell_cmd_list[] = {
	{"test",   "test [msg]\n\r",shell_cmd_test},
	{"getaddr",   "getaddr\n\r",shell_cmd_getaddr},
	{"getmode",   "getmode\n\r",shell_cmd_getmode},
	{"dumpregs",   "dumpregs\n\r",shell_cmd_dumpregs},
	{"dumpconfig",   "dumpconfig\n\r",shell_cmd_dumpconfig},
	{"setpwm",   "setpwm [id (0..2)] [ch (0..3)] [duty (0..65535)]\n\r",shell_cmd_setpwm},
	{"setbrightness",   "setbrightness [0..255]\n\r",shell_cmd_setbrightness},
	{"savesettings",   "savesettings\n\r",shell_cmd_savesettings},
	{"setgain",   "setgain [ch (0..2)] [gain (-32766...32766)]\n\r",shell_cmd_setgain},
	{"setoffset",   "setoffset [ch (0..2)] [offset (-32766...32766)]\n\r",shell_cmd_setoffset},
	{"setgamma",   "setgamma [ch (0..2)] [gamma (-32766...32766)]\n\r",shell_cmd_setgamma},
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
	case DMX_MODE_NONE:
		print("Mode None (Uninitialized)");
		break;
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

//dumpconfig
static int shell_cmd_dumpconfig(int argc, char ** argv)
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
		}
	}
	else
	{
		return 0;
	}
	return 1;
}

//getaddr
//getmode
//setaddr
//setmode
//dumpregs
//dumpconfig
//saveconfig
//menuconfig
//setgain [ch] [val]
//setoffset [ch] [val]
//setstriplen [id] [length]
//setstripfx [id] [mode]
//setstripspeed [id] [speed]
//setstripsize [id] [size]
//setstripcompl [id] [compl]
//setstripv1 [id] [v1]
//setstripv2 [id] [v2]
//setstripv3 [id] [v3]
//setstrip on|off
//setbrightness [max]
//settestmode on|off
//setpwmfreq [val]
//setpwm [id] [ch] [duty]
//setpwmrgb [id] [r] [g] [b]
//startmode (off|default|reset)

/*

static int shell_cmd_test(int argc, char ** argv)
{
	  if(0 == strncmp(argv[1], "status", SHELL_CMD_BUF_LEN))
    {
     //   aux_power_print_info();
    }
    else if(0 == strncmp(argv[1], "set", SHELL_CMD_BUF_LEN))
    {
	//			aux_power_enable((aux_power_conn_t)atoi(argv[2]),(bool)atoi(argv[3]));
		}
    else if(0 == strncmp(argv[1], "gpo", SHELL_CMD_BUF_LEN))
		{
	//	    aux_gpo_enable((bool)atoi(argv[2]));
	  }
		else
		{
			return 0;
		}
    return 1;
}

static int shell_i2c_test(int argc, char ** argv)
{
	if(0 == strncmp(argv[1], "read", SHELL_CMD_BUF_LEN))
    {
      if (argc == 3)
      {
        uint8_t addr = atoi(argv[2]);
        uint8_t reg = atoi(argv[3]);
        uint8_t reg_val = 0;

        I2C_MasterSendStart(addr, I2C_WRITE_XFER_MODE);
        I2C_MasterWriteByte(reg);
       // I2C_MasterSendStop();
        I2C_MasterSendRestart(addr, I2C_READ_XFER_MODE);
        reg_val = I2C_MasterReadByte(I2C_NAK_DATA);
        I2C_MasterSendStop();
        print("%d read from %d->%d\n",reg_val,addr,reg);
      }
	  else
	  {
		 return 0;
	  }
    }
    else if(0 == strncmp(argv[1], "write", SHELL_CMD_BUF_LEN))
    {
      if (argc == 4)
      {
        uint8_t addr = atoi(argv[2]);
        uint8_t reg = atoi(argv[3]);
        uint8_t data = atoi(argv[4]);
        I2C_MasterSendStart(addr, I2C_WRITE_XFER_MODE);
        I2C_MasterWriteByte(reg);
        I2C_MasterWriteByte(data);
        I2C_MasterSendStop();
        print("%d written to %d->%d\n",data,addr,reg);
      }
	  else
	  {
		 return 0;
	  }
	}
	else
	{
		return 0;
	}
    return 1;
}

static int shell_cmd_freq(int argc, char ** argv)
{
    if (argc == 1)
    {
	    uint32_t freq = atoi(argv[1]);
	    uint32_t res;
	    radio_set_freq((double)freq, &res);
	    return 1;
    }
    else
    {
        return 0;
    }
}

static int shell_cmd_mode(int argc, char ** argv)
{
	if(0 == strncmp(argv[1], "lsb", SHELL_CMD_BUF_LEN))
    {
     	radio_set_mode(RADIO_MODE_LSB);
    }
    else if(0 == strncmp(argv[1], "usb", SHELL_CMD_BUF_LEN))
    {
     	radio_set_mode(RADIO_MODE_USB);
		}
    else if(0 == strncmp(argv[1], "am", SHELL_CMD_BUF_LEN))
		{
     	radio_set_mode(RADIO_MODE_AM);
	  }
	else
	{
		return 0;
	}
    return 1;
}

*/
