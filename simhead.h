// The socket libraries we use
//  along with Team 2's custom functions
#include "simsocket.h"

// Our custom packet library
#include "simpack.h"

#include <stdio.h>

// Comment #define directive below to 
//  disable debug messages to the console
// #define DEBUG_MSG

#define DEBUG_UNPACK_ABR

// Port number for both the client and the server
const int port_no = 12345;

// IP address that the client directs the message to
const char* server_ip_addr = "127.0.0.1";