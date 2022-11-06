//#include <stdio.h>
//#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "mem_init.h"

#define MEDIAN_WINDOW 5
#define TEMPCHANGE_WINDOW 30
#define ALARM_TEMP 58
#define ALARM_TEMP_AVG_PERCENT 0.9
#define ALARM_TEMP_SINGLE_VARIANCE 8


static int alarm_active = 0;

// start taken from https://www.codevscolor.com/c-program-find-median-array
static void sort(int *arr, int size)
{
    int i;
    int j;
    int t;

    for (i = 0; i < size; i++)
    {
        for (j = i + 1; j < size; j++)
        {
            if (arr[j] < arr[i])
            {
                t = arr[i];
                arr[i] = arr[j];
                arr[j] = t;
            }
        }
    }
}
static float getMedian(int *arr, int size)
{
    float retVal;
    if ((size % 2) == 0)
    {
        retVal = (arr[(size - 1) / 2] + arr[size / 2]) / 2.0;
    }
    else
    {
        retVal = arr[size / 2];
    }
    return retVal;
}
// end taken from https://www.codevscolor.com/c-program-find-median-array


static void *tempmonitor(void *arg)
{
    level_t *level = arg;

    int temparray[MEDIAN_WINDOW] = {0};
    int sortarray[MEDIAN_WINDOW] = {0};
    int smoothedtemparray[TEMPCHANGE_WINDOW] = {0};
    int tempsReceived = 0;
    int temparrayindex = 0;
    int smoothedtemparrayindex = 0;
    int tempOldest;

	for (;;) {
		// Calculate address of temperature sensor
        int temp = level->temperature_sensor;
        int hightemps = 0;
        // Add temperature to array
        if(temparrayindex == MEDIAN_WINDOW) {
            temparrayindex = 0;
        }
        temparray[temparrayindex] = temp;
        temparrayindex++; // maybe later
        tempsReceived++;

        if(tempsReceived >= MEDIAN_WINDOW) {
            // smooth the temperature & save
            sortarray[0] = temparray[0];
            sortarray[1] = temparray[1];
            sortarray[2] = temparray[2];
            sortarray[3] = temparray[3];
            sortarray[4] = temparray[4];
            // sort array
            sort(sortarray, MEDIAN_WINDOW);
            // get median
            if(smoothedtemparrayindex == TEMPCHANGE_WINDOW) {
                smoothedtemparrayindex = 0;
            }
            smoothedtemparray[smoothedtemparrayindex] = getMedian(sortarray, MEDIAN_WINDOW);
            smoothedtemparrayindex++;

			for (int i = 0;i < TEMPCHANGE_WINDOW ;i++) {
				// Temperatures of 58 degrees and higher are a concern
				if (smoothedtemparray[i] >= ALARM_TEMP) {
                    hightemps++;
                }
			}
			
			if (tempsReceived >= (TEMPCHANGE_WINDOW + MEDIAN_WINDOW)) {
				// If 90% of the last 30 temperatures are >= 58 degrees, this is considered a high temperature. Raise the alarm
				if (hightemps >= (TEMPCHANGE_WINDOW * ALARM_TEMP_AVG_PERCENT)) {
					alarm_active = 1;
				}
				// If the newest temp is >= 8 degrees higher than the oldest temp (out of the last 30), this is a high rate-of-rise. Raise the alarm
				if ((temp - tempOldest) >= ALARM_TEMP_SINGLE_VARIANCE) {
					alarm_active = 1;
                }
            }
		}
        tempOldest = temp;
		usleep(2000);
	}
    pthread_exit(NULL);
}

static void *openboomgate(void *arg)
{
    boom_gate_t *boom_gate = arg;
	pthread_mutex_lock(&boom_gate->mutex);
	for (;;) {
		if (boom_gate->status == 'C') {
			boom_gate->status = 'R';
			pthread_cond_broadcast(&boom_gate->cond);
            pthread_mutex_unlock(&boom_gate->mutex);
		}
		if (boom_gate->status == 'O') {
            pthread_mutex_lock(&boom_gate->mutex); // lock it open
            for(;;) {
                // do nothing!
            }
		}
		pthread_cond_wait(&boom_gate->cond, &boom_gate->mutex);
	}
	pthread_mutex_unlock(&boom_gate->mutex);
	pthread_exit(NULL);
}


static void emergency_mode(shared_memory_t *shm) {
	// Handle the alarm system and open boom gates
	// Activate alarms on all levels
	for (int i = 0; i < LEVELS; i++) {
        shm->data->level[i].alarm = 1;
	}
	
	// Open up all boom gates
	for (int i = 0; i < ENTRANCES; i++) {
        pthread_t boomgatethread;
		pthread_create(&boomgatethread, NULL, &openboomgate, &shm->data->entrance[i].boom_gate);
	}
	for (int i = 0; i < EXITS; i++) {
        pthread_t boomgatethread;
		pthread_create(&boomgatethread, NULL, &openboomgate, &shm->data->exit[i].boom_gate);
	}
	
	// Show evacuation message on an endless loop
	for (;;) {
        
		const char evacmessage[] = "EVACUATE ";
		for (const char *p = evacmessage; *p != '\0'; p++) {
			for (int i = 0; i < ENTRANCES; i++) {
                information_sign_t *info = &shm->data->entrance[i].information_sign;
				//int addr = 288 * i + 192;
				//volatile struct parkingsign *sign = shm + addr;
				pthread_mutex_lock(&info->mutex);
				//sign->display = *p;
                (void)strcpy(&info->display, p);
				pthread_cond_broadcast(&info->cond);
				pthread_mutex_unlock(&info->mutex);
			}
			usleep(20000);
		}
        
	}
}


int main(int argc, char ** argv)
{

    // deliberately ignore argc and argv to avoid compiler warnings
    (void)argc;
    (void)argv;

    shared_memory_t shm;
    get_shared_object(&shm, "PARKING"); // Need to error handle this
	
	for (int i = 0; i < LEVELS; i++) {
        pthread_t tempmonthread;
		pthread_create(&tempmonthread, NULL, &tempmonitor, &shm.data->level[i]);
	}
	for (;;) {
		if (alarm_active == 1) {
			emergency_mode(&shm);
		}
		usleep(1000);
	}
	

    munmap(shm.data, sizeof(shared_memory_data_t));
    shm_unlink(shm.name);
}
