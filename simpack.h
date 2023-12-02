#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>

#define PAYLOAD_SIZE 1024

// 
/*
 * Data Type: packet_s
 * Alt Name: Simple Packet
 *
 * Description:	The data structure for 
 *				Team 2's simple packet data structure
 * Members:
 *  - index: Determines the packet's place in the 
 * 			  entire set of packets
 *  - incoming: Flag that determines whether more packets 
 * 			are to come or if it is the last one in the set
 *  - payload_len: Length of the payload data
 * 
 *  - payload: Array of bytes representing the payload data
 */
typedef struct _packet_s {
	// which fragment is this (integer 1-255)?
    uint8_t  index;

	// are there more fragments coming? 
	//  (integer, yes: 1, no: 0)	
	uint8_t  incoming;

	// length of payload data
	uint16_t payload_len;

	// payload data  
	uint8_t  payload[PAYLOAD_SIZE];   
} packet_s;
