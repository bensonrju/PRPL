/* Socket Server - Phase 2
 *
 * Group 2
 * Justin Benson
 * Samantha DuBois
 * Ali Harb
 * Nolan Adams
 *
 * created 2023-11-07
 * updated 2023-12-04
 */

/*
Code library structure:

main_server.c (main program)
  |- simhead.h (addr/port, debug flags)
	   |- stdio.h
	   |- simpack.h (defines packet structure)
	   |- simsocket.h (functions to set up sockets)
	   |    |- stdlib.h
	   |    |- string.h
	   |    |- sys/socket.h
	   |    |- netinet/in.h
	   |    |- netinet/ip.h
	   |    |- arpa/inet.h
*/

#include "simhead.h"
#include <unistd.h>
//#include <time.h>

//#define DEBUG_MSG_F

#define DEBUG_MSG

char output_file_path[] = "output.jpg";

int status = 0;
int count = 0;
int tot_pkts = 0;

/*
struct ack_packet_s
{
	int sequence_num;
};
*/

struct ack_packet_s ack_packet_send;

socket_s pSocket;

// holds packet being received
packet_s recv_pkt;

int main(int argc, char **argv)
{
	// debug message
	printf("[server] listening to socket on port %d\n",
		   port_no);

	// create socket using:
	//   AF_INET (IPv4)
	//   SOCK_DGRAM (UDP, 0)
	pSocket.fd = socket(AF_INET, SOCK_DGRAM, 0);
	//int server_sfd = socket(AF_INET, SOCK_DGRAM, 0);

	// define server address using sockaddr_in data structure
	struct sockaddr_in server_addr;
	socklen_t server_addr_size = sizeof(server_addr);

	// define client address using sockaddr_in data structure
	// (we need the client's info to send ACKs)
	struct sockaddr_in client_addr = {0};
	socklen_t client_addrlen = sizeof(client_addr);

	// Call the server-side server address definer function
	def_srv_addr_serverSide(&server_addr, port_no);

	// Bind socket to address (only necessary on server end)
	int sock_bind;
	sock_bind = bind(pSocket.fd,
					 (struct sockaddr_in *)&server_addr,
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

	int buf_index = 0;
	uint8_t *buffer;

	// receiving packets loop
	int all_packets_recv = 0;
	while (!all_packets_recv) {
		// receive packet (blocking)
		int msg_size = 
			recvfrom(pSocket.fd,		// int sockfd
					&recv_pkt,			// void *buf
					sizeof(recv_pkt),	// size_t len
					0,					// int flags
					// struct sockaddr *src_addr
					(struct sockaddr *)&client_addr, 
					&client_addrlen		// socklen_t *addrlen
					);

		// TODO: use total msg size to set buffer size
		if (count == 0) {
			buffer = (uint8_t *)malloc(recv_pkt.tot_pkts * PAYLOAD_SIZE);
			printf("[DEBUGGING] buffer size: %d\n", 
					recv_pkt.tot_pkts * PAYLOAD_SIZE);
		}

		// TODO: recv_pkt -> go to -> buffer
		for (int i = 0; i < recv_pkt.payload_len; i++) {
			// printf("[DEBUGGING]%d - recv_pkt: %x\n",buf_index, recv_pkt.payload[i]);
			buffer[buf_index] = recv_pkt.payload[i];
			buf_index++;
		}
		// buf_index++;
		// buffer[buf_index] = (uint16_t)recv_pkt.payload;
		// printf("[DEBUGGING] buf: %s\n\n", buffer);

		// error handling
		if (msg_size < 0) {
			perror("[server] recvfrom()");
			exit(1);
		}

		#ifdef DEBUG_UNPACK_ABR
			if (!status) {
				printf("[server] Receiving packets... ");
				status = 1;
			} else { 
				printf("%d ", recv_pkt.index); 
			}
		#endif

		// debug info: show received string
		#ifdef DEBUG_MSG_F
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

		note: source port number will probably be 
			different from destination port number
	  		if both server and client are 
			running on the same machine
		*/
		#endif

		// If we received a valid message, we should acknowledge it
		if (msg_size > 0) {
			#ifdef DEBUG_MSG
				printf("[server] frame received: %d\n", 
							recv_pkt.index);
			#endif

			ack_packet_send.sequence_num = recv_pkt.index;

			sleepMillisec(750);

			// send ack
			// note: gets sent to 'client_addr' not 'server_addr',
			//       'client_addr' is the source address (i.e. return address) we got from the recvfrom() function
			sendto( pSocket.fd, &ack_packet_send, 
				sizeof(ack_packet_send), 0, 
				(struct sockaddr *)&client_addr, 
				client_addrlen
			);

			#ifdef DEBUG_MSG
				printf("[server] ack sent (%d)\n", 
						ack_packet_send.sequence_num);
			#endif

			count++;
			
		} else {
			printf("[server] frame not received\n"); 
		}

		// check if final packet was received
		if (ack_packet_send.sequence_num == recv_pkt.tot_pkts - 1 
				&& count == recv_pkt.tot_pkts) {
			printf("\n[server] finished receiving all packets\n");
			all_packets_recv = 1;
		}
	}

	// file pointer
	FILE *fp;

	// Open the target file
	fp = fopen(output_file_path, "wb");
	if (fp == NULL) {
		printf("ERROR: Could not open file. \n");
		return 0;
	}

	// TODO: reassembly file from buffer

	int extra = sizeof(recv_pkt.payload) - recv_pkt.payload_len;
	// printf("[DEBUGGING] extra: %d\n", extra);
	// printf("[DEBUGGING]:\"%s\"\t", buffer);

	// Place buffer into output file
	//fprintf(fp,"%s",buffer);
	int num_bytes = ((recv_pkt.tot_pkts - 1) * PAYLOAD_SIZE)
					+ extra;

	fwrite(buffer, sizeof(uint8_t), num_bytes, fp);

	fclose(fp);

	close(pSocket.fd);
	return 0;
}
