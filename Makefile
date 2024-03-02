TEST_DIRS = `pwd`/mount_dir `pwd`/root_dir `pwd`/cache_dir

compile:
	make -C fs/

clean:
	make -C fs/ clean

run:
	fs/copyfs -o default_permissions -d -s $(TEST_DIRS)