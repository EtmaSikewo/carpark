#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "mem_init.h"

// Definitions
#define TIME_SCALE 1
#define MS_IN_MICROSECONDS 1000 * TIME_SCALE
#define QueueSize 100
#define MAX_CARS 400

#define DEBUG 0

// ---------------------------------------------
// Global variables for the carpark simulator
// variables for car queue
pthread_mutex_t queueEntry;
pthread_mutex_t queueExit;
int carLevelCount[LEVELS] = {0};

// variables for fire alarm
bool TriggerAlarmFixed = false;
bool TriggerAlarmRateOfRise = false;

// ---------------------------------------------
// Methods in charge of random generation
// ---------------------------------------------

// The random function needs to be thread safe when generating random numbers
// ---------------------------------------------
pthread_mutex_t rand_mutex;
int randThread(void)
{
    int randomNumber;
    pthread_mutex_lock(&rand_mutex);
    randomNumber = rand();
    pthread_mutex_unlock(&rand_mutex);
    return randomNumber;
}

// Random generator for picking a carpark level
// ---------------------------------------------
int randLevel(void)
{
    int randomLevel;
    randomLevel = randThread() % LEVELS;
    if (carLevelCount[randomLevel] < QueueSize)
    {
        return randomLevel;
    }
    else
    {
        // Loop through the levels to find a level with space
        for (int i = 0; i < LEVELS; i++)
        {
            if (carLevelCount[i] < QueueSize)
            {
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
    if (pthread_mutex_init(&display->mutex, NULL) != 0)
    {
        printf("Display mutex init failed");
    }
    return NULL;
}

// ---------------------------------------------
// Functions for the fire alarm
// ---------------------------------------------

// Structure to hold the shared memory and level of the fire alarm
typedef struct fireAlarm
{
    shared_memory_t shm;
    int level;
} fireAlarm_t;

void *TemperatureSensor(void *arg)
{
    // Get the level from the fireAlarm struct
    // fireAlarm *fire = arg;
    // int level = fire->level;
    // shared_memory_data_t *data = &fire->shm;

    // Get the shared memory data from arg
    shared_memory_data_t *data = arg;

    int temperature;
    for (;;)
    {
        // Loop through the levels
        for (int i = 0; i < LEVELS; i++)
        {

            if (TriggerAlarmFixed)
            {
                // Set the fire alarm to true
                data->level[i].temperature_sensor = 60;
            }
            else if (TriggerAlarmRateOfRise)
            {
                // If value is not higher than 500
                if (data->level[i].temperature_sensor < 500)
                {
                    // 10% chance of raising the temperature by 1
                    if (randThread() % 10 == 0)
                    {
                        data->level[i].temperature_sensor += 1;
                    }
                }
            }
            else
            {
                // Generate a temperature between 28 and 32
                temperature = randThread() % 5 + 28;
                // Set the temperature in the shared memory
                data->level[i].temperature_sensor = temperature;
            }

            // Print the temperature
            // printf("Temperature sensor: Level %d, Temperature: %d\n", i, data->level[i].temperature_sensor);
        }
        // Wait 2ms
        usleep(2 * MS_IN_MICROSECONDS);
    }
}

// ---------------------------------------------
// Functions relating to the boomgate          |
// ---------------------------------------------

// Function to initalise the boom gate
// ---------------------------------------------
void *boomgateInit(void *arg)
{
    boom_gate_t *boom_gate = arg;

    // Error check the mutex and condition variable
    if (pthread_mutex_init(&boom_gate->mutex, NULL) != 0)
    {
        printf("Boom gate mutex init failed");
    }

    // Lock the mutex
    pthread_mutex_lock(&boom_gate->mutex);
    // Set the boom gate to closed
    boom_gate->status = 'C';
    // Broadcast the condition variable
    pthread_cond_broadcast(&boom_gate->cond);
    // Unlock the mutex
    pthread_mutex_unlock(&boom_gate->mutex);
    return NULL;
    pthread_exit(NULL);
}

// Function that will be called by the boom gate thread
// ---------------------------------------------
void *boomGateSimualtor(void *arg)
{
    // Get the boom gate thread
    boom_gate_t *boom_gate = arg;
    // Lock the boomgate mutex for use
    pthread_mutex_lock(&boom_gate->mutex);
    // Logic for the boom gate
    //------------------------
    // If the boomgate has been set to raise
    if (boom_gate->status == 'R')
    {
        // Raise the boom gate after 10ms
        usleep(10 * MS_IN_MICROSECONDS);
        boom_gate->status = 'O';
        // Broadcast the change to the boom gate
        pthread_cond_broadcast(&boom_gate->cond);
        // Print the boom gate status
        if (DEBUG)
            printf("Boom gate: Raised\n");
        usleep(20 * MS_IN_MICROSECONDS);
    }
    // If the boom gate has been set to open
    else if (boom_gate->status == 'O')
    {
        // Print the boom gate status
    }
    // If the boom gate is set to lowering
    else if (boom_gate->status == 'L')
    {
        // Lower the boom gate after 10ms
        usleep(10 * MS_IN_MICROSECONDS);
        boom_gate->status = 'C';
        // Broadcast the change to the boom gate
        pthread_cond_broadcast(&boom_gate->cond);
        // Print the boom gate status
        if (DEBUG)
            printf("Boom gate: Lowered\n");
        usleep(20 * MS_IN_MICROSECONDS);
    }
    // If the boom gate is set to closed
    else if (boom_gate->status == 'C')
    {
        // print the boom gate status
        if (DEBUG)
            printf("Boom gate: Closed\n");
    }
    // If the boom gate is set to an illegal status
    else
    {
        // Print the boom gate status
        printf("Boom gate is in an illegal state");
        pthread_exit(NULL);
    }
    // Unlock the boom gate mutex
    pthread_mutex_unlock(&boom_gate->mutex);

    // Exit the thread
    pthread_exit(NULL);
    return NULL;
}

// ---------------------------------------------
// Functions relating to the car               |
// ---------------------------------------------

// Function to get a plate from plates.txt
// ---------------------------------------------
char *getPlate()
{
    // Allocate memory for the plate
    char *plate = malloc(6);
    FILE *fp = fopen(PLATES_DIR, "r");
    // Error check the file
    if (fp == NULL)
    {
        printf("Error opening file");
    }
    // Pick a random line
    int randomPlateLine = randThread() % 100 + 1;
    // printf("Line: %d\n", randomPlateLine);
    for (int i = 0; i < randomPlateLine; i++)
    {
        fgets(plate, sizeof(plate) + 1, fp);
    }
    fclose(fp);

    // 50% chance of making a random plate
    if(randThread() % 2 == 0){
        // Erase plate 
        memset(plate, 0, 6);
        // Generate 3 random numbers between 0 and 9
        int j = 0;
        for (j = 0; j < 3; j++) {
            plate[j] = rand() % 10 + '0';
        }
        for (j = 3; j < 6; j++) {
            plate[j] = rand() % 26 + 'A';
        }
    }

    return plate;
}

// Return 0 if failed, return 1 if success
int sendCarToLevel(int8_t entranceLevel, char grantedLevel, char *LicensePlate, shared_memory_data_t *shm)
{

    // Simulate boom gate to open
    // TODO: Make this a function
    // Lock the boom gate mutex
    pthread_mutex_lock(&shm->entrance[entranceLevel].boom_gate.mutex);

    shm->entrance[entranceLevel].boom_gate.status = 'R';
    printf("Boom gate raised on entrance %d\n", entranceLevel + 1);

    // Wait to open boom gate
    pthread_cond_wait(&shm->entrance[entranceLevel].boom_gate.cond, &shm->entrance[entranceLevel].boom_gate.mutex);

    printf("Boom gate closed on entrance %d\n", entranceLevel + 1);
    shm->entrance[entranceLevel].boom_gate.status = 'L';

    // Wait to close

    printf("%s heading to level: %c\n", LicensePlate, grantedLevel);

    pthread_mutex_unlock(&shm->entrance[entranceLevel].boom_gate.mutex);
    return 1;
}

int sendCarToExit(int8_t exitLevel, char *LicensePlate, shared_memory_data_t *shm)
{

    // Simulate boom gate to open
    // TODO: Make this a function
    pthread_mutex_lock(&shm->exit[exitLevel].boom_gate.mutex);
    shm->exit[exitLevel].boom_gate.status = 'R';
    printf("Boom gate raised on exit %d\n", exitLevel + 1);

    // Wait to open boom gate
    pthread_cond_wait(&shm->exit[exitLevel].boom_gate.cond, &shm->exit[exitLevel].boom_gate.mutex);

    printf("Boom gate closed on exit %d\n", exitLevel + 1);
    shm->exit[exitLevel].boom_gate.status = 'L';

    pthread_mutex_unlock(&shm->exit[exitLevel].boom_gate.mutex);

    // send signal to exitLPR handler to minus one from the capacity
    pthread_cond_signal(&shm->exit[exitLevel].lpr_sensor.cond);

    return 1;
}

// TERRIBLE CAR GENERATOR FOR NOW!
// ---------------------------------------------
void *carThread(void *shmCar)
{
    // Grab shared memory
    shared_memory_data_t *shm = shmCar;
    // Grab a random license plate from the text file
    //! TODO: Make this generate a random license plate
    char *plate = getPlate();
    char LicensePlate[6];
    memcpy(LicensePlate, plate, 6);
    free(plate);
    // Grab a random level
    int level = randLevel();
    // Grab a random exit level
    int exitLevel = level;
    // Grab a random time to wait
    //! TODO TIMINGS
    int waitTime = randThread() % 10000 + 100;

    // print car details
    printf("%s has arrived at entrance %d\n", LicensePlate, level + 1);

    // Access the levels LPR sensor
    lpr_sensor_t *lpr = &shm->entrance[level].lpr_sensor;

    // Lock the LPR mutex
    // pthread_mutex_lock(&lpr->mutex);

    memcpy(lpr->plate, LicensePlate, sizeof(LicensePlate));
    // strcpy(lpr->plate, LicensePlate);

    // Broadcast the condition variable
    // pthread_cond_broadcast(&lpr->cond);
    // Signal the LPR sensor

    pthread_cond_signal(&lpr->cond);
    // broadcast the condition variable
    // pthread_cond_broadcast(&lpr->cond);

    // pthread_mutex_unlock(&lpr->mutex);

    // TO THIS POINT

    // Wait for the info sign to say the car can enter
    information_sign_t *info = &shm->entrance[level].information_sign;
    pthread_mutex_lock(&info->mutex);
    pthread_cond_wait(&info->cond, &info->mutex);

    if (info->display != '1' && info->display != '2' && info->display != '3' && info->display != '4' && info->display != '5')
    {
        // Print the rejection message
        printf("%s has been rejected\n", lpr->plate);
        // Unlock the mutex
        pthread_mutex_unlock(&info->mutex);
        // Exit the thread
        return NULL;
    }

    printf("%s has been granted access to level %c\n", lpr->plate, info->display);

    exitLevel = atoi(&info->display) - 1;

    pthread_mutex_unlock(&info->mutex);

    sendCarToLevel(level, info->display, lpr->plate, shm);

    //! TODO TIMINGS
    // trigger level LPR
    lpr_sensor_t *levelLpr = &shm->level[level].lpr_sensor;
    memcpy(levelLpr->plate, LicensePlate, 6);

    // Leave car park after wait time
    usleep(waitTime * MS_IN_MICROSECONDS);
    printf("%s is leaving through exit %d\n", LicensePlate, exitLevel + 1);
    // Trigger exit LPR
    lpr_sensor_t *exitLPR = &shm->exit[exitLevel].lpr_sensor;
    memcpy(exitLPR->plate, LicensePlate, 6);
    
    if (DEBUG)
        printf("%s took %dms\n", LicensePlate, waitTime);

    sendCarToExit(exitLevel, LicensePlate, shm);

    return NULL;
}

// Generate a car every 1-100ms
void *generateCar(void *shm)
{
    // Wait 6 seconds before generating cars
    usleep(6000 * 1000);
    printf("Generating cars\n");
    // Setting the maxmium amount of spawned cars before queue implementation!!!
    //! TODO: Implement queue
    pthread_t cars[MAX_CARS];
    for (int i = 0; i < MAX_CARS; i++)
    {
        pthread_create(&cars[i], NULL, carThread, shm);
        // int time = (randThread() % (1000 - 1 + 1)) + 1; // create random wait time between 1-100ms
        int time = randThread() % 100 + 1; // create random wait time between 1-100ms

        usleep(time * MS_IN_MICROSECONDS);
    }

    return NULL;
}

// ---------------------------------------------
// Set defaults
// ---------------------------------------------
// create a function that sets the default value for boom gate
void setBoomGateStatus(boom_gate_t *boom_gate, char status)
{
    boom_gate->status = status;
}

void setDefaults(shared_memory_t shm)
{

    pthread_attr_t pthreadAttr;
    pthread_mutexattr_t mutexAttr;
    pthread_condattr_t condAttr;
    pthread_attr_init(&pthreadAttr);
    pthread_mutexattr_init(&mutexAttr);
    pthread_condattr_init(&condAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);

    pthread_t *boomgatethreads = malloc(sizeof(pthread_t) * (ENTRANCES + EXITS));

    for (int i = 0; i < ENTRANCES; i++)
    {
        boom_gate_t *bg = &shm.data->entrance[i].boom_gate;
        pthread_mutex_init(&bg->mutex, &mutexAttr);
        pthread_cond_init(&bg->cond, &condAttr);
        pthread_create(boomgatethreads + i, NULL, boomgateInit, (void *)bg);
    }

    for (int i = 0; i < EXITS; i++)
    {
        boom_gate_t *bg = &shm.data->exit[i].boom_gate;
        pthread_mutex_init(&bg->mutex, &mutexAttr);
        pthread_cond_init(&bg->cond, &condAttr);
        pthread_create(boomgatethreads + i, NULL, boomgateInit, (void *)bg);
    }

    // Create threads for the LPR sensors
    // pthread_t *lprthreads = malloc(sizeof(pthread_t) * (ENTRANCES + EXITS));
    // For LPR entrance
    for (int i = 0; i < ENTRANCES; i++)
    {
        lpr_sensor_t *lpr = &shm.data->entrance[i].lpr_sensor;
        pthread_mutex_init(&lpr->mutex, &mutexAttr);
        pthread_cond_init(&lpr->cond, &condAttr);
        // pthread_create(lprthreads + i, NULL, initLPR,(void *) lpr);
    }
    // For LPR exit
    for (int i = 0; i < EXITS; i++)
    {
        lpr_sensor_t *lpr = &shm.data->exit[i].lpr_sensor;
        pthread_mutex_init(&lpr->mutex, &mutexAttr);
        pthread_cond_init(&lpr->cond, &condAttr);
        // pthread_create(lprthreads + i, NULL, initLPR,(void *) lpr);
    }
    // For LPR on each level
    for (int i = 0; i < LEVELS; i++)
    {
        lpr_sensor_t *lpr = &shm.data->level[i].lpr_sensor;
        pthread_mutex_init(&lpr->mutex, &mutexAttr);
        pthread_cond_init(&lpr->cond, &condAttr);
        // pthread_create(lprthreads + i, NULL, initLPR,(void *) lpr);
    }

    // Create threads for the info signs
    pthread_t *infothreads = malloc(sizeof(pthread_t) * (ENTRANCES));
    // For info signs entrance
    for (int i = 0; i < ENTRANCES; i++)
    {
        information_sign_t *info = &shm.data->entrance[i].information_sign;
        pthread_mutex_init(&info->mutex, &mutexAttr);
        pthread_cond_init(&info->cond, &condAttr);
        pthread_create(infothreads + i, NULL, initDisplay, (void *)info);
    }
}

// main function
int main(void)
{
    // system("./manager&");

    printf("Welcome to the simulator\n");
    // Set the seed of the random number generator
    srand(time(0));

    shared_memory_t shm;
    if (!create_shared_object(&shm, "PARKING"))
    {
        printf("Failed to create shared object\n");
        return 1;
    }
    setDefaults(shm);

    // create a thread to generate cars
    pthread_t carGen;
    // copy shm to a new pointer
    // shared_memory_t *shmCar = &shm;

    shared_memory_data_t *shmCar = shm.data;
    shared_memory_data_t *shmFire = shm.data;

    // lpr_sensor_t *lpr2 = &shm.data->entrance->lpr_sensor;
    pthread_create(&carGen, NULL, generateCar, (void *)shmCar);

    // Create threads for temperature sensor
    // fireAlarm_t fire[LEVELS];

    // Make one thread of temperature sensor
    pthread_t firethreads;
    pthread_create(&firethreads, NULL, TemperatureSensor, (void *)shmFire);

    // pthread_t fireThreads[LEVELS];

    // Change level 4 LPR plate

    // Put string into LPR sensor plate
    // char *plate = getPlate();
    // lpr_sensor_t *lpr = &shm.data->entrance[0].lpr_sensor;
    // memcpy(lpr->plate, plate, sizeof(char)*6);
    // printf("%s read into level LPR\n", lpr->plate);
    // free(plate);

    for (;;)
    {

        // Trigger a fire randomly 
        if ((!TriggerAlarmFixed && !TriggerAlarmRateOfRise)) {
            // Randomly trigger a fire in 0.1% of the time
            if (randThread() % 10000 == 0) {
                // RAndomly choose a fire alarm to trigger
                int fireAlarm = rand() % 2;
                if (fireAlarm == 0)
                {
                    TriggerAlarmFixed = true;
                    printf("Fire simulated fixed\n");
                }
                else
                {
                    TriggerAlarmRateOfRise = true;
                    printf("Fire simulated rise\n");
                }
            }
        }

        // // Close the boom gates for testing
        // for (int i = 0; i < ENTRANCES; i++) {
        //     shm.data->entrance[i].boom_gate.status = 'L';
        // }

        for (int i = 0; i < ENTRANCES; i++)
        {
            boom_gate_t *bg = &shm.data->entrance[i].boom_gate;

            if (bg->status == 'R')
            {
                pthread_t boomgatethread;
                pthread_create(&boomgatethread, NULL, (void *(*)(void *))boomGateSimualtor, (void *)bg);
            }
            else if (bg->status == 'L')
            {
                pthread_t boomgatethread;
                pthread_create(&boomgatethread, NULL, (void *(*)(void *))boomGateSimualtor, (void *)bg);
            }
        }

        for (int i = 0; i < EXITS; i++)
        {
            boom_gate_t *bg = &shm.data->exit[i].boom_gate;

            if (bg->status == 'R')
            {
                pthread_t boomgatethread;
                pthread_create(&boomgatethread, NULL, (void *(*)(void *))boomGateSimualtor, (void *)bg);
                // pthread_join(boomgatethread, NULL);
            }
            else if (bg->status == 'L')
            {
                pthread_t boomgatethread;
                pthread_create(&boomgatethread, NULL, (void *(*)(void *))boomGateSimualtor, (void *)bg);
                // pthread_join(boomgatethread, NULL);
            }
        }

        usleep(5 * MS_IN_MICROSECONDS);
    }

    destroy_shared_object(&shm);
}