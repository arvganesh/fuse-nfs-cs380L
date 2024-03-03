TEST_DIRS = `pwd`/mount_dirs/md1 `pwd`/root_dir `pwd`/cache_dir
DEBUG_FLAGS = -o default_permissions -d -s
ALL_FLAGS =  -o default_permissions -d -s `pwd`/mount_dirs/md2 `pwd`/root_dir `pwd`/cache_dir
compile:
	make -C fs/

clean:
	make -C fs/ clean

run:
	fs/copyfs $(DEBUG_FLAGS) $(TEST_DIRS)