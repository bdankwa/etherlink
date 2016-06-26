/*
 * EthernetTransmit.cpp
 *
 *  Created on: Jun 22, 2016
 *      Author: cleadmin
 */

#include <sys/time.h>
#include "EthernetTransmit.h"
#include "ethernet.h"

EthernetTransmit::EthernetTransmit() {

    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(32028);
   // localAddr.sin_addr.s_addr = inet_addr("10.168.10.10");
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
    test = IDLE;
    txCount = 0;

    a664Filter = new A664Filter();

}

void EthernetTransmit::run() {

	int sent_size;
	int txRate_ms = 25;
	int count = MAX_TEST_LOOPS;
    struct timespec ts;

	ts = { txRate_ms / 1000, (txRate_ms % 1000) * 1000 * 1000 };
    test = CAPACITY;
    cancelled = false;

    a664Filter->start(); // Start filter

    while((count-->= 0) && (cancelled == false)){
		// TODO generate tx buffer
    	/*txBuffer[0] = 0xDEADBEEF;
    	txBuffer[1] = 0xBAADF00D;
    	txBuffer[2] = 0xEAEF2000;
    	txBuffer[3] = 0xF0247E30;
    	txBuffer[4] = 0x87952EA0;
    	txBuffer[5] = 0x87952EA0;
    	txBuffer[6] = 0x87952EA0;
    	txBuffer[7] = 0x87952EA0;
    	txBuffer[8] = 0x87952EA0;
    	txBuffer[9] = 0x87952EA0;
    	txBuffer[10] = 0x87952EA0;
    	txBuffer[11] = 0x87952EA0;
    	txBuffer[12] = 0x87952EA0;
    	txBuffer[13] = 0x87952EA0;
    	txBuffer[14] = 0x87952EA0;
    	txBuffer[15] = 0x87952EA0;
    	txBuffer[16] = 0x87952EA0;
    	txBuffer[17] = 0x87952EA0;
    	txBuffer[18] = 0x87952EA0;
    	txBuffer[19] = 0x87952EA0; */

    	generatePattern(txBuffer);

    	switch(test){
    	case CAPACITY:
    		nanosleep(&ts, NULL);
			transmit();
			txCount++;
			if(count == 0){
				test = IDLE; // For now only capacity test
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

    //Wait 1 second to ensure all final packets from EMU are received
    ts = { 1000 / 1000, (1000 % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);

    emit daignosticsComplete();

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
	//printf("Transmitted %i bytes\n", sent_size);
	emit newTxPacket(txBuffer, sent_size);

}

int EthernetTransmit::getTxCount(){
	return txCount;
}

void EthernetTransmit::reset(int channel){

	char* arguments[12];
	cancelled = true;
	test = IDLE;
	txCount = 0;
	cancelled = true;

	if(channel == 0){
		arguments[0] = "udp2a664";
		arguments[1] = "-d";
		arguments[2] = "02:00:00:00:00:11"; //set dest MAC address
		arguments[3] = "-m";
		arguments[4] = "34:E6:D7:36:C2:54"; // spoof source MAC address
		arguments[5] = "-s";
		arguments[6] = "10.168.10.10"; // spoof source IP address
		arguments[7] = "-v";
		arguments[8] = "-i";
		arguments[9] = "eth0";
		arguments[10] = "32100"; //first local UDP port to listen to
		arguments[11] = "1"; // number of ports to listen to
		arguments[12] = "10.168.10.1"; // IP address to forward the A664 packets to
	}
	else{
		arguments[0] = "udp2a664";
		arguments[1] = "-d";
		arguments[2] = "02:00:00:00:00:12"; //set dest MAC address
		arguments[3] = "-m";
		arguments[4] = "34:E6:D7:36:C2:54"; // spoof source MAC address
		arguments[5] = "-s";
		arguments[6] = "10.168.10.10"; // spoof source IP address
		arguments[7] = "-v";
		arguments[8] = "-i";
		arguments[9] = "eth0";
		arguments[10] = "32100"; //first local UDP port to listen to
		arguments[11] = "1"; // number of ports to listen to
		arguments[12] = "10.168.10.1"; // IP address to forward the A664 packets to
	}

	a664Filter->killudp2a664();
	a664Filter->setArguments(arguments);
}

void EthernetTransmit::cancel(){
	cancelled = true;
}

/****************************
 * getTime()
 *  Get current timer count.
 ****************************/
unsigned int EthernetTransmit::getTime(){

	struct timeval time;
	gettimeofday(&time, NULL);

	return(time.tv_usec);
}
/******************************************
 * generateRandomUint()
 *  Generate a random 32-bit unsigned int.
 ******************************************/
unsigned int EthernetTransmit::generateRandomUint(){

	//static uint2_t idx = 0;

	unsigned int lower = 0;
	unsigned int upper = 0xff;
	unsigned int x;

	x = (rand() % (upper - lower + 1) + lower) & 0xff;
	srand(getTime());
	x |= (rand() % (upper - lower + 1) + lower) << 8;
	srand(getTime());
	x |= (rand() % (upper - lower + 1) + lower) << 16;
	srand(getTime());
	x |= (rand() % (upper - lower + 1) + lower) << 24;

	return x;
}
/******************************************
 * generatePattern()
 *  Generate data pattern to transmit.
 ******************************************/
void EthernetTransmit::generatePattern(unsigned int * txBuff){
	int i;
	for(i=0; i<MAX_TX_BUFF_SIZE; i++){
		txBuff[i] = generateRandomUint();
	}
}






EthernetTransmit::~EthernetTransmit() {
	// TODO Auto-generated destructor stub
}

