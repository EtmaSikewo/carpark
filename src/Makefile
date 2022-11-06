#Makefile for carpark assignment
CC = gcc
CFLAGS = -Wall -Werror -pedantic
# Libraries to link with (none for the moment)
LIBS = -lpthread -lrt
# This creates a list of object files from the source files
OBJECTS = $(SOURCES:%.c=%.o)
default: all

all: carpark	
	 echo "Done."

manager.o simulator.o firealarm.o:  mem_init.h
firealarm.o:
	$(CC) $(CFLAGS) -o firealarm.o firealarm.c $(LIBS)
simulator.o: 
	$(CC) $(CFLAGS) -o simulator.o simulator.c $(LIBS)
manager.o: 
	$(CC) $(CFLAGS) -o manager.o manager.c $(LIBS)

carpark: manager.o simulator.o firealarm.o


# This is a target that will compiler all needed source files into object files
# We don't need to specify a command or any rules, "make" will handle it automatically
%.o: %.c

clean:	
	 rm -f carpark *.o

.PHONY: all clean