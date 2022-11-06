//#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "mem_init.h"
#include "firealarm.h"
#include <string.h>

//int shm_fd;
//volatile void *shm;
static int alarm_active = 0;
//static pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
//static pthread_cond_t alarm_condvar = PTHREAD_COND_INITIALIZER;


static struct tempnode *deletenodes(struct tempnode *templist, int after)
{

	if (templist->next != NULL) {
		templist->next = deletenodes(templist->next, after - 1);
	}
	if (after <= 0) {
		free(templist);
		templist = NULL;
	}
	return templist;
}
static int compare(const void *first, const void *second)
{
	return *((const int *)first) - *((const int *)second);
}

static void *tempmonitor(void *arg)
{
    level_t *level = arg;
	struct tempnode *templist = NULL;
	struct tempnode *newtemp;
    struct tempnode *medianlist = NULL;
    struct tempnode *oldesttemp;
    int count;
	int temp;
    int mediantemp;
    int hightemps;

	for (;;) {
		// Calculate address of temperature sensor
		//addr = 0150 * level + 2496;
		//temp = *((int16_t *)(shm + addr));
        temp = level->temperature_sensor;
		
		// Add temperature to beginning of linked list
		newtemp = malloc(sizeof(struct tempnode));
		newtemp->temperature = temp;
		newtemp->next = templist;
		templist = newtemp;
		
		// Delete nodes after 5th
		(void)deletenodes(templist, MEDIAN_WINDOW);
		
		// Count nodes
		count = 0;
		for (struct tempnode *t = templist; t != NULL; t = t->next) {
			count++;
		}
		
		if (count == MEDIAN_WINDOW) { // Temperatures are only counted once we have 5 samples
			int *sorttemp = malloc(sizeof(int) * MEDIAN_WINDOW);
			count = 0;
			for (struct tempnode *t = templist; t != NULL; t = t->next) {
                count++;
				sorttemp[count] = t->temperature;
			}
			qsort(sorttemp, MEDIAN_WINDOW, sizeof(int), compare);
			mediantemp = sorttemp[(MEDIAN_WINDOW - 1) / 2];
			
			// Add median temp to linked list
			newtemp = malloc(sizeof(struct tempnode));
			newtemp->temperature = mediantemp;
			newtemp->next = medianlist;
			medianlist = newtemp;
			
			// Delete nodes after 30th
			(void)deletenodes(medianlist, TEMPCHANGE_WINDOW);
			
			// Count nodes
			count = 0;
			hightemps = 0;
			
			for (struct tempnode *t = medianlist; t != NULL; t = t->next) {
				// Temperatures of 58 degrees and higher are a concern
				if (t->temperature >= 58) {
                    hightemps++;
                }
				// Store the oldest temperature for rate-of-rise detection
				oldesttemp = t;
				count++;
			}
			
			if (count == TEMPCHANGE_WINDOW) {
				// If 90% of the last 30 temperatures are >= 58 degrees,
				// this is considered a high temperature. Raise the alarm
				if (hightemps >= (TEMPCHANGE_WINDOW * 0.9)) {
					alarm_active = 1;
				}
				// If the newest temp is >= 8 degrees higher than the oldest
				// temp (out of the last 30), this is a high rate-of-rise.
				// Raise the alarm
				if ((templist->temperature - oldesttemp->temperature) >= 8) {
					alarm_active = 1;
            }
            }
		}
		
		usleep(2000);
		
	}
    pthread_exit(NULL);
}

static void *openboomgate(void *arg)
{
	//struct boomgate *bg = arg;
    boom_gate_t *boom_gate = arg;
	pthread_mutex_lock(&boom_gate->mutex);
	for (;;) {
		if (boom_gate->status == 'C') {
			boom_gate->status = 'R';
			pthread_cond_broadcast(&boom_gate->cond);
		}
		if (boom_gate->status == 'O') {
		}
		pthread_cond_wait(&boom_gate->cond, &boom_gate->mutex);
	}
	pthread_mutex_unlock(&boom_gate->mutex);
	pthread_exit(NULL);
}

int main(int argc, char ** argv)
{

    // deliberately ignore argc and argv
    // to avoid compiler warnings
    (void)argc;
    (void)argv;

    shared_memory_t shm;
	//shm_fd = shm_open("PARKING", O_RDWR, 0);
	//shm = (volatile void *) mmap(0, 2920, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    get_shared_object(&shm, "PARKING"); // Need to error handle this

	//pthread_t *threads = malloc(sizeof(pthread_t) * LEVELS);
	
	for (int i = 0; i < LEVELS; i++) {
        pthread_t tempmonthread;
		pthread_create(&tempmonthread, NULL, &tempmonitor, &shm.data->level[i]);
	}
	for (;;) {
		if (alarm_active == 1) {
			goto emergency_mode;
		}
		usleep(1000);
	}
	
	emergency_mode:
	//fprintf(stderr, "*** ALARM ACTIVE ***\n");
	
	// Handle the alarm system and open boom gates
	// Activate alarms on all levels
	for (int i = 0; i < LEVELS; i++) {
		//int addr = 0150 * i + 2498;
		//char *alarm_trigger = (char *)shm + addr;
		//*alarm_trigger = 1;
        shm.data->level[i].alarm = 1;
	}
	
	// Open up all boom gates
	//pthread_t *boomgatethreads = malloc(sizeof(pthread_t) * (ENTRANCES + EXITS));
	for (int i = 0; i < ENTRANCES; i++) {
		//int addr = 288 * i + 96;
		//volatile struct boomgate *bg = shm + addr;
        pthread_t boomgatethread;
		pthread_create(&boomgatethread, NULL, &openboomgate, &shm.data->entrance[i].boom_gate);
	}
	for (int i = 0; i < EXITS; i++) {
		//int addr = 192 * i + 1536;
		//volatile struct boomgate *bg = shm + addr;
        pthread_t boomgatethread;
		pthread_create(&boomgatethread, NULL, &openboomgate, &shm.data->exit[i].boom_gate);
	}
	
	// Show evacuation message on an endless loop
	for (;;) {
		const char evacmessage[] = "EVACUATE ";
		for (const char *p = evacmessage; *p != '\0'; p++) {
			for (int i = 0; i < ENTRANCES; i++) {
                information_sign_t *info = &shm.data->entrance[i].information_sign;
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
	
    // replace with pthread_exit
	//for (int i = 0; i < LEVELS; i++) {
	//	pthread_join(threads[i], NULL);
	//}
	
    //termination is handled in the simulator
	//munmap((void *)shm, sizeof(shared_memory_data_t));
	//close(shm.fd);
    munmap(shm.data, sizeof(shared_memory_data_t));
    shm_unlink(shm.name);
}
