/**************************************************************
* Class:  CSC-415-1 Summer 2023
* Names: Kaung Nay Htet, Himal Shrestha, Rory McGinnis,  James Donnelly
* Student IDs:922292784, 922399514, 921337245, 917703805
* GitHub Name: rorymcginnis1
* Group Name: Team Drivers
* Project: Basic File System
*
* File: mfs.c
*
* Description: The file where we define fs_setcwd , fs_getcwd
* fs_isFile , fs_isDir ,fs_mkdir ,fs_opendir , fs_readdir
* fs_closedir , fs_stat
* And optimally complete
* fs_delete, fs_rmdir
*
*
**************************************************************/
#include <stdio.h>
#include <string.h>
#include "mfs.h"
#include "b_io.h"
#include "fsInit.h"
#include <time.h>
extern struct DirectoryEntry *globalDirEntries;
int fs_stat( const char *path, struct fs_stat *buf)
{

	for (int i=0; i< NUM_DIRECT_ENTRIES; i++){
		if(strcmp(path, globalDirEntries[i].fileName)==0){
			buf ->st_size = globalDirEntries[i].fileSize;
			buf-> st_blksize = BLOCK_SIZE;
			buf-> st_blocks = (globalDirEntries[i].fileSize + BLOCK_SIZE -1)/BLOCK_SIZE;
			buf-> st_accesstime = globalDirEntries[i].dateAccessed;
			buf->st_modtime =globalDirEntries[i].dateModified;
			buf->st_createtime = globalDirEntries[i].dateCreated;
			
			return 0;
		
		}
		return -1;
	
	}

}

// int fs_delete(char* filename)
// {
// 	if(fs_isFile (filename)){
// 		for (int i=0; i<NUM_DIRECT_ENTRIES;i++){
// 			if(strcmp(filename, newD[i].fileName)==0){
// 				releaseBlocks(newD[i].fileLocation, (newD[i].fileSize + BLOCK_SIZE - 1) / BLOCK_SIZE);
// 				strcpy(newD[i].fileName, "");
// 				newD[i].fileSize= 0;
// 				newD[i].fileLocation=-1;
// 				newD[i].dateCreated=NULL;
// 				newD[i].dateAccessed=NULL;
// 				newD[i].dateModified=NULL;
// 				newD[i].isaDirectory=0;
// 				return 0;
// 	}
	
	
// 			}
// 		}
// 		return -1;
	
	
// 	}





// int fs_rmdir(const char *pathname){
// 	if(fs_isDir){
// 		if(fs_d_is_empty(*pathname)){
		
			
// 		}
// 		else{
// 		return -1;
// 		}
	
// 	}
// 	else{return -1;}



// }


// int fs_isFile(const char *filename) {
//     for (int i=0; i < NUM_DIRECT_ENTRIES; i++)
//         if (strcmp(filename, newD[i].fileName) == 0){
//             return !newD[i].isaDirectory;
//         }
//         return 0; 
//         // file not found hence directory
// }         

// int fs_isDir(const char *pathname) {
//      for (int i = 0; i < NUM_DIRECT_ENTRIES; i++) {
//         if (strcmp(pathname, newD[i].fileName) == 0) {
//             return newD[i].isaDirectory;
//         }
//     }
//         return 0;   
//         //directory not found hence file
//     }
//     //see if dir 


int fs_mkdir(const char *pathname, mode_t mode) {
    // iterate over directory
    for (int i = 0; i < NUM_DIRECT_ENTRIES; i++) {
        // filename matches pathname?
        if (strcmp(globalDirEntries[i].fileName, pathname) == 0) {
            return -1; // Return -1 as the directory already exists
        }
    }

    // find available spot for directory entry
    int dirIndex = -1; // Index of the available directory entry slot
    for (int i = 0; i < NUM_DIRECT_ENTRIES; i++) {
        // fileName of the current directoryentry  is an empty string,
        // slot available
        if (globalDirEntries[i].fileName[0] == '\0') { 
            dirIndex = i; // slot, store its index
            break; // Break
        }
    }

    // no available slot
    if (dirIndex == -1) {
         printf("Failed to create directory '%s': No available slot.\n", pathname);
        return -1; // Return -1 indicating no space for the new directory
    }

    // Initialize the new directory entry
    strncpy(globalDirEntries[dirIndex].fileName, pathname, MAX_NAME_LENGTH); // Copy the pathname into the fileName
    globalDirEntries[dirIndex].fileSize = 0; // Initial size of directory is 0
    globalDirEntries[dirIndex].fileLocation = -1; // Set fileLocation to an invalid value (-1) indicating the directory is initially empty
    globalDirEntries[dirIndex].isaDirectory = 1; // Flag to indicate that this entry is a directory

    // Get the current time
    time_t currentTime = time(NULL);
    // Set dateCreated, dateAccessed, and dateModified to current time
    globalDirEntries[dirIndex].dateCreated = currentTime;
    globalDirEntries[dirIndex].dateAccessed = currentTime;
    globalDirEntries[dirIndex].dateModified = currentTime;
printf("Directory '%s' created successfully.\n", pathname);
    return 0; // Return 0 indicating that the directory was created successfully
}




