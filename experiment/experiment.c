#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#define min(a, b) ((a) < (b) ? (a) : (b));

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Invalid arguments. Usage: ./a.out [nfs|myfs]\n");
        return -1;
    }

    int pow = atoi(argv[2]);
    const int FINAL_FILE_SIZE = (1 << pow);
    int WRITE_SIZE = min(4096, FINAL_FILE_SIZE);
    const int NUM_ITERS =  FINAL_FILE_SIZE / WRITE_SIZE;

    char write_path[128];
    if (!strcmp(argv[1], "nfs")) {
        strcpy(write_path, "/u/arvgan/cs380L/lab2/root_dir/big_file_");
    } else {
        strcpy(write_path, "/tmp/arvgan/mount_dir/big_file_");
    }

    strcat(write_path, argv[2]);
    assert(write_path != NULL);

    fprintf(stderr, "writing to %s\n, write_size: %d\n", write_path, WRITE_SIZE);

    char buf[WRITE_SIZE];
    memset(buf, 1, WRITE_SIZE);
    int write_fd = open(write_path, O_WRONLY, O_APPEND);
    for (int i = 0; i < NUM_ITERS; i++) {
        write(write_fd, buf, WRITE_SIZE);
    }
    
    close(write_fd);
    return 0;
}
