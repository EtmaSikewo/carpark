#include <stdio.h>
#include "mem_init.h"


//  !!TODO rename this struct
typedef struct carcar {
    char plate[6];
    int timeEntered;    //  time when the car arrives at the first gate
    int entryGate;
    int exitGate;
    char assignedLevel;  //  the level the car is assigned to
    int parkDuration;  //  between 100-10000ms
} carcar_t = {
    .plate = "000000",
    .exitGate = rand() % ENTRANCES;
};

//  main function to create PARKING mem segment
int createMemSegment(int argc, char** argv) {
    //  create the shared memory segment
    shared_memory_t shm;
    create_shared_object(*shm, "PARKING");



    return 0;
}

// FUTURE ME: was last using the functions to create teh shared mem