#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "billing.h"
// #include <time.h>
// #include <pthread.h>
// #include <sys/mman.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <fcntl.h>
#include "mem_init.h"
#include <sys/time.h>

#define MS_IN_MICROSECONDS 1000
#define CENTS_PER_MS 0.05


// Global variables
int parking[LEVELS]; 
float totalBilling = 0;

// Set each parking to parkingcapactiy
void setupParking(){
    for (int i = 0; i < LEVELS; i++) {
        parking[i] = PARKING_CAPACITY;
    }
}

// Function to check car parking availability
int check_parking_availability(int level) {
    if (parking[level] > 0) {
        return 1;
    }
    return 0;
}

// Function to check if all levels are full
int check_all_levels_full() {
    for (int i = 0; i < LEVELS; i++) {
        if (parking[i] > 0) {
            return 0;
        }
    }
    return 1;
}


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
// int randLevel(void){
//     int randomLevel;
//     randomLevel = randThread() % LEVELS;
//     // Print level
//     printf("Super Duper genertaed level: %d\n", randomLevel);
//     return randomLevel;
// }



// Save shm and gate number in a struct
typedef struct gate_data
{
    shared_memory_t shm;
    int gate;
} gate_data_t;


typedef struct car
{
    char plate[LICENCE_PLATE_SIZE];
    struct timeval entryTime;
} car_t;

car_t car_list[PARKING_CAPACITY * LEVELS + 10];

void InitCarCapacity()
{
    for (int i = 0; i < PARKING_CAPACITY * LEVELS + 10; i++)
    {
        for (int j = 0; j < LICENCE_PLATE_SIZE; j++)
        {
            memset(&car_list[i].plate[0], '$', LICENCE_PLATE_SIZE);
        }
    }
}

void AddCarToCapacity(char *licencePlate)
{
    for (int i = 0; i < PARKING_CAPACITY * LEVELS + 10; i++)
    {
        // find the next available cell in the list
        // if the plate is '$'
        if (car_list[i].plate[0] == '$')
        {
            strcpy(car_list[i].plate, licencePlate);
            gettimeofday(&car_list[i].entryTime, NULL);
            break;
        }
    }
}

// void *SetCarStayDuration(void *argies)
// {
//     //  set the initial values
//     car_field_t *car_field = argies;
//     lpr_sensor_t *p_entrance_lpr = &car_field->p_entrance_lpr;
//     lpr_sensor_t *p_exit_lpr = &car_field->p_exit_lpr;


//     // set the start time
//     struct timeval exitTime;

//     pthread_mutex_lock(&p_entrance_lpr->mutex);
//     pthread_cond_wait(&p_entrance_lpr->cond, &p_entrance_lpr->mutex);
//     pthread_mutex_unlock(&p_entrance_lpr->mutex);

//     return NULL;
// }

int DoesCarExist(char *licencePlate)
{
    // loop through car_list
    for (int i = 0; i < PARKING_CAPACITY * LEVELS + 10; i++)
    {
        if (strcmp(car_list[i].plate, licencePlate) == 0)
        {
            // car exists
            return 1;
        }
    }
    return 0;
}



void *lprEntranceHandler(void *arg)
{
    gate_data_t *gate_data = arg;
    shared_memory_t shm = gate_data->shm;
    int gate = gate_data->gate;
    //printf("lprEntranceHandler: gate %d\n", gate);

    lpr_sensor_t *lpr = &shm.data->entrance[gate].lpr_sensor;
    int levelToPark = 0;
    char plate[6] = "      ";
    strcpy(plate, lpr->plate);

    
    // Lock mutex
    //pthread_mutex_lock(&lpr->mutex);

    for (;;){
        // Wait for condition
        pthread_mutex_lock(&lpr->mutex);
        pthread_cond_wait(&lpr->cond, &lpr->mutex);
        pthread_mutex_unlock(&lpr->mutex);
        // Pick a random level to park on 
    
        // Set info sign to 4 testing
        pthread_mutex_lock(&shm.data->entrance[gate].information_sign.mutex);

        // Check if car park is full 
        if (check_all_levels_full()) {
            //shm.data->entrance[gate].information_sign.display = 'F';
            strcpy(&shm.data->entrance[gate].information_sign.display, "F");
            pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
            pthread_cond_signal(&shm.data->entrance[gate].information_sign.cond);
        }
        else if (DoesCarExist(plate))
        {
            strcpy(&shm.data->entrance[gate].information_sign.display, "X");
            pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
            pthread_cond_signal(&shm.data->entrance[gate].information_sign.cond);
        }

        else
        {
            for (;;)
            {
                levelToPark = randThread() % LEVELS;
                // Check if parking is available
                if (check_parking_availability(levelToPark))
                {
                    // Remove one from parking
                    parking[levelToPark] = parking[levelToPark] - 1;
                    // Make char of level
                    // char level_char = level + '0';

                    // print the level

                    // Level 0, level 1, level 2, level 3, level 4
                    // level+1

                    if (levelToPark == 0)
                    {
                        strcpy(&shm.data->entrance[gate].information_sign.display, "1");
                        pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
                        // print the level
                        break;
                    }
                    else if (levelToPark == 1)
                    {
                        strcpy(&shm.data->entrance[gate].information_sign.display, "2");
                        pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
                        break;
                    }
                    else if (levelToPark == 2)
                    {
                        strcpy(&shm.data->entrance[gate].information_sign.display, "3");
                        pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);

                        break;
                    }
                    else if (levelToPark == 3)
                    {
                        strcpy(&shm.data->entrance[gate].information_sign.display, "4");
                        pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
                        break;
                    }
                    else if (levelToPark == 4)
                    {
                        strcpy(&shm.data->entrance[gate].information_sign.display, "5");
                        pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
                        break;
                    }
                }
            }
            pthread_mutex_lock(&shm.data->entrance[gate].lpr_sensor.mutex);
            AddCarToCapacity(shm.data->entrance[gate].lpr_sensor.plate);
            pthread_mutex_unlock(&shm.data->entrance[gate].lpr_sensor.mutex);
        }

        //pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);

        pthread_cond_signal(&shm.data->entrance[gate].information_sign.cond);

    }

    return NULL;
}


