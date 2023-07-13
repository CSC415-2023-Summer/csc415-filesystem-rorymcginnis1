#ifndef RD_H
#define RD_H
#include "fsInit.h"
#define NUM_DIRECT_ENTRIES 50
#define MAX_NAME_LENGTH 255
#define BLOCK_SIZE 512



int initialize_root_directory(int minEntreis, DirectoryEntry * parent);

#endif
