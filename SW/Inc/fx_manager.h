/*
 * fx_manager.h
 *
 *  Created on: Apr 7, 2020
 *      Author: Michael
 */

#ifndef FX_MANAGER_H_
#define FX_MANAGER_H_

#include <stdint.h>

//Called once at startup. Hook to install the Effects to the FX List
void install_fx(void);

//Called to start a new effect. Return the effect that was actually enabled
uint8_t start_fx(uint8_t id);

//Called every frame
void run_fx(void);


#endif /* FX_MANAGER_H_ */
