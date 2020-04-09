/*
 * fx_list.c
 *
 *  Created on: Apr 7, 2020
 *      Author: Michael
 */
#include "fx_list.h"

s_fx_param fx_list[MAX_FX];
uint8_t fxcnt = 0;

uint8_t register_fx(s_fx_param fx_param)
{
	fxcnt++;
	fx_list[fxcnt] = fx_param;
	return fxcnt;
}
