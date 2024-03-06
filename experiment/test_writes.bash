#!/bin/bash

# BASE_DIR variable
BASE_DIR="/tmp/arvgan"

# MOUNT_DIR variable
MOUNT_DIR="${BASE_DIR}/mount_dir/"

# ROOT_DIR variable - uncomment the one you need
ROOT_DIR="arvgan@magenta.cs.utexas.edu:${BASE_DIR}/root_dir"

# CACHE_DIR variable
CACHE_DIR="${BASE_DIR}/cache_dir/"

# TEST_DIRS variable
TEST_DIRS="${MOUNT_DIR} ${ROOT_DIR} ${CACHE_DIR}"

# DEBUG_FLAGS variable
DEBUG_FLAGS="-o default_permissions -d -s"

# RUN_FLAGS variable
RUN_FLAGS="-o default_permissions"

# rm -rf ${CACHE_DIR}*
# ../fs/copyfs ${RUN_FLAGS} ${TEST_DIRS}

# Loop 30 times
for i in {0..30}; do
    # Clear the cache directory
    # ../fs/copyfs ${RUN_FLAGS} ${TEST_DIRS}
    ssh arvgan@magenta.cs.utexas.edu touch /u/arvgan/cs380L/lab2/root_dir/break_cache
    
    OUTPUT=$( time a.out nfs ${i} )
    REAL_TIME=$(echo "$OUTPUT" | tail -n 1)
    COMMAND_OUTPUT=$(echo "$OUTPUT" | head -n -1)
    
    # Unmount the filesystem
    # fusermount -u /tmp/arvgan/mount_dir
    
    # Print the output in a Python-parseable format: "power_of_two: output"
    echo "${i}: \"${REAL_TIME}\""

    ssh arvgan@magenta.cs.utexas.edu rm /u/arvgan/cs380L/lab2/root_dir/break_cache
    
done

fusermount -u /tmp/arvgan/mount_dir