#include "file.h"

#include <stdlib.h>
#include <string.h>

struct file_descriptor* lazy_open(const char* path, int flags) {
    struct file_descriptor* new_fd = malloc(sizeof(struct file_descriptor));
    if (!new_fd) return NULL;

    new_fd->fdnum = -1;
    new_fd->flags = flags;
    new_fd->status = NON_RESIDENT;
    new_fd->path = malloc(strlen(path) + 1);
    if (strcpy(new_fd->path, path) < 0) return NULL;

    return new_fd;
}