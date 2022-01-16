CFLAGS = -std=c99 -O2 -Wall -fopenmp
CC = gcc

all:
        $(CC) $(CFLAGS) -o sequential bucket_sort_seq.c
        $(CC) $(CFLAGS) -o parallel bucket_sort_omp.c

sequential:
        $(CC) $(CFLAGS) -o sequential bucket_sort_seq.c

parallel:
        $(CC) $(CFLAGS) -o parallel bucket_sort_omp.c

clean:
        rm -f *.o .a