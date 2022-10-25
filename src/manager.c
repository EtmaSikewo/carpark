#include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <pthread.h>
// #include <sys/mman.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <fcntl.h>
#include "mem_init.h"

// int currLevelCapacity[LEVELS];

//  structs for car and level managers
// typedef struct car_manager
// {
//     char plate[6];   //  default value is "000000"
//     int timeEntered; //  between 100-10000ms
// } car_manager_t;
// typedef struct level_manager
// {
//     int currLevelCapacity;
//     car_manager_t car_manager[PARKING_CAPACITY];
// } levels_t;

// levels_t levels[LEVELS];

int main(void)
{
    //  create the shared memory segment
    shared_memory_t shm;
    get_shared_object(&shm, "PARKING"); // Need to error handle this

    for(;;) {
        for (int i = 0; i < ENTRANCES; i++) {
            pthread_cond_signal(&shm.data->entrance[i].boom_gate.cond);
            pthread_cond_wait(&shm.data->entrance[i].boom_gate.cond, &shm.data->entrance[i].boom_gate.mutex);
            //this is when the car will drive through the boom gate
            printf("car has entered the parking lot\n");
        }
    }

    return 0;
}