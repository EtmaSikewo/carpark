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

    // Either generate a random plate or read from a file
    FILE *plateFile = fopen("plates.txt", "r");
    char *plate = malloc(6);

    // // Find how many lines in the file
    // int lines = 0;
    // char c;
    // for (c = getc(plateFile); c != EOF; c = getc(plateFile)) {
    //     if (c == '\n') {
    //         lines++;
    //     }
    // }
    //Read a random line from the file using file length 
    int lineNum = rand() % 100;
    // printf("lineNum: %d\n", lineNum);
    // go to a random line in the file
    rewind(plateFile);
    for(int i = 0; i < lineNum; i++){
        fgets(plate, sizeof(plate), plateFile);
    }

    // printf("plate: %s\n", plate);
    // Roll a 50/50 chance to generate a random plate
    if (rand() % 2) {
        // printf("Generating random plate");
        // erase plate
        memset(plate, 0, 6);
        int j = 0;
        for (j = 0; j < 3; j++)
        {
            plate[j] = rand() % 10 + '0';
        }
        for (j = 3; j < 6; j++)
        {
            plate[j] = rand() % 26 + 'A';
        }
        plate[6] = '\0';
    }

    //Close the file
    fclose(plateFile);

    //Return plate
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

//------------------------------------------------------------
//BOILER PLATE STUFF FOR LATER
//------------------------------------------------------------
// Thread for the car
void *carThread(void *arg)
{
    // carcar_t *car = (carcar_t *)arg;

    //Grab all the data of the car
    // char *plate = car->plate;
    // int entryGate = car->entryGate;
    // int exitGate = car->exitGate;
    // int parkDuration = car->parkDuration;

    // Add the car to the queue
    // addCarToQueue(car);

    // Wait for the info to enter

    //for(;;){
        // If (car is rejected)
        // {
        //     // Car leaves
        //     // Car is removed from queue
        // }

        // else if (car is accepted)
        // {
            //Car is accepted 
            // break
        //}


    //}

    // Car waits for 10ms then enters
    // usleep(10 * MS_IN_MICROSECONDS);
    // Head to level and trigger LPR
    // Park 

    //Car parks and waits for duration 
    // usleep(parkDuration * MS_IN_MICROSECONDS);

    //Exits 
    // Head to exit and trigger LPR
    // wait for exit gate to open
    // for (;;)
    // { Wait to exit
    // }
    // done
}


void lprFunction(carcar_t *car) {
    //char queuePlates[5];
    return;



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

    // Simualtor welcome message
    printf("Welcome to the Parking Simulator\n");


    //Set the seed of the random number generator
    srand(time(0));
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
        // Run getPlate
        char *plate = getPlate();
        printf("Plate: %s\n", plate);
        //printf("Looping...\n");

    }

    

    return 0;
}