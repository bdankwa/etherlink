/*
 * buffer.h
 *
 *  Created on: May 11, 2016
 *      Author: cleadmin
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdlib.h>

#define RX_BUFF_SZ		(17*2*1024*4)
//#define RX_BUFF_SZ		(4)

/* Type for shared buffer */
typedef struct{
	u_int32_t* data; // data array
	u_int32_t head; // head of ring buffer
	u_int32_t tail; // tail of ring buffer
	u_int32_t* size;
	u_int32_t head_size_idx;
	u_int32_t tail_size_idx;
}ringBuffer_t;

/******************************************************************************
* Function name: initBuffer
* Parameters: Pointer to buffer
* Return: None.
*
* Description: This function initializes ring buffer.
*******************************************************************************/
void initBuffer(ringBuffer_t* buff);

/******************************************************************************
* Function name: addDataToBuffer
* Parameters: Pointer to buffer, data
* Return: Status ( 0 = data added,  -1 = buffer full.
*
* Description: This function adds a data element to the buffer.
*******************************************************************************/
int addDataToBuffer(ringBuffer_t* buff, void* data, u_int32_t data_size);

/******************************************************************************
* Function name: removeDataFromBuffer
* Parameters: Pointer to buffer, data
* Return: Status ( 0 = data removed,  -1 = buffer empty.
*
* Description: This function removes a data element from the buffer.
*******************************************************************************/
int removeDataFromBuffer(ringBuffer_t* buff, u_int32_t** data, u_int32_t* size);

/******************************************************************************
* Function name: deleteBuffer
* Parameters: Pointer to buffer
* Return: None.
*
* Description: This function deletes buffer.
*******************************************************************************/
void deleteBuffer(ringBuffer_t* buff);


#endif /* BUFFER_H_ */
