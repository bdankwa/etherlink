/*
 * EthernetTransmit.h
 *
 *  Created on: Jun 22, 2016
 *      Author: cleadmin
 */

#ifndef ETHERNETTRANSMIT_H_
#define ETHERNETTRANSMIT_H_

#include <QObject>
#include <QThread>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "A664Filter.h"

#define MAX_TX_BUFF_SIZE (2048)
#define MAX_TEST_LOOPS (1000)

typedef enum{
	IDLE		= 0,
	CAPACITY 	= 1,
	RATE 		= 2,
	ROBUST		= 3
} TestType_e;

class EthernetTransmit : public QThread{
	Q_OBJECT

signals:
	void newTxPacket(unsigned int* txPacket, unsigned int size);
	void daignosticsComplete();

public:
	EthernetTransmit();
	void cancel();
	int getTxCount();
	void reset(int channel);
	int sizeToSend;
	virtual ~EthernetTransmit();

private:
	struct sockaddr_in localAddr;
	int socket_id;
	TestType_e test;
	bool cancelled;
	int txCount;
	A664Filter* a664Filter;
	unsigned int txBuffer[MAX_TX_BUFF_SIZE];
	unsigned int getTime();
	unsigned int generateRandomUint();
	void generatePattern(unsigned int * txBuff);
	void transmit();
	void run();
};

#endif /* ETHERNETTRANSMIT_H_ */
