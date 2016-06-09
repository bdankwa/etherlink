/*
 * EthernetReader.cpp
 *
 *  Created on: May 17, 2016
 *      Author: bdankwa
 */

#include "EthernetReader.h"
#include <stdio.h>

EthernetReader::EthernetReader(ringBuffer_t* rxBuffer) {
	this->rxBuffer = rxBuffer;

}

void EthernetReader::run() {

	unsigned int* packet;
	unsigned int size;

	while(1){
		if (removeDataFromBuffer(rxBuffer, &packet, &size) == -1){
			printf("Buffer is empty, tail at %d \n", rxBuffer->tail);
			continue;
		}
	    printf("Packet of size %i received, emiting signal\n", size);
		emit newPacket(packet, size);
	}
}


