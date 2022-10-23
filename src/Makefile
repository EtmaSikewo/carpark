#Makefile for carpark assignment
CC = gcc
CFLAGS = -Wall -Werror -pedantic -pthread -ltr

all: carpark	
	 echo "Done."

manager.o simulator.o firealarm.o:  mem_init.h

carpark: manager.o simulator.o firealarm.o

clean:	
	 rm -f carpark *.o

.PHONY: all clean