// The socket libraries we use
//  along with Team 2's custom functions
#include "simsocket.h"

// Our custom packet library
#include "simpack.h"

#include <stdio.h>
#include <time.h>

// Comment #define directive below to 
//  disable debug messages to the console
// #define DEBUG_MSG

//#define DEBUG_UNPACK_ABR

#define NSEC_PER_MS 1000000

//#define DEBUG_SLEEP

struct ack_packet_s
{
	int sequence_num;
};

#define TIMEOUT_INT 750

#define CLOCKS_PER_MSEC 1000/CLOCKS_PER_SEC

//clock_t timerStart;
time_t timerStart;

// Port number for both the client and the server
const unsigned int port_no = 12345;

// IP address that the client directs the message to
const char* server_ip_addr = "127.0.0.1";

int sleepMillisec(unsigned int msec) {
    int status;
	time_t time_in_ns = msec * NSEC_PER_MS;

    struct timespec remaining, request = { 0, time_in_ns };

    #ifdef DEBUG_SLEEP
    printf("Sleeping for %ld nanoseconds \n", time_in_ns);
    #endif

    status = nanosleep(&request, &remaining);

    #ifdef DEBUG_SLEEP
	if (status == 0) {
		printf("Sleep complete. \n");
	} else {
		printf("Error in sleeping. Code: %d After %ld ns \n", 
                    status, 
					time_in_ns - remaining.tv_nsec);
	}
    #endif

    return status;
}

int start_timeout() {
    timerStart = clock() * CLOCKS_PER_MSEC;

    return 0;
}

int check_timeout(time_t tout_interval) {
    time_t currTime = clock() * CLOCKS_PER_MSEC;
    if ((currTime - timerStart) > tout_interval) {
        return 1;
    } else {
        return 0;
    }
}

/*
int sendWithTimeout(
    int cli_sfd, 
    packet_s* targ_pkt, 
    ack_packet_s ack_addr,
    int ack_size, 
    struct sockaddr* srv_addr,
    int* size_ptr) {
        int recv_size;
        recv_size = 
            recvfrom(cli_sfd, targ_pkt, ack_size, 
                    0, srv_addr,size_ptr
                );
    }
*/