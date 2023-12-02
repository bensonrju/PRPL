/* Socket Server - Phase 2
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

 main_server.c (main program)
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

#include "simhead.h"

// char output_file_path[] = "output.jpg";

int status = 0;

// holds packet being received
packet_s recv_pkt;

int main(int argc, char** argv) {
	// debug message
	printf("[server] listening to socket on port %d\n", 
			port_no);
	
	// create socket using:
	//   AF_INET (IPv4)
	//   SOCK_DGRAM (UDP, 0)
	int server_sfd;
	server_sfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	// define server address using sockaddr_in data structure
	struct sockaddr_in server_addr;

	// define client address using sockaddr_in data structure
	// (we need the client's info to send ACKs)
	struct sockaddr_in client_addr = {0};
	unsigned int client_addrlen = sizeof(client_addr);

	// Call the server-side server address definer function
	def_srv_addr_serverSide(&server_addr, port_no);

	// Bind socket to address (only necessary on server end)
	int sock_bind;
	sock_bind = bind(server_sfd, 
					(struct sockaddr*)&server_addr, 
					sizeof(server_addr)); 
	
	// Error handling
	if (sock_bind != 0) {
		perror("[server] bind() error:");
		exit(1);
	}
	
	// Debug message for listening
	#ifdef DEBUG_MSG
	printf("[server] waiting for message\n");
	#endif

	// receiving packets loop
	while (1) {
		// receive packet (blocking)
		int msg_size = recvfrom(server_sfd, // int sockfd
						        &recv_pkt, // void *buf
						        sizeof(recv_pkt), // size_t len
								0, // int flags
								(struct sockaddr*)&client_addr, // struct sockaddr *src_addr
						        &client_addrlen // socklen_t *addrlen
								);
		
		// error handling
		if (msg_size < 0) {
			perror("[server] recvfrom()");
			exit(1);
		}

		#ifdef DEBUG_UNPACK_ABR
		if (!status) {
			printf("[server] Receiving packets... ");
			status = 1;
		} else { printf("%d ", recv_pkt.index); }
		#endif


		// debug info: show received string
		#ifdef DEBUG_MSG
		printf("[server] received packet %d from %s:%d :: \"%s\"\n", 
				recv_pkt.index, inet_ntoa(client_addr.sin_addr), 
				ntohs(client_addr.sin_port), recv_pkt.payload);
		/*
		received message's source ip address and port numbers:
			- raw ip:   client_addr.sin_addr
			- raw port: client_addr.sin_port

		converted to readable formats for printf():
			- ip (string): inet_ntoa(client_addr.sin_addr)
			- port (int):  ntohs(client_addr.sin_port)

		note: source port number will probably be different from destination port number
			  if both server and client are running on the same machine
		*/
		#endif

		// check if final packet was received
		if (recv_pkt.incoming == 0) {
			printf("\n[server] finished receiving all packets\n");
			break;
		}
	}
	
	return 0;

}
