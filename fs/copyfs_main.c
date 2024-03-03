#include "copyfs_ops.h"

#include <fuse.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

extern struct fuse_operations copyfs_ops;

int populate_fs_metadata(int argc, char** argv, struct copyfs_data* data) {
    data = malloc(sizeof(struct copyfs_data));
    if (!data) exit(EXIT_FAILURE);

    char* cache_dir = argv[argc - 1];
    data->cache_dir = realpath(cache_dir, NULL);
    if(!data->cache_dir) {
        fprintf(stderr, "Error getting path\n");
    }

    char* remote_root_dir = argv[argc - 2];
    data->remote_root_dir = remote_root_dir;
    if(!data->remote_root_dir) {
        fprintf(stderr, "Error getting path\n");
    }

    char* mount_dir = argv[argc - 3];
    data->mount_dir = realpath(mount_dir, NULL);
    if(!data->mount_dir) {
        fprintf(stderr, "Error getting path\n");
    }
    return 0;
}

int update_fuse_args(int* argc_ptr, char** argv) {
    int argc = *argc_ptr;
    argv[argc - 1] = NULL; // cache dir
    argv[argc - 2] = NULL; // remote root
    *argc_ptr -= 2;
    return 0;
}

int main(int argc, char** argv) {
    // usage: ./copyfs mount_dir remote_ip:root_dir cache_dir

    // assume valid argv input.
    if (argc < 4) {
        fprintf(stderr, "Must supply a root directory, mount directory, and cache directory.\n");
        return -1;
    }

    // populate 
    struct copyfs_data* copyfs_data;
    populate_fs_metadata(argc, argv, copyfs_data);

    // fuse only needs the mount dir
    update_fuse_args(&argc, argv);
    umask(0);
    printf("Mounting filesystem on %s\n", argv[argc - 1]);
    return fuse_main(argc, argv, &copyfs_ops, copyfs_data);
}