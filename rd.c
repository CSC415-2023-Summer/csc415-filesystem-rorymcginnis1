#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "fsInit.h"
#include "fsLow.h"
#include "rd.h"




int initialize_root_directory(int minEntreis, DirectoryEntry * parent) {


    	int bytesNeeded =(sizeof(DirectoryEntry)*NUM_DIRECT_ENTRIES);
	int numBlocks=(bytesNeeded +BLOCK_SIZE-1)/BLOCK_SIZE;
	
	int bytesToAllocate =numBlocks*BLOCK_SIZE;
	
	int actualEnteries = bytesToAllocate/(sizeof(DirectoryEntry));
	
	if (actualEnteries< minEntreis){
		return -1;
		}
	
	DirectoryEntry * newD =malloc(sizeof(DirectoryEntry));
	time_t currentTime = time(NULL);

	if(newD ==NULL)
	
	printf("here =%i",actualEnteries);
		return -1;
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
	if(e ==NULL){
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
	if(parent == NULL){
		parent = newD;
	
	}
	else{

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


	
	
