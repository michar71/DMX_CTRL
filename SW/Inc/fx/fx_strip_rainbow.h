/*
 * fx_strip_rainbow.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Michael Hartmann
 */

#ifndef FX_STRIP_RAINBOW_H_
#define FX_STRIP_RAINBOW_H_

//FX Description
//--------------
//Scrolling Rainbow Effect


//Variables Used
//--------------
//STRIP1_SPEED		Overall Speed
//STRIP1_SIZE 		Stepsize
//STRIP1_PATTERN    0 = Forward, 1 = Reverse
//STRIP1_COMPLEXITY Startvalue
//STRIP2_SPEED		Overall Speed
//STRIP2_SIZE 		Stepsize
//STRIP2_PATTERN    0 = Forward, 1 = Reverse
//STRIP2_COMPLEXITY Startvalue

void fx_strip_rainbow(uint8_t fx_num);
t_fx_result fx_strip_rainbow_run(t_fx_state state,uint32_t framecount,const uint32_t duration);

#endif /* FX_STRIP_RAINBOW_H_ */
