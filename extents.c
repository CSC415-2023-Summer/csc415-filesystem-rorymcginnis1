#include "extents.h"
#include "b_io.h"
#include <stdlib.h>
#include <stdio.h>
#include "fsLow.h"
#include "fsInit.h" 
 
//need to make so can call vcb as seen in initfreespace


// Global variables to store the state of the freespace map
int blockCount = 19531;     //total blocks
int bytesPerBlock = MINBLOCKSIZE;  //size of each block || bites not bits
extent* extents = NULL; //array of extents
int extentsCount = 1;//counter for extents

// Function to initialize the freespace map
int initFreeSpace() {
    
   //i put 320 cause thats what i figiured could fit into 5 blocks
    extents = (extent*)calloc(320,sizeof(extent));
    //freee initially, full block ^^^^^^^^^
    extents[0].start = 6;  // Starting from block 6 as per requirements
    extents[0].count = blockCount -6;
    return 6; // Return the block number where the freespace map starts

    //  if (vcb != NULL) {
    //     vcb->startBlock = 6;
    // }   this is for 3. change as needed
}

// Function to load the existing freespace map
int loadFreeSpace() {
    // if (totalBlockCount <= 0 || blockSize <= 0) {
    //     return -1; // Invalid input parameters
    // }
    
    // blockCount = totalBlockCount;
    // bytesPerBlock = blockSize;
    // extents = (extent*)malloc(sizeof(extent));
    // extents[0].start = 0;
    // extents[0].count = blockCount;
    
    // return 0;
    return initFreeSpace ();
    //call for freeespace loadup


    //^^^^^loads map by calling init with totalblocks and block size^^^^
}


void serializeFreeSpaceMap() {
    char* theFreeSpaceMap = (char*) malloc(bytesPerBlock * 5);
    //^^^^allocate memory for space mpa
    if (!theFreeSpaceMap) {
        printf("Memory allocation for theFreeSpaceMap failed.\n");
        return;
    }
    //loop for all extents to cpy into freespaace
    for (int i = 0; i < extentsCount; ++i) {
        char* source = (char*)&(extents[i]);
        for (size_t j = 0; j < sizeof(extent); ++j) {
            theFreeSpaceMap[i * sizeof(extent) + j] = source[j];
        }
    }

    int writeResult = 0;//LBAwrite(theFreeSpaceMap, 5, 1);
    //write to disk
    if (writeResult == -1) {
        printf("Writing theFreeSpaceMap to disk failed.\n");
    }

    free(theFreeSpaceMap);
    //freer the map
}



// Function to allocate disk blocks
extent* allocateBlocks(int required, int minPerExtent) {
    if (required <= 0 || minPerExtent <= 0) {
        //chk fr rquest blocks
        return NULL; // Invalid input parameters
    }
    if(extents[0].start <= 5){
        return NULL; //reserved blcoks
    }
    
    extent* result = (extent*)calloc(required, sizeof(extent));
    //arroary allocation
    int i = 0;  //iterate thru extents
    int extentCount = 0;// number of extents currently
    int alocd = 0; //alloocated=alocd
    // amount of blocks currently alocated

    //extents loop looking for availble extent to use
    while (alocd < required && i < 5) {
        // if extent has enough blocks
        if (extents[i].count >= minPerExtent) {
            //calc #of blocks for allocation from extent
            int allocate = extents[i].count < required - alocd ? extents[i].count : required - alocd;
            result[extentCount].start = extents[i].start;
            result[extentCount].count = allocate;
            //^^^^^^start and count set
            extents[i].start += allocate;
            extents[i].count -= allocate;
            ////^6 update start and count with allocation
            alocd += allocate;//# of blck update
            extentCount++;
            if (extents[i].count == 0) {
                i++;
            }
        } else {
            i++;
        }
    }
    
    if (alocd < required) {
        free(result);
        return NULL; // no extents available
    }

    return result;
    ///return etents arry

    // while (i < extentCount || extents[i].count >= required) {
    //     if (extents[i].count >= required) {
    //         result[extentCount].start = extents[i].start;
    //         result[extentCount].count = required;
    //         extents[i].start += required;
    //         extents[i].count -= required;
    //         if (extents[i].count == 0) {
    //             extentCount++;
    //         }
    //         return result;
    //     }
    //     i++;
    // }

    // free(result);
    // return NULL; // No suitable extents found

}

// Function to release blocks in the freespace system
void releaseBlocks(int start, int count) {
    //start postioon & block countvvvvvvv
    if (start < 0 || count <= 0 || start <=5) {
        return; // Invalid input parameters// trying to go into reeserved blcks
    }
    
    int i = 0; //iterate
    int extentIndex = -1;  //index of extent wher to release blks
    

    // Find the extent where the blocks should be released

    for (i = 0; i < blockCount; i++) {
            // if  current start = start pos & start pos is in range of extent
        if (start == extents[i].start && start < extents[i].start + extents[i].count) {
            extentIndex = i;
            break;
        }
    }

    if (extentIndex < 0) {
        return; // No extent found to release blocks
    }

    // Create a new extent with released blocks
    extents = (extent*)realloc(extents, (blockCount + 1) * sizeof(extent));
    for (i = blockCount; i > extentIndex; i--) {
        extents[i] = extents[i - 1];
    }

    //new extent init
    extents[extentIndex + 1].start = start;
    extents[extentIndex + 1].count = count;
    extents[extentIndex].count = start - extents[extentIndex].start;
    blockCount++;





    // while (start >= extents[i].count) {
    //     start -= extents[i].count;
    //     extentIndex++;
    //     i++;
    // }

    // // Insert a new extent if the released blocks are not at the beginning of an existing extent
    // if (start > 0) {
    //     extentIndex++;
    //     extents = (extent*)realloc(extents, (blockCount + 1) * sizeof(extent));
    //     for (i = extentIndex; i < blockCount; i++) {
    //         extents[i + 1] = extents[i];
    //     }
    //     extents[extentIndex - 1].count = start;
    // }

    // // Adjust the extent count and start position
    // extents[extentIndex].start += count;
    // extents[extentIndex].count -= count;
    // if (extents[extentIndex].count == 0) {
    //     extentIndex++;
    // }

    // // Remove any empty extents after releasing blocks
    // for (i = extentIndex; i < blockCount; i++) {
    //     extents[i] = extents[i + 1];
    // }
    
    // blockCount--;
}





/*
// Example usage
int main() {
    // Sample usage of the functions
    int totalBlockCount = 19531;
    int blockSize = MINBLOCKSIZE;
    int i;

    int initResult = initFreeSpace(totalBlockCount, blockSize);
    if (initResult == -1) {
        printf("Invalid input parameters for initializing freespace map.\n");
        return -1;
    }
    
    extent* allocatedExtents = allocateBlocks(5, 5);
    
    if (allocatedExtents != NULL) {
        // Blocks allocated successfully
        printf("Allocated blocks:\n");
        for (i = 0; allocatedExtents[i].count != 0; i++) {
            printf("Start: %d, Count: %d\n", allocatedExtents[i].start, allocatedExtents[i].count);
        }
        
        // Release the allocated blocks
        for (i = 0; allocatedExtents[i].count != 0; i++) {
            releaseBlocks(allocatedExtents[i].start, allocatedExtents[i].count);
        }
        
        // Free the allocated extents array
        free(allocatedExtents);
    } else {
        printf("Failed to allocate blocks.\n");
    }
    
    // Free the extents array in the freespace map
    free(extents);
    
    return 0;
}*/

