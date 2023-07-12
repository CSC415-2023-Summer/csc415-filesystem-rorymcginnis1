#include "bitmap.h"
#include <stdlib.h>
#include <stdio.h>


// Global variables to store the state of the freespace map
int blockCount = 0;
int bytesPerBlock = 0;
extent* extents = NULL;

// Function to initialize the freespace map
int initFreeSpace(int totalBlockCount, int blockSize) {
    if (totalBlockCount <= 0 || blockSize <= 0) {
        return -1; // Invalid input parameters
    }
    
    blockCount = totalBlockCount;
    bytesPerBlock = blockSize;
    extents = (extent*)malloc(sizeof(extent));
    extents[0].start = 0;
    extents[0].count = blockCount;
    
    return 0; // Return the block number where the freespace map starts
}

// Function to load the existing freespace map
int loadFreeSpace(int totalBlockCount, int blockSize) {
    if (totalBlockCount <= 0 || blockSize <= 0) {
        return -1; // Invalid input parameters
    }
    
    blockCount = totalBlockCount;
    bytesPerBlock = blockSize;
    extents = (extent*)malloc(sizeof(extent));
    extents[0].start = 0;
    extents[0].count = blockCount;
    
    return 0;
}

// Function to allocate disk blocks
extent* allocateBlocks(int required, int minPerExtent) {
    if (required <= 0 || minPerExtent <= 0) {
        return NULL; // Invalid input parameters
    }
    
    extent* result = (extent*)malloc(sizeof(extent));
    int i = 0;
    int extentCount = 0;

    while (i < extentCount || extents[i].count >= required) {
        if (extents[i].count >= required) {
            result[extentCount].start = extents[i].start;
            result[extentCount].count = required;
            extents[i].start += required;
            extents[i].count -= required;
            if (extents[i].count == 0) {
                extentCount++;
            }
            return result;
        }
        i++;
    }

    free(result);
    return NULL; // No suitable extents found
}

// Function to release blocks in the freespace system
void releaseBlocks(int start, int count) {
    if (start < 0 || count <= 0) {
        return; // Invalid input parameters
    }
    
    int i = 0;
    int extentIndex = 0;

    // Find the extent where the blocks should be released
    while (start >= extents[i].count) {
        start -= extents[i].count;
        extentIndex++;
        i++;
    }

    // Insert a new extent if the released blocks are not at the beginning of an existing extent
    if (start > 0) {
        extentIndex++;
        extents = (extent*)realloc(extents, (blockCount + 1) * sizeof(extent));
        for (i = extentIndex; i < blockCount; i++) {
            extents[i + 1] = extents[i];
        }
        extents[extentIndex - 1].count = start;
    }

    // Adjust the extent count and start position
    extents[extentIndex].start += count;
    extents[extentIndex].count -= count;
    if (extents[extentIndex].count == 0) {
        extentIndex++;
    }

    // Remove any empty extents after releasing blocks
    for (i = extentIndex; i < blockCount; i++) {
        extents[i] = extents[i + 1];
    }
    
    blockCount--;
}

// Example usage
int main() {
    // Sample usage of the functions
    int totalBlockCount = 100;
    int blockSize = 4096;
    int i;

    int initResult = initFreeSpace(totalBlockCount, blockSize);
    if (initResult == -1) {
        printf("Invalid input parameters for initializing freespace map.\n");
        return -1;
    }
    
    extent* allocatedExtents = allocateBlocks(10, 10);
    
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
}
