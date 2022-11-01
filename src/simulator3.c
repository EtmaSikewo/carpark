#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "mem_init.h"

// Global variables for the carpark simulator 
// ---------------------------------------------
// Definitions 
#define MS_IN_MICROSECONDS 1000


// ---------------------------------------------
// Methods in charge of random generation 
// ---------------------------------------------

// The random function needs to be thread safe when generating random numbers
// ---------------------------------------------
pthread_mutex_t rand_mutex;;
int randThread(void){
    int randomNumber;
    pthread_mutex_lock(&rand_mutex);
    randomNumber = rand();
    pthread_mutex_unlock(&rand_mutex);
    return randomNumber;
}

// Random generator for picking a carpark level
// ---------------------------------------------



// ---------------------------------------------
// Functions relating to the boomgate          |
// ---------------------------------------------

// Function to initalise the boom gate
// ---------------------------------------------
void *boomgateInit(void *arg){
    boom_gate_t *boom_gate = arg;
    
    // Error check the mutex and condition variable
    if (pthread_mutex_init(&boom_gate->mutex, NULL) != 0) {
        printf("Boom gate mutex init failed");
    }

    // Lock the mutex 
    pthread_mutex_lock(&boom_gate->mutex);
    // Set the boom gate to closed
    boom_gate->status = 'C';
    //Broadcast the condition variable
    pthread_cond_broadcast(&boom_gate->cond);
    // Unlock the mutex
    pthread_mutex_unlock(&boom_gate->mutex);
    return NULL;

}

// Function that will be called by the boom gate thread
// ---------------------------------------------
void boomGateSimualtor(void *arg){
    // Get the boom gate thread
    boom_gate_t *boom_gate = arg;
    // Lock the boomgate mutex for use
    pthread_mutex_lock(&boom_gate->mutex);
    // Logic for the boom gate
    //------------------------
    // If the boomgate has been set to raise
    if (boom_gate->status == 'R'){
        // Raise the boom gate after 10ms
        usleep(10 * MS_IN_MICROSECONDS);
        boom_gate->status = 'O';
        // Broadcast the change to the boom gate
        pthread_cond_broadcast(&boom_gate->cond);
        //Print the boom gate status
        printf("Boom gate opening");
    }
    // If the boom gate has been set to open
    else if (boom_gate->status == 'O'){
        //Print the boom gate status
        printf("Boom gate is open");
    }
    // If the boom gate is set to lowering 
    else if (boom_gate->status == 'L'){
        // Lower the boom gate after 10ms
        usleep(10 * MS_IN_MICROSECONDS);
        boom_gate->status = 'C';
        // Broadcast the change to the boom gate
        pthread_cond_broadcast(&boom_gate->cond);
        //Print the boom gate status
        printf("Boom gate is closing");
    }
    // If the boom gate is set to closed
    else if (boom_gate->status == 'C'){
        //Print the boom gate status
        printf("Boom gate is closed");
    }
    // If the boom gate is set to an illegal status
    else{
        //Print the boom gate status
        printf("Boom gate is in an illegal state");
    }
    // Unlock the boom gate mutex
    pthread_mutex_unlock(&boom_gate->mutex);
    printf("\n");
    // sleep for a bit
    usleep(5000 * MS_IN_MICROSECONDS);

}


// ---------------------------------------------
// Functions relating to the car               |
// ---------------------------------------------

// Function to get a plate from plates.txt
// ---------------------------------------------
char *getPlate(){
    // Allocate memory for the plate
    char *plate = malloc(6);
    FILE *fp = fopen("plates.txt", "r");
    // Error check the file
    if (fp == NULL){
        printf("Error opening file");
    }
    // Pick a random line 
    int randomPlateLine = randThread()%100 + 1;
    // printf("Line: %d\n", randomPlateLine);
    for(int i = 0; i < randomPlateLine; i++){
        fgets(plate, sizeof(plate)+1, fp);
    }
    fclose(fp);
    return plate;
}

