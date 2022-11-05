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
#define QueueSize 100

#define LICENCE_PLATE_SIZE 6
#define LICENCE_PLATE_DIR "../data/plates.txt"

#define DEBUG 1

// variables for car queue
pthread_mutex_t queueEntry;
pthread_mutex_t queueExit;
int carLevelCount[LEVELS] = {0};


// ---------------------------------------------
// Methods in charge of random generation 
// ---------------------------------------------

// The random function needs to be thread safe when generating random numbers
// ---------------------------------------------
pthread_mutex_t rand_mutex;
int randThread(void){
    int randomNumber;
    pthread_mutex_lock(&rand_mutex);
    randomNumber = rand();
    pthread_mutex_unlock(&rand_mutex);
    return randomNumber;
}

// Random generator for picking a carpark level
// ---------------------------------------------
int GetRandLevel(void){
    int randomLevel;
    randomLevel = randThread() % LEVELS;
    if (carLevelCount[randomLevel] < QueueSize){
        return randomLevel;
    }
    else{
        //Loop through the levels to find a level with space
        for (int i = 0; i < LEVELS; i++){
            if (carLevelCount[i] < QueueSize){
                return i;
            }
        }
    }
    return -1;
}

//---------------------------------------------
// Methods in charge of the car queue
//---------------------------------------------
// Method to add a car to the queue


//---------------------------------------------
// Methods for the LPR sensor
//---------------------------------------------
// Initialise the LPR sensor
// void *initLPR(void *arg)
// {
// 	lpr_sensor_t *lpr = arg;

//     // check if mutex failed
//     if (pthread_mutex_init(&lpr->mutex, NULL) != 0)
//     {
//         printf("Mutex init failed");
//     }

//     // check if failed 
//     // if (pthread_mutex_init(&lpr->mutex, &mutexAttr) != 0) {
//     //     printf("LPR mutex init failed");
//     // }

//     //pthread_cond_init(&lpr->cond, &condAttr);
//     return NULL;
// }

//---------------------------------------------
// Methods for the information display
//---------------------------------------------
// Initialise the information display
void *initDisplay(void *arg)
{
    information_sign_t *display = arg;

    // check if failed 
    if (pthread_mutex_init(&display->mutex, NULL) != 0) {
        printf("Display mutex init failed");
    }
    return NULL;
}


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
        usleep(20 * MS_IN_MICROSECONDS);
    }
    // If the boom gate has been set to open
    else if (boom_gate->status == 'O'){
        //Print the boom gate status
    }
    // If the boom gate is set to lowering 
    else if (boom_gate->status == 'L'){
        // Lower the boom gate after 10ms
        usleep(10 * MS_IN_MICROSECONDS);
        boom_gate->status = 'C';
        // Broadcast the change to the boom gate
        pthread_cond_broadcast(&boom_gate->cond);
        //Print the boom gate status
    }
    // If the boom gate is set to closed
    else if (boom_gate->status == 'C'){
        //Print the boom gate status
    }
    // If the boom gate is set to an illegal status
    else{
        //Print the boom gate status
        printf("Boom gate is in an illegal state");
    }
    // Unlock the boom gate mutex
    pthread_mutex_unlock(&boom_gate->mutex);

}

// ---------------------------------------------
// Functions relating to the car               |
// ---------------------------------------------

// Function to get a plate from plates.txt
// ---------------------------------------------
char* GenerateLicencePlate(){
    // Allocate memory for the plate
    char *plate = malloc(LICENCE_PLATE_SIZE);
    FILE *fp = fopen(LICENCE_PLATE_DIR, "r");
    // Error check the file
    if (fp == NULL){
        printf("Error opening file");
    }
    // Pick a random line 
    int randomPlateLine = randThread()%100 + 1;
    // printf("Line: %d\n", randomPlateLine);
    for(int i = 0; i < randomPlateLine; i++){
        // TODO change plate to LICENCE_PLATE_SIZE
        fgets(plate, sizeof(plate)+1, fp);
    }
    fclose(fp);
    return plate;
}

/**
 * @brief Sends a car to the desired level
 *
 * @param entranceLevel
 * @param grantedLevel
 * @param LicensePlate
 * @param shm
 * @return int
 */
