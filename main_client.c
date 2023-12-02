/* Socket Client - Phase 2
 * 
 * Group 2
 * Justin Benson
 * Samantha DuBois
 * Ali Harb
 * Nolan Adams
 * 	
 * created 2023-11-07
 * updated 2023-11-28
 */


 /*
 Code library structure:

 main_client.c (main program)
   |- client.h (file path, functions)
        |- simhead.h (addr/port, debug flags)
             |- simsocket.h (functions to set up sockets)
             |    |- stdio.h
             |    |- stdlib.h
             |    |- stdint.h
             |    |- string.h
             |    |- math.h
             |    |- sys/types.h
             |    |- sys/socket.h
             |    |- netinet/in.h
             |    |- netinet/ip.h
             |    |- arpa/inet.h
    	     |- simpack.h (defines packet structure)
                  |- stdio.h
                  |- stdlib.h
                  |- stdint.h
                  |- string.h
                  |- sys/types.h
 */

#include "client.h"

// Pointer to the array of packets to send
packet_s* pkt_arr;


int main(int argc, char** argv) {


	// Check if the program should use a different 
	//  input file other than the default
	int termOpt = getopt(argc, argv, "f:");
	
	if(termOpt == 'f') {
		#ifdef DEBUG_MSG
		printf("Ignoring default file. Using %s instead. \n", optarg);
		#endif

		filePath = optarg;
	}
	

	// Call the packing function to create 
	//  the packets we intend to send
	unsigned int num_packets = pack_packets(filePath, &pkt_arr);

	// create client socket using IPv4 and UDP
	int client_sfd = socket(AF_INET, SOCK_DGRAM, 0);

	printf("[client] server address is %s:%d\n", 
			server_ip_addr, port_no);

	// sockaddr_in struct that defines the server address
	struct sockaddr_in server_addr;

	// Call the client-side server address definer function
	def_srv_addr_clientSide(
		&server_addr, server_ip_addr, port_no);

	#ifdef DEBUG_MSG
	printf("[client] sending packets... ");
	#endif

	// Sending packets
	for (int i=0; i<num_packets; i++) {
		
		// Transmit the current packet to the appropriate address
		sendto(client_sfd, (char*)&pkt_arr[i], 
			packet_size, 0, (struct sockaddr*)&server_addr, 
			sizeof(server_addr));

		#ifdef DEBUG_MSG
		printf("%d ", i);
		#endif

	}

	#ifdef DEBUG_MSG
	printf(" Done. \n");
	#endif
	
	return 0;
}
