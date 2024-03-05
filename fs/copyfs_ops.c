#include "copyfs_ops.h"
#include "copyfs_network.h"
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
    .release = copyfs_release,
    .truncate = copyfs_truncate
};

int get_full_path(char* path_buffer, const char* relative_path, const char* base_type) {
    char* base_path = NULL;
    if (!strcmp(base_type, "root")) {
        base_path = COPYFS_DATA->remote_root_dir;
    } else if (!strcmp(base_type, "cache")) {
        base_path = COPYFS_DATA->cache_dir;
    }

    if (base_path == NULL) {
        fprintf(stderr, "get_full_path: invalid base type.\n");
        return -1;
    }

    if (!strcpy(path_buffer, base_path)) {
        return -1;
    }
    
    if (!strcat(path_buffer, relative_path)) {
        return -1;
    }
    return 0;
}

// full remote path, absolute local path
int copy_from_remote(char* remote_path, char* local_path) {
    FILE* file = rsync_wrapper(remote_path, local_path, "-tr");
    return file == NULL ? -1 : 0;
}

int copy_to_remote(char* remote_path, char* local_path) {
    FILE* file = rsync_wrapper(local_path, remote_path, "-tr");
    return file == NULL ? -1 : 0;
}

int make_resident(const char* path, struct file_descriptor* fd) {
    // Get absolute path to open in cache directory.
    char full_cache_path[512]; // should be big enough.
    if (get_full_path(full_cache_path, path, "cache") < 0) {
        fprintf(stderr, "make_resident: Error getting full cache path\n.");
        return -1;
    }

    // Update file handle if there is an associated one.
    if (fd != NULL && fd->fdnum == -1) {
        if ((fd->fdnum = open(full_cache_path, fd->flags)) < 0) {
            perror("make_resident open");
            return -1;
        }
        fprintf(stderr, "resident FD: %d\n", fd->fdnum);
        fd->status = RESIDENT;
    }

    // Already resident.
    if (!access(full_cache_path, F_OK)) {
        return 0;
    }

    // Get absolute path to open in remote.
    char full_remote_path[128];
    if (get_full_path(full_remote_path, path, "root") < 0) {
        fprintf(stderr, "make_resident: Error getting full remote path\n.");
        return -1;
    }

    // Copy from remote server.
    if (copy_from_remote(full_remote_path, full_cache_path) < 0) {
        fprintf(stderr, "make_resident: failed to copy.\n");
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
    if (make_resident(path, (struct file_descriptor*) fi->fh) < 0) {
        fprintf(stderr, "Failed to make resident: %s\n", path);
        return -1;
    }
    return 0;
}

int copyfs_read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    struct file_descriptor* fd = (struct file_descriptor*) fi->fh;
    int num_bytes;
    if (fd->status != RESIDENT) {
        fprintf(stderr, "copyfs_read: file is non resident???\n");
        return -1;
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
    if (fd->status != RESIDENT) {
        fprintf(stderr, "copyfs_read: file is non resident???\n");
        return -1;
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
    if (make_resident(path, NULL) < 0) {
        fprintf(stderr, "copyfs_getattr: Error copying from remote.");
        return -1;
    }

    char full_cache_path[512]; // should be big enough.
    if (get_full_path(full_cache_path, path, "cache") < 0) {
        fprintf(stderr, "copyfs_getattr: Error getting full cache path\n.");
        return -1;
    }

    if (lstat(full_cache_path, stbuf) < 0) {
        perror("copyfs_getattr: lstat");
        return -1;
    }

    return 0;
}

int copyfs_truncate(const char* path, off_t size) {
    // copy file to cache
    if (make_resident(path, NULL) < 0) {
        fprintf(stderr, "copyfs_truncate: Error copying from remote.");
        return -1;
    }

    char full_cache_path[512]; // should be big enough.
    if (get_full_path(full_cache_path, path, "cache") < 0) {
        fprintf(stderr, "copyfs_truncate: Error getting full cache path\n.");
        return -1;
    }

    // file is guaranteed to be resident now.
    if (truncate(full_cache_path, size) < 0) {
        perror("copyfs_truncate: truncate");
        return -1;
    }

    return 0;
}

int copyfs_fsync(const char* path, int isdatasync, struct fuse_file_info* fi) {
    struct file_descriptor* fd = (struct file_descriptor*) fi->fh;
    
    char full_remote_path[128];
    if (get_full_path(full_remote_path, path, "root") < 0) {
        fprintf(stderr, "copyfs_fsync: Error getting full remote path\n.");
        return -1;
    }

    char full_cache_path[128]; // should be big enough.
    if (get_full_path(full_cache_path, path, "cache") < 0) {
        fprintf(stderr, "copyfs_fsync: Error getting full cache path\n.");
        return -1;
    }

    if (copy_to_remote(full_remote_path, full_cache_path) < 0) {
        fprintf(stderr, "copyfs_fsync: Failed to copy to remote\n.");
        return -1;
    }

    return 0;
}

int copyfs_release(const char* path, struct fuse_file_info *fi) {
    struct file_descriptor* fd = (struct file_descriptor*) fi->fh;

    // Close associated fd
    if (fd->status == RESIDENT && close(fd->fdnum) < 0) {
        fprintf(stderr, "copyfs_release: Releasing of %s failed\n", path);
        perror("copyfs_release: close");
        return -1;
    }

    // TODO: copy file back to server if we close the file
    // in future, copy back only when last reference is released.
    char full_remote_path[128];
    if (get_full_path(full_remote_path, path, "root") < 0) {
        fprintf(stderr, "copyfs_release: Error getting full remote path\n.");
        return -1;
    }

    char full_cache_path[128]; // should be big enough.
    if (get_full_path(full_cache_path, path, "cache") < 0) {
        fprintf(stderr, "copyfs_release: Error getting full cache path\n.");
        return -1;
    }

    if (copy_to_remote(full_remote_path, full_cache_path) < 0) {
        fprintf(stderr, "copyfs_release: Failed to copy to remote\n.");
        return -1;
    }
    
    free(fd->path);
    free(fd);
    return 0;
}