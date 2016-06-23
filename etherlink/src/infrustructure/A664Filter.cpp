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
	arguments[2] = "02:00:00:00:00:11";
	arguments[3] = "-m";
	arguments[4] = "02:00:00:00:00:21";
	arguments[5] = "-s";
	arguments[6] = "10.168.20.1";
	arguments[7] = "-v";
	arguments[8] = "-i";
	arguments[9] = "eth0";
	arguments[10] = "32100";
	arguments[11] = "1";
	arguments[12] = "10.168.10.1";
	arguments[13] = NULL;

}

void A664Filter::run() {

	startA664_Filter();
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

