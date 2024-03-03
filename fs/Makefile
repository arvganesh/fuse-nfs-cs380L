CFLAGS	=  -g $(PKGFLAGS) -Wall
PKGFLAGS = `pkg-config fuse --cflags --libs`

copyfs: copyfs_main.o copyfs_ops.o file.o
	gcc -o copyfs copyfs_main.o copyfs_ops.o file.o $(CFLAGS) 

copyfs_main.o : copyfs_main.c
	gcc -c copyfs_main.c $(CFLAGS) 

copyfs_ops.o : copyfs_ops.c copyfs_ops.h
	gcc -c copyfs_ops.c $(CFLAGS)

file.o : file.c file.h
	gcc -c file.c $(CFLAGS) 

clean:
	rm *.o copyfs