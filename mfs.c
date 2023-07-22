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



// Parsepath code begins ---->

// Current temporary use value, need change, or set, later
char cwd[100] = "./../";


char tmpcwd[100]; // For testing use
char tokenArray[20][100]; // 20 is temporary count of string tokens, 100 characters in each
int arrayCount = 0; // Accompanying value to count the tokenArray

/*
* parsePath() 
* Tokenizes cwd, and uses / as a delimeter to cut out each name
* Iterates through token while it is not NULL
* Stores each token into our token array
* Then sets our tmpcwd to the tokenArray
* Parameter parentParse determines if we are doing a parse to look
* for the parent vs parsing for our change directory function. They
* are called with 1 or 0
*/


// If we would like to add the "text input" for the parse path string, we can add additional
// parameter, and work from that. The additional parameter would change the global value of 
// cwd or tempcwd
// Parameter is the "mode"

int parsePath(int parentParse){ //return int instead void, return i value thats index array
    //printf("\nCWDTEST2 : %s\n", cwd);    
    char * token = strtok(cwd, "/");
    //printf("\nCWDTEST3 : %s\n", cwd); 
    strcpy(tmpcwd, "\0");

    // Iterate through the token   
    while( token != NULL ) 
    {
      //printf( "\nTOKEN: %s", token ); //printing each token
      arrayCount++;
      strcpy(tokenArray[arrayCount],token);
      //printf( "\nTOKEN stored: %s", tokenArray[arrayCount]); //print
      // resetCWD();

      token = strtok(NULL, "/");
    } 

   

    if (parentParse == 1)
    {
        // Return the indeex for parent of parse-pathed-value
        // -1 the tail entry

        for(int i = 1; i < arrayCount+1; i++) 
        {
            strcpy(cwd + strlen(cwd),  tokenArray[i]);
            strcpy(cwd + strlen(cwd),  "/");
        }
        //printf("\nTEMPCWDTEST4 : %s\n", tmpcwd); 
        
        for (int i = 1; i < 52; i++)
        {
            //printf("\nSEARCHES: %s", directoryEntry[i].fileName);
            if (strcmp(globalDirEntries[i].fileName , tokenArray[arrayCount]) == 0)
            { //either arraycount or arraycount - 1
            //printf("Parent is : %s\n", tokenArray[arrayCount]);
            // printf("CWD : %s\n", cwd);
            return i;
            }
        }
    }

    if (parentParse == 0)
    {
        // Return the index for parse-pathed-value
        // -1 the tail entry

        for(int i = 1; i < arrayCount; i++)
        {
        strcpy(tmpcwd + strlen(tmpcwd),  tokenArray[i]); 
        strcpy(tmpcwd + strlen(tmpcwd),  "/");
        }
        return 0;
    }
    

}

// Helper function to reset cwd 
void resetCWD() { //comment code and uncomment for debugging
    //printf("\nBEFORE:\nTMPCWD: %s\nCWD%s", tmpcwd, cwd); //debugging
    strcpy(tmpcwd,  "\0");
    strcpy(cwd,  "\0");
    //printf("\nAfter:\nTMPCWD: %s\nCWD%s", tmpcwd, cwd); //debugging
}

// <------ Parsepath code ends

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


int fs_isFile(char *filename) {
    for (int i=0; i < NUM_DIRECT_ENTRIES; i++)
        if (strcmp(filename, globalDirEntries[i].fileName) == 0){
            return !globalDirEntries[i].isaDirectory;
        }
        return 0; 
        // file not found hence directory
}         

int fs_isDir(char *pathname) {
     for (int i = 0; i < NUM_DIRECT_ENTRIES; i++) {
        if (strcmp(pathname, globalDirEntries[i].fileName) == 0) {
            return globalDirEntries[i].isaDirectory;
        }
    }
        return 0;   
        //directory not found hence file
    }
    //see if dir 


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


char * fs_getcwd(char *pathname, size_t size) {
    // Check if the provided buffer is valid
    if (pathname == NULL || size == 0) {
        printf("Invalid buffer.\n");
        return NULL;
    }

    // Get the current working directory from your file system (e.g., keep track of it as you traverse the file system)
    // For this implementation, let's assume you have a variable called "currentWorkingDirectory" that holds the current working directory as a string.

    // Copy the current working directory to the provided buffer
    // Make sure to handle cases where the buffer size is insufficient
    if (strlen(cwd) >= size) {
        printf("Buffer size too small to hold the current working directory.\n");
        return NULL;
    }

    strncpy(pathname, cwd, size);
    // pathname[size - 1] = '\0'; // Ensure the buffer is null-terminated

    return pathname;
}

int fs_setcwd(char *pathname) {
    // If the provided pathname is ".", no need to change the current working directory.
    if (strcmp(pathname, ".") == 0) {
        return 0; // Successfully set the current working directory (no change).
    }

    // If the provided pathname is "..", move up one level in the directory hierarchy.
    if (strcmp(pathname, "..") == 0) {
        // Find the last occurrence of '/' in the current "cwd" and remove it to go up one level.
        char *lastSlash = strrchr(cwd, '/');
        if (lastSlash != NULL) {
            *lastSlash = '\0'; // Null-terminate the string at the last slash position.
        }
        return 0; // Successfully set the current working directory (moved up one level).
    }

    // Otherwise, handle the case when a valid directory path is provided.
    int dirIndex = parsePath(0); // Get the index of the directory entry in the file system.

    if (dirIndex == -1) {
        // Directory not found, so we can't set the current working directory to it.
        return -1;
    }

    // If the directory is found, we can set the current working directory to it.
    // First, reset the "cwd" variable to the root directory.
    strcpy(cwd, "/");

    // Now, reconstruct the path to the directory from the root to the given directory.
    for (int i = 1; i <= arrayCount; i++) {
        if (i == 1 && strcmp(tokenArray[i], "/") == 0) {
            // Skip the first token if it is a slash (root directory).
            continue;
        }
        strcat(cwd, tokenArray[i]);
        strcat(cwd, "/"); // Append a slash to separate directories.
    }

    return 0; // Successfully set the current working directory.
}

