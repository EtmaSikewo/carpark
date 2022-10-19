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


typedef struct car_manager {
    char plate[6] = "000000";
    int timeEntered;
} car_manager_t;

typedef struct level_manager {
    int currLevelCapacity;
    car_manager_t car_manager[PARKING_CAPACITY];
} levels_t;

levels_t levels[LEVELS];


int main(void){
    // printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    // //  read in from the shared memory pool
    // memoryAccess();

    someFunc(currLevelCapacity);


    return 0;
}