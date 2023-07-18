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

int fs_isFile(const char *filename) {
    struct fs_stat statbuf;
    if (fs_stat(filename, &statbuf) == -1) {
        return 0;   //assume not a file if 0 is returned
    }
    //see if it is a file
    return fs_is_a_file(statbuf.fileType);  
}         // may have to add to mfs.h header

int fs_isDir(const char *pathname) {
    struct fs_stat statbuf;
    if (fs_stat(pathname, &statbuf) == -1) {
        return 0;   
        //assume not dir if 0 return
    }
    //see if dir
    return fs_is_a_dir(statbuf.fileType); 
            // may have to add to mfs.h header
}

int fs_mkdir(const char *pathname, mode_t mode) {
    int creationStatus = 0; 

    // attempt to create directory - mode 0 
    //no permisionsss i think need to revisit
    //need to chnage if we decide to add permissions
    b_io_fd fd = b_open(pathname, 0);
    if (fd < 0) {//check for existance
        // dir doesnt already exist, creat4
        fd = b_open(pathname, 1);
        if (fd >= 0) {//check if created
            b_close(fd);//created, then close
        } else {
            creationStatus  = -1; 
            //creation err
        }
    } else {
        // exists
        creationStatus = -1;  
        //err, exists
        b_close(fd);
    }

    return creationStatus;
    //self-explanitory^^^
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
