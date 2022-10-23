#include <stdio.h>
// #include <stdlib.h>
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

//create a function that sets the default value for boom gate
void setBoomGateStatus(boom_gate_t *boom_gate, char status)
{
    boom_gate->status = status;
}

void setDefaults(shared_memory_t shm) {
    
    pthread_mutexattr_t mutexAttr;  //  !!TODO can return an error number if unsuccessful https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-pthread-mutexattr-init-initialize-mutex-attribute-object
    pthread_condattr_t condAttr;
    pthread_mutexattr_init(&mutexAttr);
    pthread_condattr_init(&condAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);


    for (int i = 0; i < ENTRANCES; i++) {
        pthread_mutex_init(&shm.data->entrance[i].boom_gate.mutex, &mutexAttr);
        pthread_cond_init(&shm.data->entrance[i].boom_gate.cond, &condAttr);
        setBoomGateStatus(&shm.data->entrance[i].boom_gate, 'C');
    }
    for (int i = 0; i < EXITS; i++) {
        pthread_mutex_init(&shm.data->exit[i].boom_gate.mutex, NULL);
        pthread_cond_init(&shm.data->exit[i].boom_gate.cond, NULL);
        setBoomGateStatus(&shm.data->exit[i].boom_gate, 'C');
    }
}

// A function that opens a boomgate with mutex
void open_boom_gate(boom_gate_t *boom_gate) {
    pthread_mutex_lock(&boom_gate->mutex);
    if (boom_gate->status == 'C') {
        printf("%c \n", boom_gate->status);
        boom_gate->status = 'R';
        printf("%c \n", boom_gate->status);
        usleep(10 * MS_IN_MICROSECONDS);
        boom_gate->status = 'O';
        printf("%c \n", boom_gate->status);
        pthread_cond_broadcast(&boom_gate->cond);
    }
    pthread_mutex_unlock(&boom_gate->mutex);
}

// A function that closes a boomgate with mutex
void close_boom_gate(boom_gate_t *boom_gate) {
    pthread_mutex_lock(&boom_gate->mutex);
    if (boom_gate->status == 'O') {
        printf("%c \n", boom_gate->status);
        boom_gate->status = 'L';
        printf("%c \n", boom_gate->status);
        usleep(10 * MS_IN_MICROSECONDS);
        boom_gate->status = 'C';
        printf("%c \n", boom_gate->status);
        pthread_cond_broadcast(&boom_gate->cond);
    }
    pthread_mutex_unlock(&boom_gate->mutex);
}

int main(void)
{
    // !!TODO move this to simulator.c
    //  start the shared memory
    shared_memory_t shm;
    create_shared_object(&shm, "PARKING");
    setDefaults(shm);
    
    // //  read in from the shared memory pool
    // memoryAccess();

    //  open and close boom gate every second
    for (;;) {
        open_boom_gate(&shm.data->entrance[0].boom_gate);
        usleep(1000 * MS_IN_MICROSECONDS);
        close_boom_gate(&shm.data->entrance[0].boom_gate);
        usleep(1000 * MS_IN_MICROSECONDS);
    }

    return 0;
}