/*
 * dmx512_config.h
 *
 *  Created on: Feb 10, 2020
 *      Author: Michael Hartmann
 */

#ifndef DMX512_CONFIG_H_
#define DMX512_CONFIG_H_

#include <stdint.h>

typedef enum{
	DMX_MODE1 = 0,
	DMX_MODE2 = 1
}dmxmode_t;



void dmx512_init(uint8_t mode,uint32_t addr);
void set_addr(uint32_t addr);
uint32_t get_addr(void);
void set_reg_length(uint32_t len);
uint32_t get_reg_length(void);
void init_regs(void);
void set_reg(uint32_t reg, uint8_t val);
uint8_t get_reg(uint32_t reg);
uint8_t* get_reg_addr(void);
dmxmode_t get_mode(void);
const char* get_reg_name(uint32_t reg);

//Mode 1 -> Slave, Only PWM
//Mode 2 -> Slave, PWM + LED Strips
//Mode 3 -> Slave, Only Strips ?
//Mode 4 -> Master ?

//Register map length
#define DMX512_MODE1_LENGTH	11
#define DMX512_MODE2_LENGTH (DMX512_MODE1_LENGTH + 15)
#define DMX512_MODE_MAX		26

//DMX512 Register Definition
#define MAX_BRIGHTNESS		0
#define CH1_RED				1
#define CH1_GREEN			2
#define CH1_BLUE			3
#define CH2_RED				4
#define CH2_GREEN			5
#define CH2_BLUE			6
#define CH3_RED				7
#define CH3_GREEN			8
#define CH3_BLUE			9
#define CH3_WHITE			10

#define FX_SELECT			11
#define	STRIP1_PATTERN		12
#define STRIP1_SPEED		13
#define STRIP1_SIZE			14
#define STRIP1_COMPLEXITY	15
#define STRIP1_V1			16
#define STRIP1_V2			17
#define STRIP1_V3			18
#define	STRIP2_PATTERN		19
#define STRIP2_SPEED		20
#define STRIP2_SIZE			21
#define STRIP2_COMPLEXITY	22
#define STRIP2_V1			23
#define STRIP2_V2			24
#define STRIP2_V3			25

#define REGISTER_NONE		255


#endif /* DMX512_CONFIG_H_ */
