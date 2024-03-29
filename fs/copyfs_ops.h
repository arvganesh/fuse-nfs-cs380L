#ifndef __COPYFS_OPS_H
#define __COPYFS_OPS_H

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <sys/types.h>

int copyfs_open(const char* path, struct fuse_file_info* fi);

int copyfs_read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi);

int copyfs_write(const char* path, const char *buf, size_t size, off_t offset, struct fuse_file_info* fi);

int copyfs_getattr(const char* path, struct stat* stbuf);

int copyfs_fsync(const char* path, int isdatasync, struct fuse_file_info* fi);

int copyfs_release(const char* path, struct fuse_file_info *fi);

int copyfs_truncate(const char* path, off_t size);

int copyfs_destroy(void* private_data);

int get_full_path(char* path_buffer, const char* relative_path, const char* base_type);

struct copyfs_data {
    char* remote_root_dir; // remote_ip:<absolute path on remote machine>
    char* mount_dir; // <absolute path on local machine>
    char* cache_dir; // <absolute path on local machine>
};

#define COPYFS_DATA ((struct copyfs_data*) fuse_get_context()->private_data)

#endif