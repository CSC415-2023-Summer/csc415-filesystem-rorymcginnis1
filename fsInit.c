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


int initialize_root_directory(int minEntreis, struct DirectoryEntry * parent)
{
	int bytesNeeded =(sizeof(DirectoryEntry)*NUM_DIRECT_ENTRIES);
	int numBlocks=(bytesNeeded +BLOCK_SIZE-1)/BLOCK_SIZE;
	
	int bytesToAllocate =numBlocks*BLOCK_SIZE;
	
	int actualEnteries = bytesToAllocate/(sizeof(DirectoryEntry));
	
	if (actualEnteries< minEntreis){
		return -1;
	}
	
	struct DirectoryEntry * newD =malloc(sizeof(DirectoryEntry)*actualEnteries);
	time_t currentTime = time(NULL);

	if(newD ==NULL)
	{
	
		return -1;
	}
		
	for (int i=0; i<actualEnteries; i++){
		strcpy(newD[i].fileName, "");
		newD[i].fileSize= 0;
		newD[i].fileLocation=-1;
		newD[i].dateCreated=currentTime;
		newD[i].dateAccessed=currentTime;
		newD[i].dateModified=currentTime;
		newD[i].isaDirectory=0;
	}

	strcpy(newD[0].fileName, ".");
	newD[0].fileSize= actualEnteries * (bytesToAllocate);
	extent * e = allocateBlocks(numBlocks, numBlocks);
	if(e ==NULL)
	{
		free(newD);
		return -1;
	}

	newD[0].fileLocation = e->start;
	newD[0].dateCreated=currentTime;
	newD[0].dateAccessed=currentTime;
	newD[0].dateModified=currentTime;
	newD[0].isaDirectory=1;
	free(e);
	strcpy(newD[1].fileName,"..");
	if(parent == NULL)
	{
		parent = newD;
	
	}
	else
	{
		newD[1].fileSize = parent[0].fileSize;
		newD[1].fileLocation = parent[0].fileLocation;
		newD[1].dateCreated=currentTime;
		newD[1].dateAccessed=currentTime;
		newD[1].dateModified=currentTime;
		newD[1].isaDirectory=1;	
	}          
	releaseBlocks(newD[1].fileLocation,numBlocks);
	LBAwrite(newD, numBlocks, newD[0].fileLocation);
	return (newD[0].fileLocation);

	}



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
	
	struct DirectoryEntry * root = malloc(sizeof(DirectoryEntry));
	
	
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

	printf("\n");

	return 0;

	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}
