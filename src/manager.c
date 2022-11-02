#include <stdio.h>
#include<stdlib.h>
// #include <time.h>
// #include <pthread.h>
// #include <sys/mman.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <fcntl.h>
#include "mem_init.h"

#define MS_IN_MICROSECONDS 1000

// Save shm and gate number in a struct
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
    //printf("lprEntranceHandler: gate %d\n", gate);

    lpr_sensor_t *lpr = &shm.data->entrance[gate].lpr_sensor;
    //boom_gate_t *boom_gate = &entrance->boom_gate;
    //information_sign_t *information_sign = &shm.data->entrance[gate].information_sign;
    //information_sign->display = '4';

    
    // Lock mutex
    //pthread_mutex_lock(&lpr->mutex);

    for (;;){
        // Wait for condition
        pthread_mutex_lock(&lpr->mutex);
        printf("Waiting for condition at gate %d\n", gate);
        pthread_cond_wait(&lpr->cond, &lpr->mutex);
        pthread_mutex_unlock(&lpr->mutex);
        printf("Condition met at entrance %d\n", gate);
        

        // // Set info sign to 4
        // pthread_mutex_lock(&shm.data->entrance[gate].information_sign.mutex);
        // shm.data->entrance[gate].information_sign.display = '4';
        // pthread_mutex_unlock(&shm.data->entrance[gate].information_sign.mutex);

        // pthread_cond_signal(&shm.data->entrance[gate].information_sign.cond);


    }
    
    // for(;;){
    //     pthread_mutex_lock(&lpr->mutex);
    //     //pthread_cond_wait(&lpr->cond, &lpr->mutex);
    //     //printf("Found: %s\n", lpr->plate);
    //     pthread_mutex_unlock(&lpr->mutex);
    //     //pthread_mutex_lock(&information_sign->mutex);
        
    //     //pthread_cond_signal(&information_sign->cond);
    //     //pthread_mutex_unlock(&information_sign->mutex);

    // }

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
            printf("%d: %c                %d: %c\n", i+1, shm.data->entrance[i].boom_gate.status, i+1, shm.data->exit[i].boom_gate.status);
        }
        printf("\n");
        //Print the status of the LPR sensors
        printf("Status of the LPR sensors\n");
        printf("Entrances           Exits           Level\n");
        for(int i = 0; i < ENTRANCES; i++){
            printf("%d: %s                %d: %s                %d: %s\n", i+1, shm.data->entrance[i].lpr_sensor.plate, i+1, shm.data->exit[i].lpr_sensor.plate, i+1, shm.data->level[i].lpr_sensor.plate);
        }
        // printf("\n");
        // //Print the status of the information signs
        // printf("Status of the information signs\n");
        // for(int i = 0; i < ENTRANCES; i++){
        //     printf("%d: %c  ", i+1, shm.data->entrance[i].info_sign.status);
        // }
        // printf("\n");
        // //Print the level information
        // printf("Level information\n");
        // for(int i = 0; i < LEVELS; i++){
        //     printf("level %d: %d/%d\n", i+1, shm.data->level[i].currLevelCapacity, PARKING_CAPACITY);
        // }
        printf("\n");
        usleep(5 * MS_IN_MICROSECONDS);


    }



int main(void)
{

    
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
    //Create a thead for each Entrance LPR
    for (int i = 0; i < ENTRANCES; i++){
        pthread_create(&lprThreadEntrances[i], NULL, lprEntranceHandler, (void *) (&entranceData[i]));
    }
    
    

    
    for (;;){
        //display(shm);

    }
   

}

        
    





