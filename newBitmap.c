#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.h"
#include "fsLow.h"

unsigned char *freeSpaceMap = NULL;  // Global variable to hold the free space map
int freeSpaceStartBlock = -1;       // Global variable to store the starting block of
                                    // the free space, it assign to -1 because the free space
                                    // is not yet know or set

// Initialize the free space map
int initFreeSpace(int blockCount, int bytesPerBlock) {
    int requiredBytes = (blockCount / 8) + ((blockCount % 8) ? 1 : 0);  // Calculate the number of bytes required to represent the free space map
    int requiredBlocks = (requiredBytes / bytesPerBlock) + ((requiredBytes % bytesPerBlock) ? 1 : 0);  // Calculate the number of blocks required to store the free space map

    freeSpaceMap = (unsigned char*)malloc(requiredBlocks * bytesPerBlock);  // Allocate memory for the free space map
    if (freeSpaceMap == NULL) {
        printf("Memory allocation failed for free space map.\n");
        return -1;
    }

    // Mark the first 6 bits as used (VCB and free space map)
    for (int i = 0; i < 6; i++) {
        freeSpaceMap[i / 8] |= (1 << (i % 8));  // Set the corresponding bits to 1 to mark them as used
    }

    // Write the free space map to disk
    LBAwrite(freeSpaceMap, requiredBlocks, 1);

    freeSpaceStartBlock = 1;  // Assuming the free space map starts from block 1
    return freeSpaceStartBlock;
}

// Load the existing free space map from disk
int loadFreeSpace(int blockCount, int bytesPerBlock) {
    int requiredBytes = (blockCount / 8) + ((blockCount % 8) ? 1 : 0);  // Calculate the number of bytes required to represent the free space map
    int requiredBlocks = (requiredBytes / bytesPerBlock) + ((requiredBytes % bytesPerBlock) ? 1 : 0);  // Calculate the number of blocks required to store the free space map

    freeSpaceMap = (unsigned char*)malloc(requiredBlocks * bytesPerBlock);  // Allocate memory for the free space map
    if (freeSpaceMap == NULL) {
        printf("Memory allocation failed for free space map.\n");
        return -1;
    }

    // Read the free space map from disk
    LBAread(freeSpaceMap, requiredBlocks, 1);

    freeSpaceStartBlock = 1;  // Assuming the free space map starts from block 1
    return freeSpaceStartBlock;
}

// Allocate disk blocks
extent *allocateBlocks(int required, int minPerExtent, int blockCount, int requiredBlocks) {
    extent *result = (extent*)malloc(sizeof(extent) * (required / minPerExtent + 2));  // Allocate memory for allocation extents
    if (result == NULL) {
        printf("Memory allocation failed for allocation extents.\n");
        return NULL;
    }

    int blocksFound = 0;
    int extentCount = 0;
    int extentStart = -1;

    for (int i = freeSpaceStartBlock; i < blockCount; i++) {
        int byteIndex = i / 8;
        int bitIndex = i % 8;

        if ((freeSpaceMap[byteIndex] & (1 << bitIndex)) == 0) {
            // Block is free, mark it as used
            freeSpaceMap[byteIndex] |= (1 << bitIndex);

            if (extentStart == -1) {
                // Start of a new extent
                extentStart = i;
            }

            blocksFound++;

            if (blocksFound >= required) {
                // Reached the required number of blocks
                result[extentCount].start = extentStart;
                result[extentCount].count = blocksFound;
                extentCount++;

                // Add a terminating extent with start and count as 0
                result[extentCount].start = 0;
                result[extentCount].count = 0;

                // Write the updated free space map to disk
                LBAwrite(freeSpaceMap, requiredBlocks, 1);

                return result;
            }
        } else {
            // Block is used, reset the extent start
            extentStart = -1;
            blocksFound = 0;
        }
    }
}

// Release disk blocks
void releaseBlocks(int start, int count, int requiredBlocks) {
    for (int i = start; i < start + count; i++) {
        int byteIndex = i / 8;
        int bitIndex = i % 8;

        // Mark the block as free
        freeSpaceMap[byteIndex] &= ~(1 << bitIndex);
    }

    // Write the updated free space map to disk
    LBAwrite(freeSpaceMap, requiredBlocks, 1);
}
