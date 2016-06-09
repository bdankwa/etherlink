/*
 ============================================================================
 Name        : utils.h
 Author      : Boakye Dankwa
 Version     :
 Course      : CSCI503 Operating Systems
 Project	 : Project 3
 Description : Utilities for project 3
 ============================================================================
 */
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>

#include "utils.h"

/******************************************************************************
* Function name: generateRandomInt
* Parameters: lower - lower limit
*             upper - upper limit
* Return: random integer in the range [lower , upper].
*
* Description: This function generates a random integer in the range [lower , upper] .
*******************************************************************************/
int generateRandomInt(int lower, int upper,int seed){
	struct timeval   tv;
	struct timezone  tz;
	double cur_time;

	gettimeofday(&tv, &tz);
	cur_time = tv.tv_sec + tv.tv_usec;

	srand(cur_time);// seed random number generator with current time

	return (rand() % (upper - lower + 1) + lower);
}

/******************************************************************************
* Function name: getCurrentTime
* Parameters: None
* Return: seconds since epoch.
*
* Description: This function returns the current time since epoch in seconds .
*******************************************************************************/
double getCurrentTime(void){
	struct timeval   tv;
	struct timezone  tz;
	double cur_time;

	gettimeofday(&tv, &tz);
	cur_time = tv.tv_sec + tv.tv_usec / 1000000.0;

	return cur_time;
}
