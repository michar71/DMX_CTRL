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

uint8_t register_fx(const s_fx_param* pfx_param, uint8_t fx_num)
{
	uint8_t fx_cnt = 1;
	//If FX = 0 we find the first empty slot and install it there
	if (fx_num == 0)
	{
		while ((pfx_list[fx_cnt] != NULL) && (fx_cnt < MAX_FX-1))
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
