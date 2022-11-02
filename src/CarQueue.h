// Create a queue of cars using nested structures

typedef struct car 
{
    char plate[6];
    struct car *next;
} Car;

typedef struct carQueue
{
    Car *front;
    Car *rear;
} CarQueue;


