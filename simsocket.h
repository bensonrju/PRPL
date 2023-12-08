#include <stdlib.h>
#include <string.h>

// Libraries needed for socket I/O
//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

enum mch { server, client };

typedef struct _semi_socket_s
{    
    // Port Number
    unsigned int port_no;

    // Destination IP Address
    char* ip;

	// Socket IP address data
	struct sockaddr_in addr;

	// Size of the address data structure
	socklen_t addr_len;

} semi_socket_s;

typedef struct _socket_s
{
	// Source side of the socket
	// src describes the socket from the program's perspective
	semi_socket_s src;

	// Destination side of the socket
	// dest describes the socket from the perspective of the 
	//  other machine we are communicating with
	semi_socket_s dest; 

	// Socket File Descriptor
	int fd;

} socket_s;

void init_sock_struct()

void init_socket(socket_s* sckt, mch pos) {

    int sck_bind;

    sckt->fd = socket(AF_INET, SOCK_DGRAM, 0);

    sckt->src.addr_len = sizeof(sckt->src.addr);

    sckt->dest.addr = {0};
    sckt->dest.addr_len = sizeof(sckt->dest.addr);

    if (pos == server) {
        def_srv_addr_serverSide(&sckt->src.addr, sckt->src.port_no);
    }

    sck_bind = bind(sckt->fd, 
             (struct sockaddr_in*) &sckt->src.addr),
             sckt->src.addr_len);
    
    // Error handling
	if (sck_bind != 0) {
		perror("[server] bind() error:");
		exit(1);
	}
}

/* 
 * Function: def_srv_addr_clientSide
 * Alt Name: Define Server Address Client-Side
 * Description: Creates the server_addr data structure 
 *              to send packets for the client program
 * Parameters: 
 *  - server_addr: sockaddr_in POINTER argument
 *  - ip_addr: Destination IP address
 *  - port_no: Socket port number
 *
 * Returns: None
 */
void def_srv_addr_clientSide(
    struct sockaddr_in* server_addr, 
    const char* ip_addr, 
    const int port_no
    ) {
    // AF_INET: sets address scheme to IPv4
	server_addr->sin_family = AF_INET;

    // htons(): converts byte order of port number                        
	server_addr->sin_port = htons(port_no);

    // inet_addr(): converts string to IPv4 address                      
	server_addr->sin_addr.s_addr = inet_addr(ip_addr);

    // using memset() to set sin_zero field to zero
	memset(server_addr->sin_zero, 0, 8);
}

/*
 * Function: def_srv_addr_serverSide
 * Alt Name: Define Server Address Server-Side
 * Description: Creates the server_addr data structure 
 *              to send packets for the server program
 * Parameters: 
 *  - server_addr: sockaddr_in POINTER argument
 *  - port_no: Socket port number
 *
 * Returns: None
 * 
 * Note: This function doesn't need the IP address in
 *       contrast to the client-side function because the
 *       server is only listening for packets at this time
*/
void def_srv_addr_serverSide(
    struct sockaddr_in* server_addr,  
    const int port_no
    ) {
    // AF_INET: sets address scheme to IPv4
	server_addr->sin_family = AF_INET;

    // htons(): converts byte order of port number         
	server_addr->sin_port = htons(port_no); 

    // INADDR_ANY: binds to address of all 
	// 			   available network interfaces  
    // (it's listening from all potential addresses)
	server_addr->sin_addr.s_addr = INADDR_ANY; 

    // using memset() to set sin_zero field to zero
	memset(server_addr->sin_zero, 0, 8); 
}