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

// // Directory iteration functions
// struct fs_diriteminfo *fs_readdir(fdDir *dirp);
// int fs_closedir(fdDir *dirp);

// Parsepath code begins ---->

// Current temporary use value, need change, or set, later
char cwd[100] = "./../";

struct fdDir* dir = NULL;

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

        return 0;
    }

    if (parentParse == 0)
    {
        // Return the index for parse-pathed-value
        // the tail entry

        for(int i = 1; i < arrayCount; i++)
        {
        strcpy(tmpcwd + strlen(tmpcwd),  tokenArray[i]); 
        strcpy(tmpcwd + strlen(tmpcwd),  "/");
        }

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

        return 0;
    }
    
}

void set_cwd(char* path) {
    strcpy(cwd, path);
}

void set_tmpcwd(char* path) {
    strcpy(tmpcwd, path);
}

// Helper function to reset cwd 
void resetCWD() { //comment code and uncomment for debugging
    //printf("\nBEFORE:\nTMPCWD: %s\nCWD%s", tmpcwd, cwd); //debugging
    strcpy(tmpcwd,  "\0");
    strcpy(cwd,  "\0");
    //printf("\nAfter:\nTMPCWD: %s\nCWD%s", tmpcwd, cwd); //debugging
}

// <------ Parsepath code ends

// open, read, close dir begins here ------->

fdDir *fs_opendir(const char *pathname)
{
    // fdDir dir; // Store in global?

    // call parsePath and return index to the path
    int currentFD = parsePath(0);
    // set cwd if necessary

    if (dir == NULL)
    {
        printf("Error memory allocate directory to store.\n");
    }

    // Need rework for logic figuring out if parsepath is valid or not
    if (currentFD > 1)
    {
        printf("Directory to be opened is valid.\n");
    }
    else 
    {
        printf("Directory to be opened is invalid.\n");
        return 0;
    }

    // Malloc structure to store directory
    dir = malloc(sizeof(fdDir));
    
    dir->d_reclen = globalDirEntries[currentFD].fileSize;
    dir->dirEntryPosition = currentFD;
    dir->directory = globalDirEntries[currentFD];
    //struct fs_diriteminfo * di;		/* Pointer to the structure you return from read */

    return dir;
    }

struct fs_diriteminfo *fs_readdir(fdDir * dirp)
{
    struct fs_diriteminfo* result = NULL;


    // check if the arg is valid
    if (dirp == NULL)
    {
        printf("The directory in dirp is invalid.\n");
        return NULL;
    }
    
    result->d_reclen = globalDirEntries[dirp->dirEntryPosition].fileSize;
    // set the fileType. It could either be a dir or a file.
    result->fileType = globalDirEntries[dirp->dirEntryPosition].isaDirectory;
    // set the name of the directoryItem
    strcpy(result->d_name, globalDirEntries[dirp->dirEntryPosition].fileName);
    
    return result;
}

int fs_closedir(fdDir * dirp)
{
    // free(dirp->directory);
    // dirp->directory = NULL;

    // free the dirp instance
    // free(dirp);
    return 1;
}


// <--------- open, read, close dir code ends here



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

int fs_delete(char* filename)
{
	for (int i=0; i<NUM_DIRECT_ENTRIES; i++){
		if(strcmp(globalDirEntries[i].fileName, filename) ==0){
			strncpy(globalDirEntries[i].fileName, "", MAX_NAME_LENGTH);
    			globalDirEntries[i].fileSize = 0;
    			globalDirEntries[i].fileLocation = -1;
    			globalDirEntries[i].isaDirectory = 1;
    			time_t currentTime = time(NULL);
    			globalDirEntries[i].dateCreated = NULL;
    			globalDirEntries[i].dateAccessed = NULL;
  			globalDirEntries[i].dateModified = NULL;
			printf("File '%s' removed successfully.\n", filename);
			return 0;
			
		}
	}

return -1;
	
	
}




int fs_rmdir(const char *pathname){
	for (int i=0; i<NUM_DIRECT_ENTRIES; i++){
		if(strcmp(globalDirEntries[i].fileName, pathname) ==0){
			if(globalDirEntries[i].fileSize==0){
				strncpy(globalDirEntries[i].fileName, "", MAX_NAME_LENGTH);
    				globalDirEntries[i].fileSize = 0;
    				globalDirEntries[i].fileLocation = -1;
    				globalDirEntries[i].isaDirectory = 1;
    				time_t currentTime = time(NULL);
    				globalDirEntries[i].dateCreated = NULL;
    				globalDirEntries[i].dateAccessed = NULL;
  				globalDirEntries[i].dateModified = NULL;
				printf("Directory '%s' removed successfully.\n", pathname);
				return 0;
			}
		}
	}
	
	return -1;
	

}


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

    if (strlen(cwd) >= size) {
        printf("Buffer size too small to hold the current working directory.\n");
        return NULL;
    }

    // Copy the current working directory to the provided buffer

    strncpy(pathname, cwd, size-1);

    pathname[size - 1] = '\0'; // Ensure the buffer is null-terminated

    return pathname;
}

// Function to set the current working directory
int fs_setcwd(char* pathname) {
    if (pathname == NULL) {
        return -1; // Invalid input
    }

    int parentParseResult = parsePath(0); // Parse the provided pathname

    // Check if the directory exists and it is indeed a directory
    if (parentParseResult == 0 || globalDirEntries[parentParseResult].isaDirectory == 0) {
        return -1; // The provided pathname is invalid or not a directory
    }

    // Copy the parsed pathname to the global cwd variable
    strcpy(cwd, pathname);

    // Append a trailing slash to the cwd if not already present
    if (cwd[strlen(cwd) - 1] != '/') {
        strcpy(cwd + strlen(cwd), "/");
    }

    return 0; 
}
