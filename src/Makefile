#Makefile for carpark assignment
CC = gcc
CFLAGS = -Wall -Werror -pedantic -pthread

all: carpark	
	 echo "Done."

manager: manager.o
	 echo "Done."

simulator: simulator.o
	 echo "Done."

firealarm: firealarm.o
	 echo "Done."

manager.o simulator.o firealarm.o:  mem_init.h

carpark: manager simulator firealarm

clean:	
	 rm -f carpark *.o

.PHONY: all clean