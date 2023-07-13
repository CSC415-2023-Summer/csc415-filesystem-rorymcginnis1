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

#define MAX_NAME_LENGTH 64

struct VolumeControlBlock 
	{	
    char volumeName[256];
	int totalBlockSize;      
	int blockSize;   
    int startBlock;    
    int signature;
    int numberOfBlocks;
	int numberOfFreeBlocks;
	// Created time??
	
	} VolumeControlBlock;

//To hold our necessary data for a file, such as current date and name location size
struct DirectoryEntry 
	{ 
	char fileName[MAX_NAME_LENGTH];
	int fileLocation;
	int fileSize;
	time_t dateCreated;
	time_t dateAccessed;
	time_t dateModified;
	int isaDirectory; // To decide if it's a folder or file?
	} DirectoryEntry;

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