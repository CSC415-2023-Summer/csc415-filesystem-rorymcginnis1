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

#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
	{
	/** TODO add al the information you need in the file control block **/
	char * buf;		//holds the open file buffer
	int index;		//holds the current position in the buffer
	int buflen;		//holds how many valid bytes are in the buffer
	DirectoryEntry* fi;
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
			return i;		//Not thread safe (But do not worry about it for this assignment)
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open (char * filename, int flags)
	{
	b_io_fd returnFd;

	//*** TODO ***:  Modify to save or set any information needed
	//
	//
		
	if (startup == 0) b_init();  //Initialize our system
	
	returnFd = b_getFCB();				// get our own file descriptor
										// check for error - all used FCB's
	
	return (returnFd);						// all set
	}


// Interface to seek function	
int b_seek (b_io_fd fd, off_t offset, int whence)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
		
	return (0); //Change this
	}



// Interface to write function	
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
    // calc the number of bytes to write in the current blockvvvv
    int bytes_to_write_in_block = (count < space_available) ? count : space_available;
    // calc the disk block number to write tovvv
    int block_number = fcbArray[fd].index / B_CHUNK_SIZE;

    // write data to the disk directly 
    LBAwrite(fd, buffer, block_number, bytes_to_write_in_block);
    // Update the file pointer/index
    fcbArray[fd].index += bytes_to_write_in_block;

    // update #of bytes left to wrtie
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
            LBAwrite(fd, buffer + fcbArray[fd].index, block_number, B_CHUNK_SIZE);
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
            LBAwrite(fd, buffer + fcbArray[fd].index, block_number, count);
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

	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	return (0);	//Change this
	}
	
// Interface to Close the file	
int b_close (b_io_fd fd)
	{

	}