int sendCarToLevel(int8_t entranceLevel, char grantedLevel,
                   char *LicensePlate, shared_memory_data_t *shm)
{
    if (grantedLevel < 0)
    {
        printf("Display says X or F\n");
        return 0;
    }
    // Simulate boom gate to open
    // TODO: Make this a function
    shm->entrance[entranceLevel].boom_gate.status = 'R';
    printf("Boom gate raised on %d\n", entranceLevel + 1);
    printf("Boom gate closed on %d\n", entranceLevel + 1);
    shm->entrance[entranceLevel].boom_gate.status = 'L';

    printf("%s heading to level: %c\n", LicensePlate, grantedLevel + 1);

    // Exit the thread
    pthread_exit(NULL);
    return 1;
}

void ActivateLPR(int destinationLevel, char *licencePlate, shared_memory_data_t *p_shm)
{
    lpr_sensor_t *p_lpr = &p_shm->entrance[destinationLevel].lpr_sensor;
    memcpy(p_lpr->plate, licencePlate, LICENCE_PLATE_SIZE);
    pthread_cond_signal(&p_lpr->cond);
}

void WaitForLPRorGate(int destinationLevel, shared_memory_data_t *p_shm)
{
    lpr_sensor_t *p_lpr = &p_shm->entrance[destinationLevel].lpr_sensor;
    information_sign_t *p_info = &p_shm->entrance[destinationLevel].information_sign;

    // wait for LPR or gate to be free
    pthread_cond_wait(&p_info->cond, &p_info->mutex);
    printf("%s has been granted access to level %c\n", p_lpr->plate,
           p_shm->entrance[destinationLevel].information_sign.display + 1);
}

/**
 * @brief Car thread - generates a car and sends it to an entrance
 * In these steps:
 ** 1. Generate a random number plate
 ** 2. Generate a random level number
 ** 3. Send the car to the entrance (activate the level LPR)
 ** 4. wait for LPR to choose a level (and set it to its display)
 ** 5. Send the car through the boom gate (read display and send it to that level)
 *
 * @param shmCar pointer to the shared memory data
 * @return void* necessary for threading
 */
void *carThread(void *shmCar)
{
    shared_memory_data_t *p_shm = shmCar;

    //* step 1 - generate a random number plate
    char *plate = GenerateLicencePlate();
    char licencePlate[LICENCE_PLATE_SIZE];
    memcpy(licencePlate, plate, LICENCE_PLATE_SIZE);
    free(plate);

    //* step 2 - generate a random level number
    int destinationLevel = GetRandLevel();
    if (DEBUG)
        printf("%s has arrived at entrance %d\n", licencePlate, destinationLevel + 1);

    //* step 3 - send the car to the entrance (activate the level LPR)
    // Access the levels LPR sensor
    ActivateLPR(destinationLevel, licencePlate, p_shm);

    //* step 4 - wait for LPR to choose a level
    WaitForLPRorGate(destinationLevel, p_shm);

    //* step 5 - send the car through the boom gate
    sendCarToLevel(destinationLevel,
                   p_shm->entrance[destinationLevel].information_sign.display,
                   p_shm->entrance->lpr_sensor.plate, p_shm);
    return NULL;
}

