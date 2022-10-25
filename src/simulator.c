#include <stdio.h>
#include "mem_init.h"

#define MS_IN_MICROSECONDS 1000

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

//A set of functions that sets the default value for all boom gates
void bgFunction(void * bg)
{
    boom_gate_t * boom_gate = (boom_gate_t *)bg;
    pthread_mutex_lock(&boom_gate->mutex);
    for(;;) {
        pthread_cond_wait(&boom_gate->cond, &boom_gate->mutex);
        if (boom_gate->status == 'C') {
            boom_gate->status = 'R';
            usleep(10 * MS_IN_MICROSECONDS);
            boom_gate->status = 'O';
            pthread_cond_broadcast(&boom_gate->cond);
            usleep(20 * MS_IN_MICROSECONDS);
            boom_gate->status = 'L';
            usleep(10 * MS_IN_MICROSECONDS);
            boom_gate->status = 'C';
        }
        else {
            printf("boomgate status is illegal\n");
        }
    }
    pthread_mutex_unlock(&boom_gate->mutex);
}

//create a function that sets the default value for boom gate
void setBoomGateStatus(boom_gate_t *boom_gate, char status)
{
    boom_gate->status = status;
}

void setDefaults(shared_memory_t shm) {
    
    pthread_attr_t pthreadAttr;
    pthread_mutexattr_t mutexAttr;
    pthread_condattr_t condAttr;
    pthread_attr_init(&pthreadAttr);
    pthread_mutexattr_init(&mutexAttr);
    pthread_condattr_init(&condAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);

    for (int i = 0; i < ENTRANCES; i++) {
        pthread_mutex_init(&shm.data->entrance[i].boom_gate.mutex, &mutexAttr);
        pthread_cond_init(&shm.data->entrance[i].boom_gate.cond, &condAttr);
        setBoomGateStatus(&shm.data->entrance[i].boom_gate, 'C');
        pthread_t boomgateThread;
        pthread_create(&boomgateThread, &pthreadAttr, (void *)bgFunction, &shm.data->entrance[i].boom_gate);
    }
    for (int i = 0; i < EXITS; i++) {
        pthread_mutex_init(&shm.data->exit[i].boom_gate.mutex, NULL);
        pthread_cond_init(&shm.data->exit[i].boom_gate.cond, NULL);
        setBoomGateStatus(&shm.data->exit[i].boom_gate, 'C');
        pthread_t boomgateThread;
        pthread_create(&boomgateThread, &pthreadAttr, (void *)bgFunction, &shm.data->exit[i].boom_gate);
    }
}

//  main function to create PARKING mem segment
int main(int argc, char **argv)
{
    //  start the shared memory
    shared_memory_t shm;
    create_shared_object(&shm, "PARKING");
    setDefaults(shm);

    for(;;) {
        //do nothing
    }

    return 0;
}