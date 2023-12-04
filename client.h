// Includes shared between the
//  client and server
#include "simhead.h"

// File path for the input file
char *filePath = "";

#include <unistd.h>

// #define DEBUG_PACKING

// #define DEBUG_PACKING2

// Size of a single byte
// Used for determining the number of characters
//  to read from the input file at once
#define BYTE_SIZE sizeof(uint8_t)

unsigned int packet_size = sizeof(packet_s);

/*
 * Function: get_file_size
 * Alt Name: Get File Size
 * Description: Returns the size of a file pointer's file
 *
 * Parameters:
 *  - fp: The file pointer for the target file
 *
 * Returns: The size of the file in bytes
 */
unsigned int get_file_size(FILE *fp)
{
    int fsize = 0;
    fseek(fp, 0L, SEEK_END);
    fsize = ftell(fp);
    rewind(fp);
    return fsize;
}

/*
 * Function: pack_packets
 * Alt Name: Pack Packets
 * Description: Copies the contents of a file at a given directory
 *              and stores the data in an array of packets
 * Parameters:
 *  - filePath: A string of the file system path from the
 *              working directory to the target file
 *  - pkt_ptr: The memory address of the pointer to the array of packets
 *
 * Returns: The number of packets in the array
 */
unsigned int pack_packets(
    const char *filePath, packet_s **pkt_ptr)
{

    // file pointer
    FILE *fptr;

    unsigned int file_size = 0, // Total size of input file
        num_packets = 0,        // Number of packets in data array
        itr = 0,                // Iterator for a packet's payload
        pkt_idx = 0;            // Iterator for the packet array

    // Open the target file
    fptr = fopen(filePath, "r");
    if (fptr == NULL) {
        printf("ERROR: Could not open file. \n");
        return 0;
    }

    // Determine the total size of the file
    file_size = get_file_size(fptr);

    // Calculate the number of packets needed
    //  to transmit the file's contents
    num_packets = file_size / PAYLOAD_SIZE + 
                (file_size % PAYLOAD_SIZE != 0);

    // Create the array of packets large enough to hold
    //  all the data in the input file
    *pkt_ptr = (packet_s *)malloc(packet_size * num_packets);

    // Temporary variable to make accessing packets easier
    packet_s *pkt_arr = *pkt_ptr;

    #ifdef DEBUG_PACKING
        printf("File size: %u \t Expected packets: %u \n",
            file_size, num_packets);
    #endif

    // Beginning with the first packet, pack the data from the file
    //  into the payload until it is completely full and then
    //  continue where it left off in the next packet
    for (pkt_idx = 0; pkt_idx < (num_packets - 1); pkt_idx++) {
        itr = 0;

        #ifdef DEBUG_PACKING
            printf("\nIndex: ");
        #endif

        // Read the next payload worth of bytes and then store
        //  that data into the payload of the current packet
        fread(&pkt_arr[pkt_idx].payload, BYTE_SIZE, PAYLOAD_SIZE, fptr);

        // Set the index of the current packet to the
        //  index of the array iterator
        // This should ensure the packets
        //  are reassembled in the right order
        pkt_arr[pkt_idx].index = pkt_idx;

        // Set the length of the current payload
        //  to default to the max possible size
        pkt_arr[pkt_idx].payload_len = PAYLOAD_SIZE;

        // Set tot_pkts to num_packets
        //  indicating how many packets in total there are
        pkt_arr[pkt_idx].tot_pkts = num_packets;

        #ifdef DEBUG_PACKING
            printf("%u Done. \n", itr);
        #endif
    }

    itr = 0;

    #ifdef DEBUG_PACKING2
        printf("Packing last packet... ");
    #endif

    // For the last packet, iterate over each byte
    //  of the last few bytes of the file
    while (fread(&pkt_arr[pkt_idx].payload[itr], BYTE_SIZE, 1, fptr)) {
        itr++;

        #ifdef DEBUG_PACKING2
            if (itr % 32 == 0) {
                printf("%u ", itr);
            }
        #endif
    }

    #ifdef DEBUG_PACKING2
        printf("%u Done. \n", itr);
    #endif

    // Make sure to set the index of the last packet
    pkt_arr[pkt_idx].index = pkt_idx;

    // Set the length of the last packet to
    //  the remainder of the max payload size
    // This should be the only packet whose
    //  payload length isn't at max
    pkt_arr[pkt_idx].payload_len = file_size % PAYLOAD_SIZE;

    // Set tot_pkts to num_packets
    //  indicating how many packets are coming in total
    pkt_arr[pkt_idx].tot_pkts = num_packets;

    // Return the total number of packets in the array
    return num_packets;
}