// ---------------------------------------------
// Set defaults 
// ---------------------------------------------
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
    
    pthread_t *boomgatethreads = malloc(sizeof(pthread_t) * (ENTRANCES + EXITS));
    
    for (int i = 0; i < ENTRANCES; i++) {
		boom_gate_t *bg = &shm.data->entrance[i].boom_gate;
		pthread_create(boomgatethreads + i, NULL, boomgateInit,(void *) bg);
	}

    for (int i = 0; i < EXITS; i++) {
        boom_gate_t *bg = &shm.data->exit[i].boom_gate;
        pthread_create(boomgatethreads + i, NULL, boomgateInit,(void *) bg);
    }


    // Create threads for the boom gate
    //pthread_t *boomgatethreads = malloc(sizeof(pthread_t) * (ENTRANCES + EXITS));

    // for (int i = 0; i < ENTRANCES; i++){
    //     boom_gate_t *boomgate = &shm.data->entrance[i].boom_gate;
    //     boomgate->status = 'C';
    //     pthread_create(&boomgatethreads[i], &pthreadAttr, boomgateInit, boomgate);
    // }


    // for (int i = 0; i < ENTRANCES; i++) {

    //     pthread_mutex_init(&shm.data->entrance[i].boom_gate.mutex, &mutexAttr);
    //     pthread_cond_init(&shm.data->entrance[i].boom_gate.cond, &condAttr);
    //     setBoomGateStatus(&shm.data->entrance[i].boom_gate, 'C');
        

    // }

    // for (int i = 0; i < EXITS; i++) {
    //     pthread_mutex_init(&shm.data->exit[i].boom_gate.mutex, NULL);
    //     pthread_cond_init(&shm.data->exit[i].boom_gate.cond, NULL);
    //     setBoomGateStatus(&shm.data->exit[i].boom_gate, 'C');
    // }
}

// main function 
int main(void)
{
    printf("Welcome to the simulator\n");
     //Set the seed of the random number generator
    srand(time(0));

    shared_memory_t shm;
    create_shared_object(&shm, "PARKING");
    setDefaults(shm);

    // Loop through get plate function
    // for (;;){
    //     // Get a plate
    //     char *plate = getPlate();
    //     // Print the plate
    //     printf("%s", plate);
    //     // Sleep for a bit
    //     usleep(1000 * MS_IN_MICROSECONDS);
        
    //     // open all the boom gates
    //     for (int i = 0; i < ENTRANCES; i++) {
    //         shm.data->entrance[i].boom_gate.status = 'R';
    //     }
    //     // sleep for a bit
    //     usleep(2000 * MS_IN_MICROSECONDS);

    //     // check all the boom gates
    //     for (int i = 0; i < ENTRANCES; i++) {
    //         pthread_t boomgatethread;
    //         pthread_create(&boomgatethread, NULL, boomGateSimualtor, &shm.data->entrance[i].boom_gate);
    //         pthread_join(boomgatethread, NULL);
    //     }
    //     // sleep for a bit
    //     usleep(2000 * MS_IN_MICROSECONDS);
    //     // close all the boom gates
    //     for (int i = 0; i < ENTRANCES; i++) {
    //         shm.data->entrance[i].boom_gate.status = 'L';
    //     }

    //     // check all the boom gates
    //     for (int i = 0; i < ENTRANCES; i++) {
    //         pthread_t boomgatethread;
    //         pthread_create(&boomgatethread, NULL, boomGateSimualtor, &shm.data->entrance[i].boom_gate);
    //         pthread_join(boomgatethread, NULL);

    //     }

    //     // sleep for a bit
    //     usleep(2000 * MS_IN_MICROSECONDS);
        
    // }


    for (;;) {

        
        

        for(int i = 0; i < ENTRANCES; i++) {
            boom_gate_t *bg = &shm.data->entrance[i].boom_gate;

            if (bg->status == 'R'){
                pthread_t boomgatethread;
                pthread_create(&boomgatethread, NULL,(void *(*)(void *))boomGateSimualtor, (void *) bg);
                pthread_join(boomgatethread, NULL);
            }
            else if (bg->status == 'L'){
                pthread_t boomgatethread;
                pthread_create(&boomgatethread, NULL,(void *(*)(void *))boomGateSimualtor, (void *) bg);
                pthread_join(boomgatethread, NULL);
            }
        }
        

    }


}