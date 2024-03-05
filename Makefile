# MOUNT_DIR = `pwd`/../fs-test-dir/mount_dir
# ROOT_DIR = `pwd`/../fs-test-dir/root_dir
# CACHE_DIR = `pwd`/../fs-test-dir/cache_dir

BASE_DIR = /tmp/arvgan
MOUNT_DIR = $(BASE_DIR)/mount_dir/
ROOT_DIR = arvgan@magenta.cs.utexas.edu:$(BASE_DIR)/root_dir
CACHE_DIR = $(BASE_DIR)/cache_dir/

TEST_DIRS =  $(MOUNT_DIR) $(ROOT_DIR) $(CACHE_DIR)
DEBUG_FLAGS = -o default_permissions -d -s
# r -o default_permissions -d -s /Users/arvgan/Documents/School/CS380L/fuse-nfs-cs380L/../fs-test-dir/mount_dir /Users/arvgan/Documents/School/CS380L/fuse-nfs-cs380L/../fs-test-dir/root_dir /Users/arvgan/Documents/School/CS380L/fuse-nfs-cs380L/../fs-test-dir/cache_dir
# r -o default_permissions -d -s /tmp/arvgan/mount_dir arvgan@magenta.cs.utexas.edu:/tmp/arvgan/root_dir /tmp/arvgan/cache_dir

compile:
	make -C fs/

clean:
	make -C fs/ clean

run:
	fs/copyfs $(DEBUG_FLAGS) $(TEST_DIRS)

unmount-mac:
	sudo diskutil unmount force $(MOUNT_DIR)

unmount:
	fusermount -u $(MOUNT_DIR)

debug-mac:
	lldb fs/copyfs
