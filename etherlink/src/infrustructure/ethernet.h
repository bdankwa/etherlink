/*
 * ethernet_rx.h
 *
 *  Created on: May 11, 2016
 *      Author: cleadmin
 */

#ifndef ETHERNET_RX_H_
#define ETHERNET_RX_H_

#include <sys/socket.h>

int ethernet_init(const u_int32_t* ports, u_int32_t numPortIPPair,  u_int32_t* sockets, int* maxfd, fd_set* sockts);
u_int32_t ethernet_rx(int socket_id, void* msgBuff, int msgLenBytes, struct sockaddr* remoteAddr, socklen_t * remoteAddrLen);
u_int32_t ethernet_tx(int socket_id, void* msgBuff, int msgLenBytes, struct sockaddr* destAddr, socklen_t * destAddrLen);
u_int32_t ethernet_loopback(u_int8_t* destMAC, u_int8_t* spoofedMAC, u_int8_t* spoofedIP, int portOffset, int basePort, int numPorts, char* interfaceName);



#endif /* ETHERNET_RX_H_ */
