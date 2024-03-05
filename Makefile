BASE_DIR = /tmp/arvgan
MOUNT_DIR = $(BASE_DIR)/mount_dir/
ROOT_DIR = arvgan@magenta.cs.utexas.edu:$(BASE_DIR)/root_dir
CACHE_DIR = $(BASE_DIR)/cache_dir/

TEST_DIRS =  $(MOUNT_DIR) $(ROOT_DIR) $(CACHE_DIR)
DEBUG_FLAGS = -o default_permissions -d -s
RUN_FLAGS = -s -o default_permissions

compile:
	make -C fs/

clean:
	make -C fs/ clean

run-debug:
	fs/copyfs $(DEBUG_FLAGS) $(TEST_DIRS)

run:
	rm -rf $(CACHE_DIR)*
	fs/copyfs $(RUN_FLAGS) $(TEST_DIRS)

unmount-mac:
	sudo diskutil unmount force $(MOUNT_DIR)

unmount:
	fusermount -u $(MOUNT_DIR)

debug-mac:
	lldb fs/copyfs
