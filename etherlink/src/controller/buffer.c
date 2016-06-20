/*
 * buffer.c
 *
 *  Created on: May 11, 2016
 *      Author: cleadmin
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "buffer.h"

#define MAX_BUFF_SIZE	(RX_BUFF_SZ * 10) /* 1 second of data*/
#define MAX_BUFF_LEN (MAX_BUFF_SIZE / 4)

#define MAX_SIZE_IDX	(10)


ringBuffer_t buffer;

/******************************************************************************
* Function name: initBuffer
* Parameters: Pointer to buffer
* Return: None.
*
* Description: This function initializes ring buffer.
*******************************************************************************/
void initBuffer(ringBuffer_t* buff){

	u_int32_t* data = (u_int32_t*)malloc(MAX_BUFF_SIZE);
	memset(data, 0, MAX_BUFF_SIZE );

	u_int32_t* size = (u_int32_t*)malloc(MAX_SIZE_IDX * sizeof(u_int32_t));
	memset(data, 0, MAX_SIZE_IDX );

	buff->data = data;
	buff->head = 0;
	buff->tail = 0;
	buff->size = size;
	buff->head_size_idx = 0;
	buff->tail_size_idx = 0;
}

/******************************************************************************
* Function name: addDataToBuffer
* Parameters: Pointer to buffer, data
* Return: Status ( 0 = data added,  -1 = buffer full.
*
* Description: This function adds a data element to the buffer.
*******************************************************************************/
int addDataToBuffer(ringBuffer_t* buff, void* data, u_int32_t data_size){

	unsigned int index = 0;
	unsigned int size_idx = 0;

	unsigned int head, tail;

	head = buff->head;
	tail = buff->tail;

	if((head - tail) == MAX_BUFF_LEN ){
		return -1;
	}

	index = buff->head % MAX_BUFF_LEN;

	//buff->data[index] = *(u_int32_t*)data; //TODO memcpy using data_len
	memcpy((buff->data)+index, (u_int32_t*)data, data_size);

	size_idx = buff->head_size_idx % MAX_SIZE_IDX;
	(buff->size)[size_idx] = data_size;

	//printf("Added data %X,\n", ((buff->data)+index)[0]);

	__sync_fetch_and_add(&(buff->head_size_idx), 1); // atomic increment

	__sync_fetch_and_add(&(buff->head), data_size / sizeof(u_int32_t)); // atomic increment

	return 0;
}

/******************************************************************************
* Function name: removeDataFromBuffer
* Parameters: Pointer to buffer, data
* Return: Status ( 0 = data removed,  -1 = buffer empty.
*
* Description: This function removes a data element from the buffer.
*******************************************************************************/
int removeDataFromBuffer(ringBuffer_t* buff, u_int32_t** data, u_int32_t* size){

	unsigned int index = 0;
	unsigned int size_idx = 0;
	unsigned int data_size = 0;

	unsigned int head, tail;

	head = buff->head;
	tail = buff->tail;

	if((head - tail) == 0){
		return -1;
	}

	index = buff->tail % MAX_BUFF_LEN;

	size_idx = buff->tail_size_idx % MAX_SIZE_IDX;

	data_size = (buff->size)[size_idx];

	//printf("size idx %i, size %i\n", size_idx, (buff->size)[size_idx]);

	//*data = buff->data[index]; // TODO memcpy using buff->tail_size_idx
	//memcpy(*(data), (buff->data)+index, data_size );
	*(data) = (buff->data)+index;

	*size = data_size;

	//printf("Removed data %X,\n", ((buff->data)+index)[0]);

	__sync_fetch_and_add(&(buff->tail_size_idx), 1); // atomic increment

	__sync_fetch_and_add(&(buff->tail), data_size / sizeof(u_int32_t)); // atomic increment


	return 0;
}

/******************************************************************************
* Function name: deleteBuffer
* Parameters: Pointer to buffer
* Return: None.
*
* Description: This function deletes buffer.
*******************************************************************************/
void deleteBuffer(ringBuffer_t* buff){
	free(buff->data);
	free(buff->size);
}