void *lprExitHandler(void *arg) {
    gate_data_t *gate_data = arg;
    shared_memory_t shm = gate_data->shm;
    int gate = gate_data->gate;

    lpr_sensor_t *lpr = &shm.data->exit[gate].lpr_sensor;
    struct timeval entryTime, exitTime;
    
    for (;;) {
        pthread_mutex_lock(&lpr->mutex);
        pthread_cond_wait(&lpr->cond, &lpr->mutex);
        parking[gate]++;

        // loop through the car list and check if the car is in the list
        for (int i = 0; i < PARKING_CAPACITY * LEVELS + 10; i++)
        {
            if (strcmp(car_list[i].plate, lpr->plate) == 0)
            {
                // if the car is in the list, remove it from the list
                car_list[i].plate[0] = '$';
                gettimeofday(&exitTime, NULL);
                entryTime = car_list[i].entryTime;
                break;
            }
        }
        // calculate the time difference
        int timeDiff = (exitTime.tv_sec - entryTime.tv_sec) * 1000 + (exitTime.tv_usec - entryTime.tv_usec) / 1000;
        // print the time difference
        // printf("Time difference: %d\n", timeDiff);
        printf("%s stayed for %dms\n", lpr->plate, timeDiff);
        pthread_mutex_unlock(&lpr->mutex);
        
        
        // long milliseconds = (exitTime.tv_sec - entryTime.tv_sec) * 1000 + exitTime.tv_usec - entryTime.tv_usec;
        // printf("%s stayed for %lims\n", lpr->plate, milliseconds);
        // pthread_mutex_unlock(&lpr->mutex);
        totalBilling += timeDiff * CENTS_PER_MS;
    }

    return NULL;
}

void display(shared_memory_t shm){     
        // ---------------------------------------------
        // Template for the manager UI 
        // ---------------------------------------------

        /*
        Status of the boom gates
        Entrances           Exits
        1: C                1: C
        2: C                2: C
        3: C                3: C
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
        //Clear the console each loop
        system("clear");
        
        //Print the status of the boom gates
        printf("Status of the boom gates\n");
        printf("Entrances           Exits\n");
        for(int i = 0; i < ENTRANCES; i++){
            printf("%d: %c\t\t%d: %c\n", i+1, shm.data->entrance[i].boom_gate.status, i+1, shm.data->exit[i].boom_gate.status);
        }
        printf("\n");
        //Print the status of the LPR sensors
        printf("Status of the LPR sensors\n");
        printf("Entrances\tExits\t\tLevel\n");
        for(int i = 0; i < ENTRANCES; i++){
            printf("%d: %-12s %d: %-12s %d: %-6s\n", i+1, shm.data->entrance[i].lpr_sensor.plate, i+1, shm.data->exit[i].lpr_sensor.plate, i+1, shm.data->level[i].lpr_sensor.plate);
        }
        printf("\n");
        
        //Print the status of the information signs
        printf("Status of the information signs\n");
        for(int i = 0; i < ENTRANCES; i++){
            printf("%d: %-3c  ", i+1, shm.data->entrance[i].information_sign.display);
        }
        printf(" \n\n");

        // Print the parking information
        printf("Level information\n");
        for(int i = 0; i < LEVELS; i++){
            if (parking[i] == 0) {
                printf("level %d: %d/%d\t%d°C |FULL|\n", i+1, abs(parking[i]-PARKING_CAPACITY), PARKING_CAPACITY, shm.data->level[i].temperature_sensor);
            }
            else {
                printf("level %d: %d/%d\t%d°C\n", i+1, abs(parking[i]-PARKING_CAPACITY), PARKING_CAPACITY, shm.data->level[i].temperature_sensor);
            }   
        }
        
        // print the billing information
        printf("\nTotal Billing:\t$%.2f\n", totalBilling);

        printf("\n");
        usleep(50 * MS_IN_MICROSECONDS);
    }



int main(void)
{

    srand(time(0));
    setupParking();
    // // Sleep for abit 
    // usleep(2000 * MS_IN_MICROSECONDS);

    //  create the shared memory segment
    shared_memory_t shm;
    get_shared_object(&shm, "PARKING"); // Need to error handle this
    InitCarCapacity();
    
    // Structure to hold the entrance number and the shared memory
    // struct entranceNumAndShm{
    //     int entranceNum;
    //     shared_memory_t shm;
    // };

    gate_data_t entranceData[ENTRANCES];
    for (int i = 0; i < ENTRANCES; i++)
    {
        entranceData[i].shm = shm;
        entranceData[i].gate = i;
    }

    // Thread for the entranceLPRSensors
    pthread_t lprThreadEntrances[ENTRANCES];
    pthread_t lprThreadExits[EXITS]; 

    // LPR memory 
    //Create a thead for each Entrance LPR
    for (int i = 0; i < ENTRANCES; i++){
        pthread_create(&lprThreadEntrances[i], NULL, lprEntranceHandler, (void *) (&entranceData[i]));
    }

    for (int i = 0; i < EXITS; i++){
        pthread_create(&lprThreadExits[i], NULL, lprExitHandler, (void *) (&entranceData[i]));
    }

    
    for (;;){
        display(shm);
    }
   

}

        
    





