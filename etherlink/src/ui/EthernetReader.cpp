/*
 * EthernetReader.cpp
 *
 *  Created on: May 17, 2016
 *      Author: bdankwa
 */

#include "EthernetReader.h"
#include <stdio.h>


EthernetReader::EthernetReader(ringBuffer_t* rxBuffer, int numPorts) {
	this->rxBuffer = rxBuffer;
	this->numPorts = numPorts;
	pauseFlag = false;
	refreshRate_ms = 1;

}

void EthernetReader::pause() {
	pauseFlag = true;
}

void EthernetReader::resume() {
	pauseFlag = false;
}

void EthernetReader::setRefreshRate(int rate_ms) {
	refreshRate_ms = rate_ms;
}

void EthernetReader::run() {

	unsigned int* packet;
	unsigned int size;
	int i;
    struct timespec ts;


	while(!pauseFlag){

		ts = { refreshRate_ms / 1000, (refreshRate_ms % 1000) * 1000 * 1000 };
		nanosleep(&ts, NULL);

		for(i=0; i< numPorts; i++){
			if (removeDataFromBuffer(&(rxBuffer[i]), &packet, &size) == -1){
				//printf("Buffer is empty, tail at %d \n", (&(rxBuffer[i]))->tail);
				continue;
			}
		    //printf("Port %i received something, emiting signal\n", i);
			emit newPacket(packet, size, i);
		}

	}
}

EthernetReader::~EthernetReader() {
	int i;
	for(i=0; i< numPorts; i++){
		deleteBuffer(&(rxBuffer[i]));
	}
}


