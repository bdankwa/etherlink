/*
 * A664Filter.cpp
 *
 *  Created on: Jun 22, 2016
 *      Author: cleadmin
 */

#include <unistd.h>
#include <stdio.h>
#include "A664Filter.h"

A664Filter::A664Filter() {

	arguments[0] = "udp2a664";
	arguments[1] = "-d";
	arguments[2] = "02:00:00:00:00:11"; //set dest MAC address
	arguments[3] = "-m";
	arguments[4] = "02:00:00:00:00:21"; // spoof source MAC address
	arguments[5] = "-s";
	arguments[6] = "10.168.20.1"; // spoof source IP address
	arguments[7] = "-i";
	arguments[8] = "eth0";
	arguments[9] = "32100"; //first local UDP port to listen to
	arguments[10] = "1"; // number of ports to listen to
	arguments[11] = "10.168.10.1"; // IP address to forward the A664 packets to
	arguments[12] = NULL;


	/*arguments[0] = "udp2a664";
	arguments[1] = "-d";
	arguments[2] = "02:00:00:00:00:12"; //set dest MAC address
	arguments[3] = "-m";
	arguments[4] = "02:00:00:00:00:22"; // spoof source MAC address
	arguments[5] = "-s";
	arguments[6] = "10.168.20.2"; // spoof source IP address
	arguments[7] = "-v";
	arguments[8] = "-i";
	arguments[9] = "eth0";
	arguments[10] = "32100"; //first local UDP port to listen to
	arguments[11] = "1"; // number of ports to listen to
	arguments[12] = "10.168.10.1"; // IP address to forward the A664 packets to
	arguments[13] = NULL; */

}

void A664Filter::run() {

	startA664_Filter();
}

void A664Filter::killudp2a664(){
	//Find a zombie and kill it
	system("sudo killall -9 udp2a664");
}

void A664Filter::setArguments(char** args){
	arguments[0] = args[0];
	arguments[1] = args[1];
	arguments[2] = args[2];
	arguments[3] = args[3];
	arguments[4] = args[4];
	arguments[5] = args[5];
	arguments[6] = args[6];
	arguments[7] = args[7];
	arguments[8] = args[8];
	arguments[9] = args[9];
	arguments[10] = args[10];
	arguments[11] = args[11];
	arguments[12] = NULL;
}

void A664Filter::startA664_Filter(){

	int pid, status;

	switch((pid = fork())){
	case -1:
		perror("fork");
		break;
	case 0:
		status = execvp("../udp2a664/Debug/udp2a664", arguments);
		perror("execvp");
		break;
	default:
		;
		break;
	}
}

A664Filter::~A664Filter() {
	// TODO Auto-generated destructor stub
}

