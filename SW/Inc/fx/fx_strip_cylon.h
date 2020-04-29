/*
 * fx_strip_rainbow.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Michael Hartmann
 */

#ifndef FX_STRIP_CYLON_H_
#define FX_STRIP_CYLON_H_

//FX Description
//--------------
//Moving dot following a sine-pattern
//Uses real millisec timebase as an example
//Both strips show the same pattern


//Variables Used
//--------------
//STRIP1_SPEED		Overall Speed
//STRIP1_SIZE 		Afterglow length
//STRIP1_V1    		RED
//STRIP1_V2    		GREEN
//STRIP1_V3    		BLUE

void fx_strip_cylon(uint8_t fx_num);
t_fx_result fx_strip_cylon_run(t_fx_state state,uint32_t framecount,const uint32_t duration);

#endif /* FX_STRIP_RAINBOW_H_ */
