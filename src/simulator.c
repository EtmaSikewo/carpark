#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
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

char *getPlate() {
    char *plate = malloc(8);
    
    // Having issues with this, need to look into it more

    // Generate a random license plate if the plate is invalid
    // the random plate will have three numbers and three letters
    // erase plate
    memset(plate, 0, 8);
    // Generate 3 random numbers and 3 random letters
    int j = 0;
    for (j = 0; j < 3; j++) {
        plate[j] = rand() % 10 + '0';
    }
    for (j = 3; j < 6; j++) {
        plate[j] = rand() % 26 + 'A';
    }
    plate[6] = '\0';

    return plate;
}

void generateCar(carcar_t *car) {
    // Generate a random license plate
    char *plate = getPlate();
    strcpy(car->plate, plate);
    free(plate);

    // Generate a random entry gate
    car->entryGate = rand() % ENTRANCES;

    // Generate a random exit gate
    car->exitGate = rand() % EXITS;

    // Generate a random park duration
    car->parkDuration = rand() % 10000 + 100;
}

void lprFunction(carcar_t *car) {
    char queuePlates[5];



}

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

        // create a thread for each boom gate
        pthread_t boomgateThread;
        pthread_create(&boomgateThread, &pthreadAttr, (void *)bgFunction, &shm.data->entrance[i].boom_gate);

        // create a thread for each lpr
        pthread_t lprThread;
        pthread_create(&lprThread, &pthreadAttr, (void *)lprFunction, &shm.data->entrance[i].lpr_sensor);
    }
    for (int i = 0; i < EXITS; i++) {
        pthread_mutex_init(&shm.data->exit[i].boom_gate.mutex, NULL);
        pthread_cond_init(&shm.data->exit[i].boom_gate.cond, NULL);
        setBoomGateStatus(&shm.data->exit[i].boom_gate, 'C');

        // create a thread for each boom gate
        pthread_t boomgateThread;
        pthread_create(&boomgateThread, &pthreadAttr, (void *)bgFunction, &shm.data->exit[i].boom_gate);

        // create a thread for each lpr
        pthread_t lprThread;
        pthread_create(&lprThread, &pthreadAttr, (void *)lprFunction, &shm.data->exit[i].lpr_sensor);
    }
}

//  main function to create PARKING mem segment
int main(int argc, char **argv)
{
    //  start the shared memory
    shared_memory_t shm;
    create_shared_object(&shm, "PARKING");
    setDefaults(shm);

    for(int i = 0; i < 1; i++) {
        //  generate a car
        carcar_t car;
        generateCar(&car);
        printf("Car %s has entered the parking lot at gate %d\n", car.plate, car.entryGate);
        pthread_cond_signal(&shm.data->entrance[car.entryGate].lpr_sensor.cond);
        pthread_cond_wait(&shm.data->entrance[car.entryGate].lpr_sensor.cond, &shm.data->entrance[car.entryGate].lpr_sensor.mutex);
    }

    for(;;) {

    }

    

    return 0;
}