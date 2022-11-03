
// ░█████╗░░█████╗░██████╗░  ██████╗░░█████╗░██████╗░██╗░░██╗
// ██╔══██╗██╔══██╗██╔══██╗  ██╔══██╗██╔══██╗██╔══██╗██║░██╔╝
// ██║░░╚═╝███████║██████╔╝  ██████╔╝███████║██████╔╝█████═╝░
// ██║░░██╗██╔══██║██╔══██╗  ██╔═══╝░██╔══██║██╔══██╗██╔═██╗░
// ╚█████╔╝██║░░██║██║░░██║  ██║░░░░░██║░░██║██║░░██║██║░╚██╗
// ░╚════╝░╚═╝░░╚═╝╚═╝░░╚═╝  ╚═╝░░░░░╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚═╝

// ░██████╗██╗███╗░░░███╗██╗░░░██╗██╗░░░░░░█████╗░████████╗░█████╗░██████╗░
// ██╔════╝██║████╗░████║██║░░░██║██║░░░░░██╔══██╗╚══██╔══╝██╔══██╗██╔══██╗
// ╚█████╗░██║██╔████╔██║██║░░░██║██║░░░░░███████║░░░██║░░░██║░░██║██████╔╝
// ░╚═══██╗██║██║╚██╔╝██║██║░░░██║██║░░░░░██╔══██║░░░██║░░░██║░░██║██╔══██╗
// ██████╔╝██║██║░╚═╝░██║╚██████╔╝███████╗██║░░██║░░░██║░░░╚█████╔╝██║░░██║
// ╚═════╝░╚═╝╚═╝░░░░░╚═╝░╚═════╝░╚══════╝╚═╝░░╚═╝░░░╚═╝░░░░╚════╝░╚═╝░░╚═╝

// This program is the simulator for the car park system.

// The simulator is in charge of the following:
// --------------------------------------------
// Simulate cars:
// - Provide a random license plate number for each car
// - Provide a random entry time for each car at a random entry gate
// - Trigger a LPR camera to read the license plate number
// - Provide a level of parking space required for each car
// - Reject unauthorized cars
// - Drive to the parking space and park
// - Provide a random exit time for each car at a random exit gate
// - Trigger a LPR camera to read the license plate number
// - Drive out of the parking space and leave
//  --------------------------------------------
// Simulate boom gates:
// - Open the boom gate when a car is approaching
// - Boom gate will open and close after a set time upon recieving a signal from the manager
// - Close the boom gate when a car has passed
//  --------------------------------------------
// Simulate LPR cameras:
// - Read the license plate number of a car
// - Send the license plate number to the manager
//  --------------------------------------------
// Simulate temperature
// - Each parking level has a temperature sensor
// - Update the temperature of each parking level frequently
// - The temperature must be reasonable (e.g. 20-30 degrees)
// - The temperature must be able to simulate a fire
//  --------------------------------------------


//  --------------------------------------------
// Simualtor timings 
//  --------------------------------------------
// Car will generated every 1-100ms (random)
// - the stdlib.h rand() function is fine, for example (but keep in mind
// that you should protect calls to rand() with a mutex as rand() accesses a global variable
// containing the current random seed.).
// Once a car has reached the queue for a boom gate, it will wait for 2ms before triggering the LPR camera
// Boom gate takes 10ms to open and close
//Once a car has parked it will wait 100-10000ms (random) before leaving the level and triggering the LPR camera
// It then takes a car a further 10ms to drive to a random exit and trigger the exit LPR camera
// The temperature will change every 1-5ms (random) and will be between 20-40 degrees



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


// Global variables
#define entrances 5
#define exits 5
#define floors 5
#define spaces 20


// Function to read the temperature of all parking levels 
void checkTemperature(){
    // This function will read the temperature of all parking levels
    // The temperature will change every 1-5ms (random) and will be between 20-40 degrees
    // Make an array of temperatures for each level
    int temperature[floors];
    // Loop through each level and generate a random temperature
    for (int i = 0; i < floors; i++){
        // Generate a random temperature between 20-40
        temperature[i] = (rand() % (40 - 20 + 1)) + 20;
        // Print the temperature of each level
        printf("The temperature of level %d is %d degrees\n", i, temperature[i]);

    }

    // Return the temperature of each level
    // return temperature;



}

// Function to simulate a fire
void fire(){
    // TODO
}


// Function to simulate a car
void car(char license){
    // Chose a random entry gate
    int entry = rand() % entrances;

}

// Make a structure for the boom gate
struct boomGate{
    int boomGateID;
    int boomGateStatus;
    int boomGateTime;
};

// Make a structure for the car
struct car{
    char licensePlate[8];
    int entryTime;
    int exitTime;
    int entryGate;
    int exitGate;
    int level;
    int space;
};

// Function to simulate a boom gate
// Provide the gate number, the entry or exit gate and request for the gate to open or close
void boomGate(int gate, char entryOrExit, char openOrClose){
    // TODO
}





// !!!!! This needs to be in a hash table! Just making a note here !!!!! 
// Right now this is not in a hash table, I am just testing the functionality of the program

// Pick a random license plate from the plates.txt file 
// with a 50% chance of being a valid plate
char *getPlate() {
    FILE *fp;
    char *plate = malloc(8);
    int i = 0;
    int valid = rand() % 2;
    fp = fopen("plates.txt", "r");
    if (fp == NULL) {
        printf("Error opening file");
        exit(1);
    }
    // Find how many lines in the file
    int lines = 0;
    char c;
    for (c = getc(fp); c != EOF; c = getc(fp)) {
        if (c == '\n') {
            lines++;
        }
    }
    // Pick a random line
    int line = rand() % lines;
    printf("Random line = %d\n", line);
    
    // Having issues with this, need to look into it more

    // go to a random line in the file
    rewind(fp);
    for (i = 0; i < line; i++) {
        fgets(plate, 8, fp);
    }
    // get the plate
    fgets(plate, 8, fp);
    printf("Plate = %s\n", plate);

    fclose(fp);
    if (valid == 0) {
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
        printf("Random plate = %s\n", plate);


      
        
        return plate;
    }
    return plate;
}


int main(void)
{


    time_t t;
    srand((unsigned) time(&t));


    printf("Hello World!\n");
    // Call getPlate() to test it
    char *plate = getPlate();
    printf("%s\n", plate);

    // Call checkTemperature() to test it
    checkTemperature();


    // Exit the program
    return 0;
}