// Generate a car every 1-100ms
void *generateCar(void *shm)
{
    // Wait 10 seconds before generating cars
    usleep(10000 * MS_IN_MICROSECONDS);
    printf("Generating cars\n");
    // Setting the maxmium amount of spawned cars before queue implementation!!!
    //! TODO: Implement queue
    int maximumCars = 10;
    pthread_t cars[maximumCars];
    for (int i = 0; i < maximumCars; i++)
    {
        pthread_create(&cars[i], NULL, carThread, shm);
        int time = (randThread() % (100 - 1 + 1)) + 1; // create random wait time between 1-100ms

        usleep(time * MS_IN_MICROSECONDS);
    }

    return NULL;
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
        pthread_mutex_init(&bg->mutex, &mutexAttr);
        pthread_cond_init(&bg->cond, &condAttr);
		pthread_create(boomgatethreads + i, NULL, boomgateInit,(void *) bg);
	}

    for (int i = 0; i < EXITS; i++) {
        boom_gate_t *bg = &shm.data->exit[i].boom_gate;
        pthread_mutex_init(&bg->mutex, &mutexAttr);
        pthread_cond_init(&bg->cond, &condAttr);
        pthread_create(boomgatethreads + i, NULL, boomgateInit,(void *) bg);
    }

    // Create threads for the LPR sensors
    //pthread_t *lprthreads = malloc(sizeof(pthread_t) * (ENTRANCES + EXITS));
    // For LPR entrance
    for (int i = 0; i < ENTRANCES; i++) {
        lpr_sensor_t *lpr = &shm.data->entrance[i].lpr_sensor;
        pthread_mutex_init(&lpr->mutex, &mutexAttr);
        pthread_cond_init(&lpr->cond, &condAttr);
        //pthread_create(lprthreads + i, NULL, initLPR,(void *) lpr);
    }
    // For LPR exit
    for (int i = 0; i < EXITS; i++) {
        lpr_sensor_t *lpr = &shm.data->exit[i].lpr_sensor;
        pthread_mutex_init(&lpr->mutex, &mutexAttr);
        pthread_cond_init(&lpr->cond, &condAttr);
        //pthread_create(lprthreads + i, NULL, initLPR,(void *) lpr);
    }
    // For LPR on each level 
    for (int i = 0; i < LEVELS; i++) {
        lpr_sensor_t *lpr = &shm.data->level[i].lpr_sensor;
        pthread_mutex_init(&lpr->mutex, &mutexAttr);
        pthread_cond_init(&lpr->cond, &condAttr);
        //pthread_create(lprthreads + i, NULL, initLPR,(void *) lpr);
    }

    // Create threads for the info signs
    pthread_t *infothreads = malloc(sizeof(pthread_t) * (ENTRANCES));
    // For info signs entrance
    for (int i = 0; i < ENTRANCES; i++) {
        information_sign_t *info = &shm.data->entrance[i].information_sign;
        pthread_mutex_init(&info->mutex, &mutexAttr);
        pthread_cond_init(&info->cond, &condAttr);
        pthread_create(infothreads + i, NULL, initDisplay,(void *) info);
    }


}

// main function 
int main(void)
{
    //system("./manager&");

    printf("Welcome to the simulator\n");
     //Set the seed of the random number generator
    srand(time(0));

    shared_memory_t shm;
    create_shared_object(&shm, "PARKING");
    setDefaults(shm);

    // create a thread to generate cars
    pthread_t carGen;
    // copy shm to a new pointer
    //shared_memory_t *shmCar = &shm;

    shared_memory_data_t *shmCar = shm.data;
    

    //lpr_sensor_t *lpr2 = &shm.data->entrance->lpr_sensor;
    pthread_create(&carGen, NULL, generateCar, (void *) shmCar);

    // Change level 4 LPR plate 

    // Put string into LPR sensor plate
    // char *plate = getPlate();
    // lpr_sensor_t *lpr = &shm.data->entrance[0].lpr_sensor;
    // memcpy(lpr->plate, plate, sizeof(char)*LICENCE_PLATE_SIZE);
    // printf("%s read into level LPR\n", lpr->plate);
    // free(plate);

    for (;;) {

        // // Close the boom gates for testing
        // for (int i = 0; i < ENTRANCES; i++) {
        //     shm.data->entrance[i].boom_gate.status = 'L';
        // }
        
        for(int i = 0; i < ENTRANCES; i++) {
            boom_gate_t *bg = &shm.data->entrance[i].boom_gate;

            if (bg->status == 'R'){
                pthread_t boomgatethread;
                pthread_create(&boomgatethread, NULL,(void *(*)(void *))boomGateSimualtor, (void *) bg);
                //pthread_join(boomgatethread, NULL);
            }
            else if (bg->status == 'L'){
                pthread_t boomgatethread;
                pthread_create(&boomgatethread, NULL,(void *(*)(void *))boomGateSimualtor, (void *) bg);
                //pthread_join(boomgatethread, NULL);
            }
        }


        usleep(5 * MS_IN_MICROSECONDS);

    }


}