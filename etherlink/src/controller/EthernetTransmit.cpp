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
    localAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

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
    sizeToSend = 512;

}

void EthernetTransmit::run() {

	int sent_size;
	struct sockaddr_in remoteAddr;

	while(1){
		// TODO generate tx buffer

    	/* send to source whatever was received using regular socket */
    	if((sent_size = ethernet_tx(socket_id, txBuffer, sizeToSend, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr))) == -1){
        	perror("Ethernet transmit ethernet_tx");
        	exit(EXIT_FAILURE);
    	}
    	printf("Transmitting...\n");
    	emit newTxPacket(txBuffer, sent_size);

	}

}

EthernetTransmit::~EthernetTransmit() {
	// TODO Auto-generated destructor stub
}

