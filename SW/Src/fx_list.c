/*
 * fx_list.c
 *
 *  Created on: Apr 7, 2020
 *      Author: Michael
 */
#include "fx_list.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "shell.h"

const s_fx_param* pfx_list[MAX_FX];
static const s_fx_param param = {PWM_FX,MODE_CONTINOUS,0,"NULL",0,NULL};

//Point all pointers to the same "empty" FX
void init_fx_list(void)
{
	uint8_t cnt = 0;
	for(cnt = 0;cnt < MAX_FX;cnt++)
		pfx_list[cnt] = &param;
}

uint8_t register_fx(const s_fx_param* pfx_param, uint8_t fx_num)
{
	uint8_t fx_cnt = 1;
	//If FX = 0 we find the first empty slot and install it there
	if (fx_num == 0)
	{
		while ((pfx_list[fx_cnt]->fx_run_pointer != NULL) && (fx_cnt < MAX_FX-1))
		{
			fx_cnt++;
		}

		//No slots available
		if (fx_cnt == MAX_FX-1)
		{
			print("FX Space full");
			return 0;
		}
		fx_num = fx_cnt;
	}

	pfx_list[fx_num] = pfx_param;
	print("ID:%d\t FX: %s",fx_num,pfx_param->fxname);
	return fx_num;
}
