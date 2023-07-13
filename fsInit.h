#ifndef FSINIT_H
#define FSINIT_H

#define NUM_DIRECT_ENTRIES 50
#define MAX_NAME_LENGTH 255
#define BLOCK_SIZE 512
#define MAGICNUMBER 0x1A3B5C  //signature number
#define NUM_DIRECT_ENTRIES 50


typedef struct VolumeControlBlock 
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

typedef struct DirectoryEntry 
	{ 
	char fileName[MAX_NAME_LENGTH];
	int fileLocation;
	int fileSize;
	time_t dateCreated;
	time_t dateAccessed;
	time_t dateModified;
	int isaDirectory; // To decide if it's a folder or file?
	} DirectoryEntry;


#endif
