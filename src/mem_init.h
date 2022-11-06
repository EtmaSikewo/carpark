#include <pthread.h>
#include <stdint.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>

//  Change this value to scale the speed of time
#define TIME_SCALE 1
#define MS_IN_MICROSECONDS 1000 * TIME_SCALE

#define ENTRANCES 5
#define EXITS 5
#define LEVELS 5
#define PARKING_CAPACITY 20

#define LICENCE_PLATE_SIZE 6
#define PLATES_DIR "../data/plates.txt"
#define BILLING_DIR "../data/billing.txt"

#define DEBUG 0

typedef struct lpr_sensor
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char plate[6];      //  default value is "000000"
    uint8_t padding[2]; //  padding
} lpr_sensor_t;

typedef struct boom_gate
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char status;        //  'C' = closed, 'O' = open, 'R' = raising, 'L' = lowering
    uint8_t padding[7]; //  padding
} boom_gate_t;

typedef struct information_sign
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char display;       //  default is ' '
    uint8_t padding[7]; //  padding
} information_sign_t;

//  struct for entrance
typedef struct entrance
{
    lpr_sensor_t lpr_sensor;
    boom_gate_t boom_gate;
    information_sign_t information_sign;
} entrance_t;

//  struct for exit
typedef struct exit
{
    lpr_sensor_t lpr_sensor;
    boom_gate_t boom_gate;
} exit_t;

//  struct for levels
typedef struct level
{
    lpr_sensor_t lpr_sensor;
    uint16_t temperature_sensor;
    char alarm;         // default is '0'
    uint8_t padding[5]; //  padding
} level_t;

/**
 * A shared memory control structure.
 */
typedef struct shared_memory_data
{
    entrance_t entrance[ENTRANCES];
    exit_t exit[EXITS];
    level_t level[LEVELS];
} shared_memory_data_t;

/**
 * A shared memory control structure.
 */
typedef struct shared_memory
{
    const char *name;
    int fd;
    shared_memory_data_t *data;
} shared_memory_t;

// below functions derived from AMS tasks

bool create_shared_object(shared_memory_t *shared_mem_obj, const char *shared_mem_name)
{

    shm_unlink(shared_mem_name);
    shared_mem_obj->name = shared_mem_name;

    if ((shared_mem_obj->fd = shm_open(shared_mem_name, O_CREAT | O_RDWR, 0666)) < 0)
    {
        shared_mem_obj->data = NULL;
        return false;
    }

    if ((ftruncate(shared_mem_obj->fd, sizeof(shared_memory_data_t))) < 0)
    {
        shared_mem_obj->data = NULL;
        return false;
    }

    char *sd;
    if ((sd = mmap(0, sizeof(shared_memory_data_t), PROT_WRITE | PROT_READ, MAP_SHARED, shared_mem_obj->fd, 0)) == (char *)-1)
    {
        shared_mem_obj->data = MAP_FAILED;
        return false;
    }
    shared_mem_obj->data = (shared_memory_data_t *)sd;

    return true;
}

void destroy_shared_object(shared_memory_t *shared_mem_obj)
{
    if (shared_mem_obj->name != (char *)"")
    {
        munmap(shared_mem_obj->data, sizeof(shared_memory_data_t));
        shm_unlink(shared_mem_obj->name);
        shared_mem_obj->fd = -1;
        shared_mem_obj->data = NULL;
    }
}

bool get_shared_object(shared_memory_t *shared_mem_obj, const char *shared_mem_name)
{
    if ((shared_mem_obj->fd = shm_open(shared_mem_name, O_RDWR, 0666)) < 0)
    {
        shared_mem_obj->data = NULL;
        return false;
    }

    char *sd;
    if ((sd = mmap(0, sizeof(shared_memory_data_t), PROT_WRITE | PROT_READ, MAP_SHARED, shared_mem_obj->fd, 0)) == (char *)-1)
    {
        shared_mem_obj->data = MAP_FAILED;
        return false;
    }
    shared_mem_obj->data = (shared_memory_data_t *)sd;
    return true;
}