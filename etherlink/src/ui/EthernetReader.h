/*
 * EthernetReader.h
 *
 *  Created on: May 17, 2016
 *      Author: bdankwa
 */

#ifndef ETHERNETREADER_H_
#define ETHERNETREADER_H_

#include <QObject>
#include <QThread>
#include <stdlib.h>
#include "buffer.h"

class EthernetReader : public QThread {
	Q_OBJECT

signals:
	void newPacket(unsigned int* packet, unsigned int size );

public:
	EthernetReader(ringBuffer_t* rxBuffer);

private:
	ringBuffer_t* rxBuffer;
	void run();
};

#endif /* ETHERNETREADER_H_ */
