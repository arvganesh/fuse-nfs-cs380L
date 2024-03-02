#include <fuse.h>

int copyfs_open(const char* path, struct fuse_file_info* fi);

int copyfs_read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi));

int copyfs_write(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi);

int copyfs_getattr(const char* path, struct stat* stbuf);

int copyfs_fsync(const char* path, int isdatasync, struct fuse_file_info* fi);

int copyfs_release(const char* path, struct fuse_file_info *fi);

struct fuse_operations copyfs_ops;