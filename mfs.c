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
extern struct DirectoryEntry *newD;
int fs_stat( const char *path, struct fs_stat *buf)
{

	for (int i=0; i< NUM_DIRECT_ENTRIES; i++){
		if(strcmp(path, newD[i].fileName)==0){
			buf ->st_size = newD[i].fileSize;
			buf-> st_blksize = BLOCK_SIZE;
			buf-> st_blocks = (newD[i].fileSize + BLOCK_SIZE -1)/BLOCK_SIZE;
			buf-> st_accesstime = newD[i].dateAccessed;
			buf->st_modtime =newD[i].dateModified;
			buf->st_createtime = newD[i].dateCreated;
			
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
        if (strcmp(newD[i].fileName, pathname) == 0) {
            return -1; // Return -1 as the directory already exists
        }
    }

    // find available spot for directory entry
    int dirIndex = -1; // Index of the available directory entry slot
    for (int i = 0; i < NUM_DIRECT_ENTRIES; i++) {
        // fileName of the current directoryentry  is an empty string,
        // slot available
        if (newD[i].fileName[0] == '\0') { 
            dirIndex = i; // slot, store its index
            break; // Break
        }
    }

    // no available slot
    if (dirIndex == -1) {
        return -1; // Return -1 indicating no space for the new directory
    }

    // Initialize the new directory entry
    strncpy(newD[dirIndex].fileName, pathname, MAX_NAME_LENGTH); // Copy the pathname into the fileName
    newD[dirIndex].fileSize = 0; // Initial size of directory is 0
    newD[dirIndex].fileLocation = -1; // Set fileLocation to an invalid value (-1) indicating the directory is initially empty
    newD[dirIndex].isaDirectory = 1; // Flag to indicate that this entry is a directory

    // Get the current time
    time_t currentTime = time(NULL);
    // Set dateCreated, dateAccessed, and dateModified to current time
    newD[dirIndex].dateCreated = currentTime;
    newD[dirIndex].dateAccessed = currentTime;
    newD[dirIndex].dateModified = currentTime;

    return 0; // Return 0 indicating that the directory was created successfully
}




//below is irrelevant to this code but referenced for it
//delete later, or at least reformat so noot over 100 char inline
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//------------------------------------------------------------------
//saw fileType below in the README file
//line 74
// struct fs_diriteminfo
//     {
//     unsigned short d_reclen;    /* length of this record */
//     unsigned char fileType;    
//     char d_name[256]; 			/* filename max filename is 255 characters */
//     };

//saw the statbuf is fsshell.c
//line 93
// int displayFiles (fdDir * dirp, int flall, int fllong)
// 	{
// #if (CMDLS_ON == 1)				
// 	if (dirp == NULL)	//get out if error
// 		return (-1);
	
// 	struct fs_diriteminfo * di;
// 	struct fs_stat statbuf;
	
// 	di = fs_readdir (dirp);
// 	printf("\n");
// 	while (di != NULL) 
// 		{
// 		if ((di->d_name[0] != '.') || (flall)) //if not all and starts with '.' it is hidden
// 			{
// 			if (fllong)
// 				{
// 				fs_stat (di->d_name, &statbuf);
// 				printf ("%s    %9ld   %s\n", fs_isDir(di->d_name)?"D":"-", statbuf.st_size, di->d_name);
// 				}
// 			else
// 				{
// 				printf ("%s\n", di->d_name);
// 				}
// 			}
// 		di = fs_readdir (dirp);
// 		}
// 	fs_closedir (dirp);
// #endif
// 	return 0;
// 	}
