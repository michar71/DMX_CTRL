/*
 * fx_api.h
 *
 *  Created on: Apr 7, 2020
 *      Author: Michael
 */

#ifndef FX_API_H_
#define FX_API_H_

#include <stdint.h>

//Reset the Frame Delay Counter
void reset_frame_delay(void);

//Returns true if "delay" frames have passed, else false
uint8_t check_frame_delay(uint32_t delay);


#endif /* FX_API_H_ */
