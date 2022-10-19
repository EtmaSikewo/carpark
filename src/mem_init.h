#define ENTRANCES 5
#define EXITS 5
#define LEVELS 5
#define PARKING_CAPACITY 20


typedef struct lpr_sensor {
    pthread_mutex_t mutex; 
    pthread_cond_t cond;
    char plate[6] = "000000";
    byte[2];
} lpr_sensor_t;


typedef struct boom_gate {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char status = "C";
    byte[7];
} boom_gate_t;


typedef struct information_sign {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char display = " ";
    byte[7];
} information_sign_t;


//  struct for entrance
typedef struct entrance {
    lpr_sensor_t lpr_sensor;
    boom_gate_t boom_gate;
    information_sign_t information_sign;
} entrance_t;


//  struct for exit
typedef struct exit {
    lpr_sensor_t lpr_sensor;
    boom_gate_t boom_gate;
} exit_t;


//  struct for levels
typedef struct level {
    lpr_sensor_t lpr_sensor;
    int16_t temperature_sensor;
    char alarm = "0";
    byte[5];
} level_t;


/**
 * A shared memory control structure.
 */
typedef struct shared_memory {
    /// The name of the shared memory object.
    const char* name;

    /// The file descriptor used to manage the shared memory object.
    int fd;

    /// Address of the shared data block.
    entrance_t entrance[ENTRANCES];
    exit_t exit[EXITS];
    level_t level[LEVELS];
} shared_memory_t;