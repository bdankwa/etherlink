/*
 * global.h
 *
 *  Created on: May 13, 2016
 *      Author: cleadmin
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "buffer.h"

struct global {
	ringBuffer_t* rxBuff;
	ringBuffer_t* txBuff;
	int connectionStatus;
	void* exitStatus;
};

typedef struct {
	int argc;
	char** argv;
	int threadID;
	struct global* global_data;
}UIThreadArgs_t;

typedef struct {
	int threadID;
	u_int32_t* ports;
	int numPorts;
	struct global* global_data;
}EthernetThreadArgs_t;



#endif /* GLOBAL_H_ */
