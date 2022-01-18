CFLAGS = -std=c99 -O2 -Wall -fopenmp -I/share/apps/papi/5.4.1/include
CC = gcc
LIBS = -L/share/apps/papi/5.4.1/lib -lm -lpapi

all:
        $(CC) $(CFLAGS) -o sequential bucket_sort_seq.c $(LIBS)
        $(CC) $(CFLAGS) -o parallel bucket_sort_omp.c $(LIBS)

sequential:
        $(CC) $(CFLAGS) -o sequential bucket_sort_seq.c $(LIBS)

parallel:
        $(CC) $(CFLAGS) -o parallel bucket_sort_omp.c $(LIBS)

clean:
        rm sequential parallel