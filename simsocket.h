#include <stdlib.h>
#include <string.h>

// Libraries needed for socket I/O
//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>



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