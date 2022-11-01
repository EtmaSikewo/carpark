// #include <stdio.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
// #include <math.h>
// #include "mem_init.h"

// #define MS_IN_MICROSECONDS 1000




// // Okay so making some huge changes and figuring stuff out - Joe


// // ---------------------------------------------
// // Functions relating to the boomgate          |
// // ---------------------------------------------

// // Function to initalise the boom gate
// // ---------------------------------------------
// void *boomgateInit(void *arg){
//     boom_gate_t *boom_gate = arg;
    
//     // Error check the mutex and condition variable
//     if (pthread_mutex_init(&boom_gate->mutex, NULL) != 0) {
//         printf("Boom gate mutex init failed");
//     }

//     // Lock the mutex 
//     pthread_mutex_lock(&boom_gate->mutex);
//     // Set the boom gate to closed
//     boom_gate->status = 'C';
//     //Broadcast the condition variable
//     pthread_cond_broadcast(&boom_gate->cond);
//     // Unlock the mutex
//     pthread_mutex_unlock(&boom_gate->mutex);

// }

// // Function that will be called by the boom gate thread
// // ---------------------------------------------
// void boomGateSimualtor(void *arg){
//     // Get the boom gate thread
//     boom_gate_t *boom_gate = arg;
//     // Lock the boomgate mutex for use
//     pthread_mutex_lock(&boom_gate->mutex);

//     // Logic for the boom gate
    
//     // If the boomgate has been set to raise
//     if (boom_gate->status == 'R'){
//         // Raise the boom gate after 10ms
//         usleep(10 * MS_IN_MICROSECONDS);
//         boom_gate->status = 'O';
//         // Broadcast the change to the boom gate
//         pthread_cond_broadcast(&boom_gate->cond);
//         //Print the boom gate status
//         printf("Boom gate opening");
//     }
//     // If the boom gate has been set to open
//     else if (boom_gate->status == 'O'){
//         //Print the boom gate status
//         printf("Boom gate is open");
//     }
//     // If the boom gate is set to lowering 
//     else if (boom_gate->status == 'L'){
//         // Lower the boom gate after 10ms
//         usleep(10 * MS_IN_MICROSECONDS);
//         boom_gate->status = 'C';
//         // Broadcast the change to the boom gate
//         pthread_cond_broadcast(&boom_gate->cond);
//         //Print the boom gate status
//         printf("Boom gate is closing");
//     }
//     // If the boom gate is set to closed
//     else if (boom_gate->status == 'C'){
//         //Print the boom gate status
//         printf("Boom gate is closed");
//     }
//     // If the boom gate is set to an illegal status
//     else{
//         //Print the boom gate status
//         printf("Boom gate is in an illegal state");
//     }
//     // Unlock the boom gate mutex
//     pthread_mutex_unlock(&boom_gate->mutex);
// }

// // ---------------------------------------------
// // Functions relating to the car               |
// // ---------------------------------------------

// // Function to get a plate from plates.txt
// // ---------------------------------------------
// char *getPlate(){
//     // Open the file
//     FILE *fp = fopen("plates.txt", "r");
//     // Error check the file
//     if (fp == NULL){
//         printf("Error opening file");
//     }
//     // The license plate is 6 characters long
//     // Create a buffer to store the plate
//     char *plate = malloc(6 * sizeof(char));
//     // Grab a random line from the 100 lines in the file
//     int line = rand() % 100;
//     // Loop through the file until the line is reached
//     for (int i = 0; i < line; i++){
//         // Read the line
//         fgets(plate, 6, fp);
//     }
//     // Close the file
//     fclose(fp);
//     // Return the plate
//     return plate;
// }


// //  main function to create PARKING mem segment
// int main(int argc, char **argv)
// {

//     // Simualtor welcome message
//     printf("Welcome to the Parking Simulator\n");
    
//     //Set the seed of the random number generator
//     srand(time(0));
//     //  start the shared memory
//     shared_memory_t shm;
//     create_shared_object(&shm, "PARKING");
//     setDefaults(shm);

//     // sleep for a bit for startup
//     usleep(100 * MS_IN_MICROSECONDS);


//     // Loop getplate for testing
//     for (int i = 0; i < 10; i++) {
//         char *plate = getPlate();
//         printf("Plate: %s\n", plate);
//     }


//     // for(int i = 0; i < 1; i++) {
//     //     //  generate a car
//     //     carcar_t car;
//     //     generateCar(&car);
//     //     printf("Car %s has entered the parking lot at gate %d\n", car.plate, car.entryGate);
//     //     pthread_cond_signal(&shm.data->entrance[car.entryGate].lpr_sensor.cond);
//     //     pthread_cond_wait(&shm.data->entrance[car.entryGate].lpr_sensor.cond, &shm.data->entrance[car.entryGate].lpr_sensor.mutex);
//     // }

//     // for(;;) {
//     //     // Run getPlate
//     //     char *plate = getPlate();
//     //     printf("Plate: %s\n", plate);
//     //     //printf("Looping...\n");

//     // }

    

//     return 0;
// }