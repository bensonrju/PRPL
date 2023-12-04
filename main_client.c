/* Socket Client - Phase 2
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

main_client.c (main program)
   |- client.h (file path, functions)
	   |- unistd.h (getopt for CLI arguments)
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

#include "client.h"

// Pointer to the array of packets to send
packet_s *pkt_arr;

struct ack_packet_s
{
	int sequence_num;
};

// Ack packet structure
struct ack_packet_s ack_packet_recv;

int main(int argc, char **argv)
{

	// Check if the program should use a different
	//  input file other than the default
	int termOpt; // = getopt(argc, argv, "f:");

	while((termOpt = getopt(argc, argv, "f:i:")) != -1) {
		if(termOpt == 'f') {
			#ifdef DEBUG_MSG
			printf("Ignoring default file. Using %s instead. \n", optarg);
			#endif

			filePath = optarg;
		} else if (termOpt == 'i') {
			server_ip_addr = optarg;
			#ifdef DEBUG_MSG
			printf("Using IP address %s instead of default. \n", optarg);
			#endif
		}
	}
	// Call the packing function to create
	//  the packets we intend to send
	unsigned int num_packets = pack_packets(filePath, &pkt_arr);

	// create client socket using IPv4 and UDP
	int client_sfd = socket(AF_INET, SOCK_DGRAM, 0);
	// fcntl(client_sfd, F_SETFL, O_NONBLOCK);  //make socket non-blocking.

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

	int frame_no;
	int ack_recv = 0;

	// Sending packets
	for (int i = 0; i < num_packets; i++)
	{
		// current frame sequence number
		frame_no = pkt_arr[i].index;

		// While loop to wait for ACK
		ack_recv = 0; // haven't yet received ack
		while (!ack_recv)
		{
			// Transmit the current packet to the appropriate address
			sendto(client_sfd, (char *)&pkt_arr[i],
				   packet_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

#ifdef DEBUG_MSG
			printf("[client] sent packet #%d to server\n", pkt_arr[i].index);
#endif

			int addr_size = sizeof(server_addr);
			int recv_size = recvfrom(client_sfd, &ack_packet_recv, sizeof(ack_packet_recv), 0, (struct sockaddr *)&server_addr, &addr_size);

			// have we received the ack packet we were expecting?
			// if yes, allow the loop to exit and move onto the next packet
			if (ack_packet_recv.sequence_num == frame_no)
			{
				// note: the conditional here may need to slightly change when nonblocking gets implemented
				printf("correct ack received (%d)\n", frame_no);

				ack_recv = 1;
			}

			// if not, loop around and resend
			else
			{
				printf("incorrect ack received (got %d, expected %d), retrying\n", ack_packet_recv.sequence_num, frame_no);
			}
		}
#ifdef DEBUG_MSG
		printf("%d ", i);
#endif
	}

#ifdef DEBUG_MSG
	printf(" Done. \n");
#endif

	close(client_sfd);
	return 0;
}
