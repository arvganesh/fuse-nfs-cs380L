TEST_DIRS = `pwd`/mount_dir `pwd`/root_dir `pwd`/cache_dir
DEBUG_FLAGS = -o default_permissions -d -s

compile:
	make -C fs/

clean:
	make -C fs/ clean

run:
	fs/copyfs $(DEBUG_FLAGS) $(TEST_DIRS)