/*
 * EthernetTransmit.cpp
 *
 *  Created on: Jun 22, 2016
 *      Author: cleadmin
 */

#include "EthernetTransmit.h"
#include "ethernet.h"

EthernetTransmit::EthernetTransmit() {

    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(32000);
    localAddr.sin_addr.s_addr = inet_addr("10.168.10.10");

    /* Create a UDP socket */
    socket_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_id == -1){
    	perror("Ethernet transmit socket");
    	exit(EXIT_FAILURE);
    }

    if(bind(socket_id, (struct sockaddr*)&localAddr, sizeof(localAddr)) == -1){
    	perror("Ethernet transmit bind");
    	exit(EXIT_FAILURE);
    }

    memset(txBuffer, 0, sizeof(unsigned int)*MAX_TX_BUFF_SIZE);
    sizeToSend = 20;
    test = IDLE;

}

void EthernetTransmit::run() {

	int sent_size;
	int txRate_ms = 25;
	int count = MAX_TEST_LOOPS * 3;
    struct timespec ts;

	ts = { txRate_ms / 1000, (txRate_ms % 1000) * 1000 * 1000 };
    test = CAPACITY;
    cancelled = false;

    while((count-->= 0) && (cancelled == false)){
		// TODO generate tx buffer
    	txBuffer[0] = 0xDEADBEEF;
    	txBuffer[1] = 0xBAADF00D;
    	txBuffer[2] = 0xEAEF2000;
    	txBuffer[3] = 0xF0247E30;
    	txBuffer[4] = 0x87952EA0;

    	switch(test){
    	case CAPACITY:
    		nanosleep(&ts, NULL);
			transmit();
			if(count == 2* MAX_TEST_LOOPS){
				test = RATE;
			}
    		break;
    	case RATE:
    		nanosleep(&ts, NULL);
			transmit();
			if(count == MAX_TEST_LOOPS){
				test = ROBUST;
			}
    		break;
    	case ROBUST:
    		nanosleep(&ts, NULL);
			transmit();
			if(count == 0){
				test = IDLE;
			}
    		break;
    	default:
    		;
    		break;
    	}

    }

}

void EthernetTransmit::transmit(){

	int sent_size;
	struct sockaddr_in remoteAddr;

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(32100);
	remoteAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* send to source whatever was received using regular socket */
	if((sent_size = ethernet_tx(socket_id, txBuffer, sizeToSend, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr))) == -1){
    	perror("Ethernet transmit ethernet_tx");
    	exit(EXIT_FAILURE);
	}
	printf("Transmitted %i bytes\n", sent_size);
	emit newTxPacket(txBuffer, sent_size);

}

void EthernetTransmit::cancel(){
	cancelled = true;
	test = IDLE;
}

EthernetTransmit::~EthernetTransmit() {
	// TODO Auto-generated destructor stub
}

