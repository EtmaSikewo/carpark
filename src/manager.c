#include <stdio.h>
#include<stdlib.h>
// #include <time.h>
// #include <pthread.h>
// #include <sys/mman.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <fcntl.h>
#include "mem_init.h"

#define MS_IN_MICROSECONDS 1000

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
    
    // try to get shared memory object parking
    

    // for(;;) {
    //     for (int i = 0; i < ENTRANCES; i++) {
    //         pthread_cond_signal(&shm.data->entrance[i].boom_gate.cond);
    //         pthread_cond_wait(&shm.data->entrance[i].boom_gate.cond, &shm.data->entrance[i].boom_gate.mutex);
    //         //this is when the car will drive through the boom gate
    //         printf("car has entered the parking lot\n");
    //     }
    // }







// ---------------------------------------------
// Template for the manager UI 
// ---------------------------------------------

/*
Status of the boom gates
Entrances           Exits
1: C                1: C
2: C                2: C
3.C                 3: C
4: C                4: C
5: C                5: C

Status of the LPR sensors
Entrances           Exits           Level
1: 
2:
3:
4:
5:

Status of the information signs 
1:  2:  3:  4:  5:

Level information 
level 1: 0/100
level 2: 0/100
level 3: 0/100
level 4: 0/100
level 5: 0/100
*/

    // Sleep for abit 
    usleep(4000 * MS_IN_MICROSECONDS);
    // Set all boom gates to raise
    // for (int i = 0; i < ENTRANCES; i++) {
    //     shm.data->entrance[i].boom_gate.status = 'L';
    // }

    for(;;){
        
        

        //Clear the console each loop
        system("clear");
        //Print the status of the boom gates
        printf("Status of the boom gates\n");
        printf("Entrances           Exits\n");
        for(int i = 0; i < ENTRANCES; i++){
            printf("%d: %c                %d: %c\n", i+1, shm.data->entrance[i].boom_gate.status, i+1, shm.data->exit[i].boom_gate.status);
        }
        printf("\n");
        //Print the status of the LPR sensors
        printf("Status of the LPR sensors\n");
        printf("Entrances           Exits           Level\n");
        for(int i = 0; i < ENTRANCES; i++){
            printf("%d: %s                %d: %s                %d: %s\n", i+1, shm.data->entrance[i].lpr_sensor.plate, i+1, shm.data->exit[i].lpr_sensor.plate, i+1, shm.data->level[i].lpr_sensor.plate);
        }
        // printf("\n");
        // //Print the status of the information signs
        // printf("Status of the information signs\n");
        // for(int i = 0; i < ENTRANCES; i++){
        //     printf("%d: %c  ", i+1, shm.data->entrance[i].info_sign.status);
        // }
        // printf("\n");
        // //Print the level information
        // printf("Level information\n");
        // for(int i = 0; i < LEVELS; i++){
        //     printf("level %d: %d/%d\n", i+1, shm.data->level[i].currLevelCapacity, PARKING_CAPACITY);
        // }
        printf("\n");
        usleep(5 * MS_IN_MICROSECONDS);

    

    }


}



