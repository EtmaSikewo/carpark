// This program is the simulator for the car park system.


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
    // display the number of lines
    printf("Number of lines = %d\n", lines);
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



// Print hello world
int main(void)
{


    time_t t;
    srand((unsigned) time(&t));


    printf("Hello World!\n");
    // Call getPlate() to test it
    char *plate = getPlate();
    printf("%s\n", plate);
    return 0;
}
