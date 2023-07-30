/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Names: 
* Student IDs:
* GitHub Name:
* Group Name:
* Project: Basic File System
*
* File: b_io.c
*
* Description: Basic File System - Key File I/O Operations
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"
#include "fsInit.h"
#include "fsLow.h"

#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
	{
	/** TODO add al the information you need in the file control block **/
	char * buf;		//holds the open file buffer
	int index;		//holds the current position in the buffer
	int buflen;		//holds how many valid bytes are in the buffer
	int currentBlk;
	DirectoryEntry* fi;
	int numBlocks;
	} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].buf = NULL; //indicates a free fcbArray
		}
		
	startup = 1;
	}

//Method to get a free FCB element
b_io_fd b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].buf == NULL)
			{
			return i; //Not thread safe (But do not worry about it for this assignment)
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open (char * filename, int flags)
	{
	b_io_fd fd;
	
	if(startup ==0)
		b_init();
		

	fd=b_getFCB();
	fcbArray[fd].buf = malloc(B_CHUNK_SIZE);
	fcbArray[fd].index = 0;
	fcbArray[fd].buflen=0;
	fcbArray[fd].currentBlk=0;
	fcbArray[fd].numBlocks=(fcbArray[fd].fi->fileSize+ (B_CHUNK_SIZE -1))/B_CHUNK_SIZE;
	
	
				// get our own file descriptor
										// check for error - all used FCB's
	
	return (fd);						// all set
	}


// Interface to seek function	
int b_seek(b_io_fd fd, off_t offset, int whence)
{
    if (startup == 0)
        b_init(); // Initialize our system

    // check that fd is between 0 and (MAXFCBS-1)
    if ((fd < 0) || (fd >= MAXFCBS))
    {
        return -1; // Invalid file descriptor
    }

    switch (whence)
    {
    case SEEK_SET:
        if (offset < 0 || offset > fcbArray[fd].fi->fileSize)
        {
            // Offset goes beyond the file boundaries
            return -1;
        }
        fcbArray[fd].index = offset;
        break;

    case SEEK_CUR:
        if (fcbArray[fd].index + offset < 0 || fcbArray[fd].index + offset > fcbArray[fd].fi->fileSize)
        {
            // Offset goes beyond the file boundaries
            return -1;
        }
        fcbArray[fd].index += offset;
        break;

    case SEEK_END:
        if (offset > 0 || fcbArray[fd].fi->fileSize + offset < 0)
        {
            // Offset goes beyond the file boundaries
            return -1;
        }
        fcbArray[fd].index = fcbArray[fd].fi->fileSize + offset;
        break;

    default:
        // Invalid value for 'whence'
        return -1;
    }

    return fcbArray[fd].index;
}



// Interface to write a function	
// Interface to write data to the file
int b_write(b_io_fd fd, char* buffer, int count)
{
    if (startup == 0)
        b_init(); // Initialize our system

    // Check that fd is between 0 and (MAXFCBS-1)
    if ((fd < 0) || (fd >= MAXFCBS))
    {
        return -1; // Invalid file descriptor
    }

  
    int space_available = B_CHUNK_SIZE - fcbArray[fd].index % B_CHUNK_SIZE;
	// calc the remaining space in the cur block (disk block size)^^^
    // calc the number of bytes to write in the current block
    int bytes_to_write_in_block = (count < space_available) ? count: space_available;
    int block_number = fcbArray[fd].index / B_CHUNK_SIZE;

    //Write data to the disk directly 
    LBAwrite(buffer + fcbArray[fd].index, block_number, bytes_to_write_in_block);
    // Update the file pointer/index
    fcbArray[fd].index += bytes_to_write_in_block;

    // update #of bytes left to write
    count -= bytes_to_write_in_block;
    // write remaining into blocks loop
    while (count > 0)
    {
        // if more to write bytes> current remaining block bytes remaining
        if (count >= B_CHUNK_SIZE)
        {
            // calc the disk block number to write to
            block_number = fcbArray[fd].index / B_CHUNK_SIZE;
            // Write a full block to disk using 'LBAwrite'
            LBAwrite(buffer + fcbArray[fd].index, B_CHUNK_SIZE, block_number);
            // update file pointer/index 
            fcbArray[fd].index += B_CHUNK_SIZE;
            // updates #bytes left to write
            count -= B_CHUNK_SIZE;
        }
        else
        {
            // calc the disk block number to write to
            block_number = fcbArray[fd].index / B_CHUNK_SIZE;
            //write remaining bbytes to block
            LBAwrite(buffer + fcbArray[fd].index, count, block_number);
            // update the file pointer/index
            fcbArray[fd].index += count;
            
            // presumably all bytes written
            count = 0;
        }
    }

    // Return the total number of bytes successfully written to the file
    return fcbArray[fd].index;
}



// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill 
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
int b_read (b_io_fd fd, char * buffer, int count)
	{
	
	int part1;
	int part2;
	int part3;
	int numberOfBlocks;
	int bytesRead;

	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
	
	int remainingBytes = fcbArray[fd].buflen - fcbArray[fd].index;
	
	int amountDelivered = (fcbArray[fd].currentBlk * B_CHUNK_SIZE)- remainingBytes;
	
	if ((count + amountDelivered) > fcbArray[fd].fi->fileSize)
	{
		count = fcbArray[fd].fi-> fileSize - amountDelivered;
	}
	
	if (remainingBytes >=count){
		part1 = count;
		part2 = 0;
		part3 = 0;
	}
	
	else{
		part1 = remainingBytes;
		
		part3 = count - remainingBytes;
		
		numberOfBlocks = part3 / B_CHUNK_SIZE;
		
		part2 = numberOfBlocks * B_CHUNK_SIZE;
		
		part3 = part3 - part2;
	}
	
	if(part1>0){
		memcpy(buffer,fcbArray[fd].buf + fcbArray[fd].index, part1);
		fcbArray[fd].index = fcbArray[fd].index + part1;
	}
	
	if(part2 > 0){
		bytesRead = LBAread (buffer+ part1, numberOfBlocks, fcbArray[fd].currentBlk + fcbArray[fd].fi->fileLocation);
		fcbArray[fd].currentBlk += numberOfBlocks;
		
		part2 = bytesRead * B_CHUNK_SIZE;
	
	}
	if(part3 >0)
{
		bytesRead = LBAread(fcbArray[fd].buf , 1, fcbArray[fd].currentBlk + fcbArray[fd].fi->fileLocation);
		
	bytesRead = bytesRead * B_CHUNK_SIZE;
	
	fcbArray[fd].currentBlk +=1;
	
	fcbArray[fd].index = 0;
	
	fcbArray[fd].buflen = bytesRead;
	
	if(bytesRead < part3)
		part3 = bytesRead;
	
	if (part3>0){
		memcpy(buffer+part1+part2, fcbArray[fd].buf + fcbArray[fd].index, part3);
		fcbArray[fd].index = fcbArray[fd].index + part3;
	}
	return(part1 + part2 + part3);


}
	
	
	
	return (0);	//Change this
	}

	
// // Interface to Close the file	
// int b_close (b_io_fd fd)
// 	{
// 	if ((fd<0) || (fd >= MAXFCBS))
// 		return -1;
// 	if (fcbArray[fd].fi ==NULL)
// 		return -1;
// 	if (fcbArray[fd].buf != NULL)
// 		free (fcbArray[fd].buf);
// 	fcbArray[fd].buf = NULL;
// 	fcbArray[fd].fi = NULL;
// 	return 0;

// 	}


// Interface to Close the file	
int b_close(b_io_fd fd)
{
	if (startup == 0){
        b_init(); // Initialize our system
	}

    //Check that fd is between 0 and (MAXFCBS-1)
    if (fd < 0 || fd >= MAXFCBS)
    {
        return -1; // Invalid file descriptor
    }

    // Flush any remaining data in the buffer to the file
    if (fcbArray[fd].buflen > 0)
    {
        LBAwrite(fd, fcbArray[fd].buf, fcbArray[fd].buflen);
    }

	 // and check that the specified FCB is actually in use
    if (fcbArray[fd].buf == NULL) { // File not open for this descriptor
        return -1;
    }

    // Check if the file buffer is allocated
    if (fcbArray[fd].buf != NULL) {
        free(fcbArray[fd].buf); // Free the buffer if allocated
        fcbArray[fd].buf = NULL; // Set buffer to NULL to indicate deallocation
    }
    // Free the buffer associated with the FCB
    free(fcbArray[fd].buf);
    fcbArray[fd].buf = NULL;
    fcbArray[fd].index = 0;
    fcbArray[fd].buflen = 0;

    return 0; // File closed successfully
}




