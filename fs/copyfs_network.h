#ifndef __COPYFS_NETWORK_H
#define __COPYFS_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* rsync_wrapper(char* source, char* dest, char* opts);

#endif