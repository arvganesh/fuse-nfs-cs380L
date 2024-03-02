CFLAGS	= -g $(PKGFLAGS) -Wall
PKGFLAGS = `pkg-config fuse --cflags --libs`

copyfs: copyfs_main.o copyfs_ops.o file.o
		gcc $(CFLAGS) -o copyfs copyfs_main.o copyfs_ops.o file.o

copyfs_main.o : copyfs_main.c
		gcc $(CFLAGS) -c copyfs_main.c

copyfs_ops.o : copyfs_ops.c copyfs_ops.h
		gcc $(CFLAGS) -c copyfs_ops.c

file.o : file.c file.h
		gcc $(CFLAGS) -c file.c