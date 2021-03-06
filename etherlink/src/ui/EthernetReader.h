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
	void newPacket(unsigned int* packet, unsigned int size, int portIdx );

public:
	EthernetReader(ringBuffer_t* rxBuffer, int numPorts);
	void pause(void);
	void resume(void);
	void setRefreshRate(int rate_ms);
	~EthernetReader();

private:
	ringBuffer_t* rxBuffer;
	int numPorts;
	bool pauseFlag;
	int refreshRate_ms;
	void run();
};

#endif /* ETHERNETREADER_H_ */
