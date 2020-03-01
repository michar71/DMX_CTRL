/*
 * dmx512_rec.h
 *
 *  Created on: 27.1.2013
 *      Author: honza
 */

#ifndef DMX512_REC_H_
#define DMX512_REC_H_

#include <stdint.h>



enum packet_type {
	DATA_PACKET = 0,
	TEST_PACKET,
};


void dmx512_rec_init(void);
void dmx512_rec_enable(uint8_t on);
void dmx512_setRx(void);
void dmx512_setTx(void);

#endif /* DMX512_REC_H_ */
