#ifndef __COPYFS_NETWORK_H
#define __COPYFS_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

FILE* rsync_wrapper(char* source, char* dest, char* opts, bool wait_on_copy);

#endif