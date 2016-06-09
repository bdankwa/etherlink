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

#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>

/**********************************************
 * Macros for printing error messages
 *********************************************/
#define handle_error(msg) \
	do{ fprintf(stderr, msg); printUsage(); exit(EXIT_FAILURE); } while(0)

#define print_error(msg) do{perror(msg);} while (0)

#define critical_error(msg) \
	do{ fprintf(stderr, msg); exit(EXIT_FAILURE); } while(0)
/******************************************************************************
* Function name: generateRandomInt
* Parameters: lower - lower limit
*             upper - upper limit
* Return: random integer in the range [lower , upper].
*
* Description: This function generates a random integer in the range [lower , upper] .
*******************************************************************************/
int generateRandomInt(int lower, int upper,int seed);


/******************************************************************************
* Function name: getCurrentTime
* Parameters: None
* Return: seconds since epoch.
*
* Description: This function returns the current time since epoch in seconds .
*******************************************************************************/
double getCurrentTime(void);

#endif /* UTILS_H_ */
