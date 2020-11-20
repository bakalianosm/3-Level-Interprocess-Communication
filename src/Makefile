
INCLUDE = ../include
MODULES = ../modules

ARGS = -l  2 -u 30000 -w 3

# compiler
CC = gcc 


# Compile options. 
CFLAGS = -Wall -g -I$(INCLUDE)
LDFLAGS = -lm


run: myprime
	./myprime $(ARGS)

valgrind: myprime
	valgrind ./myprime $(ARGS)

all: prime0 prime1 prime2 intermediate myprime

myprime: myprime.c
	gcc myprime.c ../modules/linked_list.c ../modules/functions.c -o myprime 

intermediate: intermediate.c
	gcc intermediate.c ../modules/linked_list.c ../modules/functions.c -o intermediate 

prime0: prime0.c 
	gcc prime0.c -o prime0 -lm

prime1: prime1.c
	gcc prime1.c -o prime1 -lm

prime2: prime2.c
	gcc prime2.c -o prime2 -lm

mytiming: mytiming.c
	gcc mytiming.c -o mytiming

clean:
	rm -f prime0 prime1 prime2 intermediate myprime *.o