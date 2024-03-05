#include "copyfs_network.h"

FILE* rsync_wrapper(char* source, char* dest, char* opts) {
    char command[512];
    
    if (strcpy(command, "rsync") < 0) return NULL;
    if (strcat(command, " ") < 0) return NULL;
    if (strcat(command, opts) < 0) return NULL;
    if (strcat(command, " ") < 0) return NULL;
    if (strcat(command, source) < 0) return NULL;
    if (strcat(command, " ") < 0) return NULL;
    if (strcat(command, dest) < 0) return NULL;

    fprintf(stderr, "Copying from %s to %s\n", source, dest);
    fprintf(stderr, "Running command %s\n", command);
    FILE* f = popen(command, "r");

    // Wait for the child process to finish.
    int status = pclose(f);
    fprintf(stderr, "rsync STATUS: %d\n", status);
    if (status != 0) {
        perror("pclose() failed");
        return NULL;
    }

    return f;
}