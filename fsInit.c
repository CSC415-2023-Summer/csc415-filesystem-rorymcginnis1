/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Names: 
* Student IDs:
* GitHub Name:
* Group Name:
* Project: Basic File System
*
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"
#include "extents.c"
#include "fsInit.h"
#include "rd.c"
 //signature number


int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{

	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */

	struct VolumeControlBlock * vcbPointer;
	vcbPointer = malloc(MINBLOCKSIZE);
	//catch case
	if (vcbPointer == NULL ){
		printf("NULL");
		return -1;
	}

	int test = LBAread(vcbPointer,1,0);
	int fs = initFreeSpace();
	if(fs != 6)
	{
		return -1;
	}
	
	int minEnt = 2;
	
	DirectoryEntry * root = malloc(sizeof(DirectoryEntry));
	
	
	int rd = initialize_root_directory(minEnt, root);
	
	//rd returns the root directory
	

	if(rd < 0)
	{
		return -1;
	}


	if (vcbPointer->signature != MAGICNUMBER)
	{
		vcbPointer->signature = MAGICNUMBER;
		vcbPointer->totalBlockSize = numberOfBlocks;
		vcbPointer->blockSize = blockSize;
		vcbPointer->startBlock = rd;
		LBAwrite (vcbPointer, 1, 0);
	}


	// else 
	// {
	// 	printf("Success")
	// }

	printf("\n");


	return 0;
	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}
