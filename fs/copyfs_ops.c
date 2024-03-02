#include "copyfs_ops.h"
#include "file.h"

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct fuse_operations copyfs_ops = {
    .open = copyfs_open,
    .read = copyfs_read,
    .write = copyfs_write,
    .getattr = copyfs_getattr,
    .fsync = copyfs_fsync,
    .release = copyfs_release
};

int make_resident(struct file_descriptor* fd) {
    // Get absolute path to open in cache directory.
    char* cache_dir = COPYFS_DATA->cache_dir;
    char full_cache_path[512]; // should be big enough.
    if (!strcpy(full_cache_path, cache_dir)) {
        return -1;
    }
    
    if (!strcat(full_cache_path, fd->path)) {
        return -1;
    }

    // TODO: copy to cache, for now using locally created test files.

    if ((fd->fdnum = open(full_cache_path, fd->flags)) < 0) {
        perror("make_resident open");
        return -1;
    }

    return 0;
}

// lazy open, doesn't actually copy from server till data is needed.
int copyfs_open(const char* path, struct fuse_file_info* fi) {
    if (!(fi->fh = (uint64_t) lazy_open(path, fi->flags))) {
        fprintf(stderr, "Failed to add file %s to file table.\n", path);
        return -1;
    }
    return 0;
}

int copyfs_read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    struct file_descriptor* fd = (struct file_descriptor*) fi->fh;
    int num_bytes;
    if (fd->status == NON_RESIDENT) {
        if (make_resident(fd) < 0) {
            return -1;
        }
    }

    // file is guaranteed to be resident now.
    if (lseek(fd->fdnum, offset, SEEK_SET) < 0) {
        perror("copyfs_read lseek");
        return -1;
    }
    
    if ((num_bytes = read(fd->fdnum, buf, size)) < 0) {
        perror("copyfs_read read");
        return -1;
    }

    return num_bytes;
}

int copyfs_write(const char* path, const char *buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    struct file_descriptor* fd = (struct file_descriptor*) fi->fh;
    int num_bytes;
    if (fd->status == NON_RESIDENT) {
        if (make_resident(fd) < 0) {
            return -1;
        }
    }

    // file is guaranteed to be resident now.
    if (lseek(fd->fdnum, offset, SEEK_SET) < 0) {
        perror("copyfs_write lseek");
        return -1;
    }

    if ((num_bytes = write(fd->fdnum, buf, size)) < 0) {
        perror("copyfs_write write");
        return -1;
    }

    return num_bytes;
}

int copyfs_getattr(const char* path, struct stat* stbuf) {
    fprintf(stderr, "Haven't implemented getattr.\n");
    return -1;
}

int copyfs_fsync(const char* path, int isdatasync, struct fuse_file_info* fi) {
    fprintf(stderr, "Haven't implemented fsync.\n");
    return -1;
}

int copyfs_release(const char* path, struct fuse_file_info *fi) {
    struct file_descriptor* fd = (struct file_descriptor*) fi->fh;

    // Close associated fd
    if (close(fd->fdnum) < 0) {
        perror("copyfs_release close");
        return -1;
    }

    // TODO: copy file back to server if we close the file
    // in future, copy back only when last reference is released.

    free(fd->path);
    free(fd);
    return 0;
}