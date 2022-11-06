#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem_init.h"

#define MS_IN_MICROSECONDS 1000

#define DEBUG 0

// Global variables
int parking[LEVELS];

// Set each parking to parkingcapactiy
void setupParking()
{
    for (int i = 0; i < LEVELS; i++)
    {
        parking[i] = PARKING_CAPACITY;
    }
}

// Function to check car parking availability
int check_parking_availability(int level)
{
    if (parking[level] > 0)
    {
        return 1;
    }
    return 0;
}

// Function to check if all levels are full
int check_all_levels_full()
{
    for (int i = 0; i < LEVELS; i++)
    {
        if (parking[i] > 0)
        {
            return 0;
        }
    }
    return 1;
}

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

// Save shm and gate number in a struct
//! TODO change shm to a pointer
typedef struct gate_data
{
    shared_memory_t shm;
    int gate;
} gate_data_t;

void *lprEntranceHandler(void *arg)
{
    gate_data_t *gate_data = arg;
    shared_memory_t shm = gate_data->shm;
    int gate = gate_data->gate;

    lpr_sensor_t *lpr = &shm.data->entrance[gate].lpr_sensor;

    for (;;)
    {
        // Wait for condition
        pthread_mutex_lock(&lpr->mutex);
        pthread_cond_wait(&lpr->cond, &lpr->mutex);
        pthread_mutex_unlock(&lpr->mutex);

        // Set info sign to 4 testing
        pthread_mutex_lock(&shm.data->entrance[gate].information_sign.mutex);

        // Check if car park is full
        if (check_all_levels_full())
        {
            // shm.data->entrance[gate].information_sign.display = 'F';
            strcpy(&shm.data->entrance[gate].information_sign.display, "F");
            pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
            return NULL;
        }
        else
        {
            for (;;)
            {
                // Pick a random level to park on
                int levelToPark = randThread() % LEVELS;
                printf("Super Duper genertaed level: %d\n", levelToPark);

                // Check parking availability for every level
                for (int i = 0; i < 5; i++)
                {
                    int CanPark = check_parking_availability(i);
                    if (DEBUG)
                        printf("CanPark: %d\n", CanPark);
                }

                // Check if parking is available
                if (check_parking_availability(levelToPark))
                {
                    // Remove one from parking
                    parking[levelToPark] = parking[levelToPark] - 1;
                    // Make char of level
                    // char level_char = level + '0';

                    // print the level
                    printf("Generated level : %d\n", levelToPark);

                    // Level 0, level 1, level 2, level 3, level 4
                    // level+1

                    if (levelToPark == 0)
                    {
                        strcpy(&shm.data->entrance[gate].information_sign.display, "1");
                        pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
                        // print the level
                        printf("Level 1 chosen: %d\n", levelToPark);
                        break;
                    }
                    else if (levelToPark == 1)
                    {
                        strcpy(&shm.data->entrance[gate].information_sign.display, "2");
                        pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
                        // print the level
                        printf("Level 2 chosen: %d\n", levelToPark);
                        break;
                    }
                    else if (levelToPark == 2)
                    {
                        strcpy(&shm.data->entrance[gate].information_sign.display, "3");
                        pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
                        // print the level
                        printf("Level 3 chosen: %d\n", levelToPark);

                        break;
                    }
                    else if (levelToPark == 3)
                    {
                        strcpy(&shm.data->entrance[gate].information_sign.display, "4");
                        pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
                        // print the level
                        printf("Level 4 chosen: %d\n", levelToPark);
                        break;
                    }
                    else if (levelToPark == 4)
                    {
                        strcpy(&shm.data->entrance[gate].information_sign.display, "5");
                        pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
                        // print the level
                        printf("Level 5 chosen: %d", 4);
                        printf(" for ");
                        
                        for (int i = 0; i < 6; i++)
                        {
                            printf("%s",  &shm.data->entrance[4].lpr_sensor.plate[i]);
                        }
                        break;
                    }
                }
            }
        }

        // pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);
        pthread_cond_signal(&shm.data->entrance[gate].information_sign.cond);
    }

    return NULL;
}

void display(shared_memory_t shm)
{
    // Clear the console each loop
    system("clear");

    // Print the status of the boom gates
    printf("Status of the boom gates\n");
    printf("Entrances           Exits\n");
    for (int i = 0; i < ENTRANCES; i++)
    {
        printf("%d: %c                %d: %c\n", i + 1, shm.data->entrance[i].boom_gate.status, i + 1, shm.data->exit[i].boom_gate.status);
    }
    printf("\n");
    // Print the status of the LPR sensors
    printf("Status of the LPR sensors\n");
    printf("Entrances           Exits           Level\n");
    for (int i = 0; i < ENTRANCES; i++)
    {
        if (strcmp(shm.data->entrance[i].lpr_sensor.plate, "") == 0)
        {
            printf("%d: %s\t\t%d: %s\t\t%d: %s\n", i + 1, shm.data->entrance[i].lpr_sensor.plate, i + 1, shm.data->exit[i].lpr_sensor.plate, i + 1, shm.data->level[i].lpr_sensor.plate);
        }
        else
        {
            printf("%d: %s\t%d: %s\t\t%d: %s\n", i + 1, shm.data->entrance[i].lpr_sensor.plate, i + 1, shm.data->exit[i].lpr_sensor.plate, i + 1, shm.data->level[i].lpr_sensor.plate);
        }
    }
    printf("\n");
    // Print the status of the information signs
    printf("Status of the information signs\n");
    for (int i = 0; i < ENTRANCES; i++)
    {
        printf("%d: %c  ", i + 1, shm.data->entrance[i].information_sign.display + 1);
    }
    printf("\n");
    usleep(50 * MS_IN_MICROSECONDS);
}

int main(void)
{
    //! TODO replace with time(0) or time(NULL) for seed
    srand(69);
    setupParking();
    // // Sleep for abit
    // usleep(2000 * MS_IN_MICROSECONDS);

    //  create the shared memory segment
    shared_memory_t shm;
    get_shared_object(&shm, "PARKING"); // Need to error handle this

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

    // LPR memory
    // Create a thead for each Entrance LPR
    for (int i = 0; i < ENTRANCES; i++)
    {
        pthread_create(&lprThreadEntrances[i], NULL, lprEntranceHandler, (void *)(&entranceData[i]));
    }

    for (;;)
    {
        display(shm);
    }
}
