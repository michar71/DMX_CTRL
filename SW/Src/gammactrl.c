/*
 * gammactrl.c
 *
 *  Created on: Jun 7, 2020
 *      Author: Micha
 */
#include "gammactrl.h"
#include "eeprom.h"
#include "settings.h"
#include <math.h>
#include "stm32f1xx_hal.h"

#define GAMMA_TABLE_PAGE 126


uint8_t calc_gamma_val(int16_t gamma, uint8_t val)
{
	float gf = ((float)gamma/100.0);
	float res;

	res = 255.0 * pow(((float)val/255.0),(1.0/gf));
	res = round(res);
	if (res > 255.0)
		res = 255.0;
	else if (res<0)
		res = 0;
	return (uint8_t)res;
}

bool recalcGamma(void)
{
	uint8_t ii;
	uint8_t jj;
	int16_t gamma;
	int16_t index;
	uint64_t pdata;
	uint32_t waddr;

	//All 4 tables are recreated every time a gamma value changes
	//The tables are stored in Flash at page 126

    //First format page
    if(EE_LL_Format(GAMMA_TABLE_PAGE,1)==false)
        return false;

    //Store data
    HAL_FLASH_Unlock();

    //Loop through channels
    for (ii=0;ii<4;ii++)
    {
    	//Loop through tables
    	switch(ii)
    	{
    	case 0:
    		gamma = settings.gamma_red;
    		break;
    	case 1:
    		gamma = settings.gamma_green;
    		break;
    	case 2:
    		gamma = settings.gamma_blue;
    		break;
    	case 3:
    		gamma = settings.gamma_strip;
    		break;
    	}

    	index = 0;

    	for (jj = 0; jj<64;jj++)  //We'll need to loop through with words....
    	{
    	    pdata = ((uint64_t)calc_gamma_val(gamma, index)   << 24)
    	          + ((uint64_t)calc_gamma_val(gamma, index+1) << 16)
				  + ((uint64_t)calc_gamma_val(gamma, index+2) << 8)
				  + ((uint64_t)calc_gamma_val(gamma, index+3));
    	    index = index + 4;
    	    waddr = (jj*4)+(ii*256)+(ADDR_FLASH_PAGE_0|(_EEPROM_FLASH_PAGE_SIZE*GAMMA_TABLE_PAGE));
    	    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,waddr,pdata)!=HAL_OK)
    	    {
    	      HAL_FLASH_Lock();
    	      return false;
    	    }
    	}
    }
    HAL_FLASH_Lock();
    return true;
}

uint8_t getGamma(t_gammactrl ch,uint8_t val)
{
	uint8_t data;
	uint16_t addr = ((uint8_t)ch * 256) + val;
	EE_LL_Read(GAMMA_TABLE_PAGE, addr, 1, &data);
	return data;
}

