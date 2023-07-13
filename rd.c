#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "bitmap.c"
#include "fsLow.h"

#define NUM_DIRECT_ENTRIES 50
#define MAX_NAME_LENGTH 255
#define BLOCK_SIZE 512

typedef struct DirectoryEntry{
char name [MAX_NAME_LENGTH];

unsigned long size;

unsigned long location;

time_t creationTime;

time_t modifiedTime;

time_t accessedTime;

int isADirectory;

} Direct;


int initialize_root_directory(int minEntreis, Direct * parent) {


    	int bytesNeeded =(sizeof(Direct)*NUM_DIRECT_ENTRIES);
	int numBlocks=(bytesNeeded +BLOCK_SIZE-1)/BLOCK_SIZE;
	
	int bytesToAllocate =numBlocks*BLOCK_SIZE;
	
	int actualEnteries = bytesToAllocate/(sizeof(Direct));
	
	Direct * newD =malloc(sizeof(Direct));
	time_t currentTime = time(NULL);

	if(newD ==NULL)
	
	printf("here =%i",actualEnteries);
		return -1;
	for (int i=0; i<actualEnteries; i++){
		strcpy(newD[i].name, "");
		newD[i].size= 0;
		newD[i].location=-1;
		newD[i].creationTime=currentTime;
		newD[i].modifiedTime=currentTime;
		newD[i].accessedTime=currentTime;
		newD[i].isADirectory=0;
	
	}
	
	strcpy(newD[0].name, ".");
	newD[0].size= actualEnteries * (bytesToAllocate);
	extent * e = allocateBlocks(numBlocks, numBlocks);
	if(e ==NULL){
		free(newD);
		return -1;
	}
	newD[0].location = e->start;
	newD[0].creationTime=currentTime;
	newD[0].modifiedTime=currentTime;
	newD[0].accessedTime=currentTime;
	newD[0].isADirectory=1;
	free(e);
	strcpy(newD[1].name,"..");
	if(parent == NULL){
		parent = newD;
	
	}
	else{

		newD[1].size = parent[0].size;
		newD[1].location = parent[0].location;
		newD[1].modifiedTime=currentTime;
		newD[1].accessedTime=currentTime;
		newD[1].isADirectory=1;
		
	}          
	releaseBlocks(newD[1].location,numBlocks);
	LBAwrite(newD, numBlocks, newD[0].location);
	return (newD[0].location);
	}


	
	
