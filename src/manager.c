// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <pthread.h>
// #include <sys/mman.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <fcntl.h>

// #include "shared_mem_access.h"

// //  !!TODO basic functions for things

// int main(int argc, char** argv)
// {
//     memoryAccess();
//     return 0;
// }

// // function for sending the command to open a boom gate
// void openGate(int gateID, unsigned char state)
// {
//     //
// }

#include <stdio.h>
#include "mem_init.h"

int currLevelCapacity[LEVELS];

typedef struct car_manager
{
    char plate[6];   //  default value is "000000"
    int timeEntered; //  between 100-10000ms
} car_manager_t;

typedef struct level_manager
{
    int currLevelCapacity;
    car_manager_t car_manager[PARKING_CAPACITY];
} levels_t;

levels_t levels[LEVELS];

//create a function that sets the default value for boom gate
void setBoomGate(boom_gate_t *boom_gate, char status)
{
    boom_gate->status = status;
}

void setDefaults(shared_memory_t shm) {
    for (int i = 0; i < ENTRANCES; i++) {
        setBoomGate(&shm.data->entrance[i].boom_gate, 'C');
    }
    for (int i = 0; i < EXITS; i++) {
        setBoomGate(&shm.data->exit[i].boom_gate, 'C');
    }
}

// A function that opens a boomgate with mutex
void open_boom_gate(boom_gate_t *boom_gate) {
    pthread_mutex_lock(&boom_gate->mutex);
    if (boom_gate->status == 'C') {
        printf("%c \n", boom_gate->status);
        boom_gate->status = 'R';
        printf("%c \n", boom_gate->status);
        pthread_mutex_unlock(&boom_gate->mutex);
        usleep(10000);
        pthread_mutex_lock(&boom_gate->mutex);
        boom_gate->status = 'O';
        printf("%c \n", boom_gate->status);
        pthread_mutex_unlock(&boom_gate->mutex);
    } else {
        pthread_mutex_unlock(&boom_gate->mutex);
    }
    pthread_cond_signal(&boom_gate->cond);
    pthread_mutex_unlock(&boom_gate->mutex);
}

// A function that closes a boomgate with mutex
void close_boom_gate(boom_gate_t *boom_gate) {
    pthread_mutex_lock(&boom_gate->mutex);
    if (boom_gate->status == 'O') {
        printf("%c \n", boom_gate->status);
        boom_gate->status = 'L';
        printf("%c \n", boom_gate->status);
        pthread_mutex_unlock(&boom_gate->mutex);
        usleep(10000);
        pthread_mutex_lock(&boom_gate->mutex);
        boom_gate->status = 'C';
        printf("%c \n", boom_gate->status);
        pthread_mutex_unlock(&boom_gate->mutex);
    } else {
        pthread_mutex_unlock(&boom_gate->mutex);
    }
    pthread_cond_signal(&boom_gate->cond);
    pthread_mutex_unlock(&boom_gate->mutex);
}

int main(void)
{
    // printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    // //  read in from the shared memory pool
    // memoryAccess();

    shared_memory_t shm;

    create_shared_object(&shm, "pog");
    setDefaults(shm);
    open_boom_gate(&shm.data->entrance[0].boom_gate);
    close_boom_gate(&shm.data->entrance[0].boom_gate);


    return 0;
}