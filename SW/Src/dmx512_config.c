/*
 * dmx512_config.c
 *
 *  Created on: Feb 10, 2020
 *      Author: Michael Hartmann
 */


#include "dmx512_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* regnames[] ={
	"MAX_BRIGHTN",
	"CH1_RED",
	"CH1_GREEN",
	"CH1_BLUE",
	"CH2_RED",
	"CH2_GREEN",
	"CH2_BLUE",
	"CH3_RED",
	"CH3_GREEN",
	"CH3_BLUE",
	"CH3_WHITE",
	"FX_SELECT",
	"STRIP1_PATTERN",
	"STRIP1_SPEED",
	"STRIP1_SIZE",
	"STRIP1_COMPLEX",
	"STRIP1_V1",
	"STRIP1_V2",
	"STRIP1_V3",
	"STRIP2_PATTERN",
	"STRIP2_SPEED",
	"STRIP2_SIZE",
	"STRIP2_COMPLEX",
	"STRIP2_V1",
	"STRIP2_V2",
	"STRIP2_V3"
};


static uint8_t* reg_data;
static uint32_t reg_addr = 0;
static uint32_t reg_len = 0;
static uint8_t dmx_mode = DMX_MODE1;

void dmx512_init(uint8_t mode,uint32_t addr)
{
	set_addr(addr);
	if ((mode & 0x01) != 0)
		dmx_mode = DMX_MODE2;
	switch (dmx_mode)
	{
		default:
		case DMX_MODE1:
			 set_reg_length(DMX512_MODE1_LENGTH);
			break;
		case DMX_MODE2:
			set_reg_length(DMX512_MODE2_LENGTH);
			break;
	}
	init_regs();
}

void set_addr(uint32_t addr)
{
	reg_addr = addr;
}

uint32_t get_addr(void)
{
	return reg_addr;
}

dmxmode_t get_mode(void)
{
	return dmx_mode;
}

void set_reg_length(uint32_t len)
{
	reg_len = len;
}

uint32_t get_reg_length(void)
{
	return reg_len;
}

void init_regs(void)
{
	//allocate memory
	if (reg_data != NULL)
		free(reg_data);

	reg_data = malloc(reg_len * sizeof(uint8_t));
	memset(reg_data, 0,reg_len);
}

void set_reg(uint32_t reg, uint8_t val)
{
	if ((reg_data != NULL) && (reg<reg_len))
	{
		reg_data[reg] = val;
	}
}

uint8_t get_reg(uint32_t reg)
{
	if ((reg_data != NULL) && (reg<reg_len))
	{
		return reg_data[reg];
	}
	else
	{
		return 0;
	}
}

uint8_t* get_reg_addr(void)
{
	return reg_data;
}


const char* get_reg_name(uint32_t reg)
{
	if (reg<reg_len)
		return regnames[reg];
	else
		return 0;
}
