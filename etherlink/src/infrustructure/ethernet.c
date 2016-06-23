/*
 * ethernet_rx.c
 *
 *  Created on: May 11, 2016
 *      Author: cleadmin
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <net/if.h>

#include "ethernet.h"
#include "utils.h"

int ethernet_init(const u_int32_t* ports, u_int32_t numPorts,  u_int32_t* sockets, int* maxfd, fd_set* sockts){

	u_int32_t i;
	int socket_id;

	char msg[50];

	FD_ZERO(sockts);

	for(i=0; i<numPorts; i++){
		if((socket_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
			print_error("Unable to create UDP socket");
			return -1;
		}

		struct sockaddr_in localAddr;
	    /* Local address structure */
	    localAddr.sin_family = AF_INET;
	    localAddr.sin_port = htons(ports[i]);
	    localAddr.sin_addr.s_addr = INADDR_ANY;

		if(bind(socket_id, (struct sockaddr*)&localAddr, sizeof(localAddr)) == -1){
			sprintf(msg, "Unable to bind port %i to socket %i", ports[i], socket_id);
			print_error(msg);
			return -1;
		}

		FD_SET(socket_id, sockts);

		sockets[i] = socket_id;

        /* Record the max file descriptor for the select statement */
        if (socket_id > *maxfd)
           *maxfd = socket_id;

	}

	return 0;
}
u_int32_t ethernet_rx(int socket_id, void* msgBuff, int msgLenBytes, struct sockaddr* remoteAddr, socklen_t* remoteAddrLen){

	return recvfrom(socket_id, msgBuff, msgLenBytes, 0, remoteAddr, remoteAddrLen);

}
u_int32_t ethernet_tx(int socket_id, void* msgBuff, int msgLenBytes, struct sockaddr* destAddr, socklen_t destAddrLen){


	return sendto(socket_id, msgBuff, msgLenBytes,0, destAddr, destAddrLen );
}

