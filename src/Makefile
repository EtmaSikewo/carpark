 #Makefile for carpark assignment
CC = gcc
CFLAGS = -Wall -Werror -pedantic
# Flags for linking (none for the moment)
LDFLAGS =
# Libraries to link with (none for the moment)
LIBS = -lpthread -lrt
# This creates a list of object files from the source files
OBJECTS = $(SOURCES:%.c=%.o)
default: all

all: carpark
	echo "Done."

manager.o simulator.o firealarm.o:  mem_init.h
firealarm.o: firealarm.h
	$(CC) $(LDFLAGS) -o firealarm.o firealarm.c $(LIBS)
simulator.o: 
	$(CC) $(LDFLAGS) -o simulator.o simulator.c $(LIBS)
manager.o: 
	$(CC) $(LDFLAGS) -o manager.o manager.c $(LIBS)

carpark: manager.o simulator.o firealarm.o
    
# This will link the executable from the object files
$(EXE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o  $(EXE) $(LIBS)


# This is a target that will compiler all needed source files into object files
# We don't need to specify a command or any rules, "make" will handle it automatically
%.o: %.c

clean:	
	 rm -f carpark *.o

clean:
	rm -f carpark *.o

.PHONY: all clean