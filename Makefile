# Makefile for mem memory system, CSE374 22WI


CC = gcc
CARGS = -Wall -std=c11

all: bench

# basic build
bench: bench.o memory.o mem_utils.o
	$(CC) $(CARGS) -o bench $^

# object files
memory.o: memory.c mem.h mem_impl.h
	gcc -Wall -std=c11 -g -c memory.c

mem_utils.o: mem_utils.c mem_impl.h mem.h
	gcc -Wall -std=c11 -g -c mem_utils.c

bench.o: bench.c mem.h mem_impl.h
	gcc -Wall -std=c11 -g -c bench.c

noassert: CARGS += -D NDEBUG
noassert: bench

debug: CARGS += -g
debug: bench

test: debug
	./bench 10 50

clean:
	rm -f bench *.o *~ 
