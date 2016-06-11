/*
 * process_ethernet.c
 *
 *  Created on: May 13, 2016
 *      Author: cleadmin
 */

#include <netinet/in.h>
#include <pthread.h>

#include "ethernet.h"
#include "process_ethernet.h"
#include "buffer.h"
#include "utils.h"

static int maxfd = 0;
static fd_set sockets;
static u_int32_t* socket_ids;

int initialize_ethernet(u_int32_t* ports, u_int32_t numPortIPPair);

void* ethernet_thread(void* params){


	EthernetThreadArgs_t* args = (EthernetThreadArgs_t*)params;

	char msg[50];
	struct sockaddr_in remoteAddr;
	socklen_t remoteAddrSize = sizeof(remoteAddr);
	u_int32_t buf[RX_BUFF_SZ];
	u_int32_t* ports;
	u_int32_t nPorts = 0;

	u_int32_t i;
	int rx_size = 0;

	//void* retval;
	//ringBuffer_t* rxUDPBuffer = (ringBuffer_t*)malloc(sizeof(ringBuffer_t)*nPorts);
	ringBuffer_t* rxUDPBuffer = args->global_data->rxBuff;
	void* retval = args->global_data->exitStatus;
	ports = args->ports;
	nPorts = args->numPorts;

	if(initialize_ethernet(ports, nPorts) == -1){
		critical_error("Unable to initialize Ethernet!");
	}


	/****************************************************
	 * Process Receive
	 ***************************************************/

	while(1){
		//printf("Ethernet thread running...\n");
		args->global_data->connectionStatus = -1;
	       if((select(maxfd + 1, &sockets, (fd_set *)0, (fd_set *)0, 0)) == 0) {
	    	   print_error("Ethernet thread error: ethernet_thread.select");
	    	   //TODO return this in args/
	    	   pthread_exit(retval);
	       }

	       args->global_data->connectionStatus = 0;

	        for (i=0;i<nPorts;i++) {
	             if (FD_ISSET(socket_ids[i], &sockets)) {
	                if((rx_size = ethernet_rx(socket_ids[i], buf, RX_BUFF_SZ, (struct sockaddr *) &remoteAddr, &remoteAddrSize)) == -1){
	     	    	   print_error("Ethernet thread error: ethernet_thread.ethernet_rx");
	     	    	   //TODO return this in args/
	     	    	   pthread_exit(retval);
	                }

	                if(addDataToBuffer(&rxUDPBuffer[i], buf, rx_size) == -1){
	        			sprintf(msg, "Ethernet rx buffer overflow on port %i", ports[i]);
	        			print_error(msg);

	                }
	                //printf("Producer added data to buffer, head @ %d.\n", rxUDPBuffer[i].head );
	              }
	          }
	          // Reset the FD set
	          FD_ZERO(&sockets);
	          for (i=0;i<nPorts;i++) {
	              FD_SET(socket_ids[i], &sockets);
	          }

	}


	/****************************************************
	 * TODO: Process Transmit
	 ***************************************************/

	return (void*)NULL;

}

int initialize_ethernet(u_int32_t* rx_ports, u_int32_t numPorts){

	socket_ids = (u_int32_t*)malloc(sizeof(u_int32_t)*numPorts);

	if(ethernet_init(rx_ports, numPorts,  socket_ids, &maxfd, &sockets) == -1){
		return -1;
	}
	return 0;
}
