#include <stdio.h>
#include "mem_init.h"

//  !!TODO rename this struct
typedef struct carcar
{
    char plate[6];
    int timeEntered; //  time when the car arrives at the first gate
    int entryGate;
    int exitGate;
    char assignedLevel; //  the level the car is assigned to
    int parkDuration;   //  between 100-10000ms
} carcar_t;

//  main function to create PARKING mem segment
int main(int argc, char **argv)
{
    //  create the shared memory segment
    shared_memory_t shm;
    get_shared_object(&shm, "PARKING");


    
    // print status of entrance boom gate
    printf("Entrance boom gate status: %c\n", shm.data->entrance[0].boom_gate.status);

    

    for(;;) {
        pthread_cond_wait(&shm.data->entrance[0].boom_gate.cond, &shm.data->entrance[0].boom_gate.mutex);
         // print status of entrance boom gate
        printf("Entrance boom gate status: %c\n", shm.data->entrance[0].boom_gate.status);
    }
    

    return 0;
}

// FUTURE ME: was last using the functions to create teh shared mem