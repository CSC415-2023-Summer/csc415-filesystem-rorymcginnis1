#ifndef RD_H
#define RD_H

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

int initialize_root_directory(int minEntreis, Direct * parent);

#endif
