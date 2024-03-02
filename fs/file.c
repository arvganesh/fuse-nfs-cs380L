#include "file.h"

struct file_descriptor* lazy_open(const char* path, int flags) {
    struct file_descriptor* new_fd = malloc(sizeof(struct file_descriptor));
    if (!new_fd) return -1;

    new_fd->fdnum = -1;
    new_fd->flags = flags;
    new_fd->status = NON_RESIDENT;
    new_fd->path = path;

    return new_fd;
}

int make_resident(struct file_descriptor* fd) {
    struct fuse_context* context = fuse_get_context();
    char* cache_path = (struct copyfs_data*) context->private_data
}