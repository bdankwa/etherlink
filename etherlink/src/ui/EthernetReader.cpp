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

}

void EthernetReader::run() {

	unsigned int* packet;
	unsigned int size;
	int i;

	while(1){

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


