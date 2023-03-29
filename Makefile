CC=gcc
CFLAGS=-Wall -Iincludes 
LDLIBS=-lm 
VPATH=src

all: test

test: bit.o test.o rank_support.o select_support.o sparse_array.o

clean:
	rm -f *~ *.o test
