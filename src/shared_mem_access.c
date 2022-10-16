/*
 * shm-client - client program to demonstrate shared memory.
 * Note: compile with -lrt:
 * gcc -o shm_client shm_client.c -lrt
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define SHMSZ 28

#include "shared_mem_access.h"

void memoryAccess(void) {
    
    int shm_fd;
    const char *key;
    char *shm, *s;

    /*
     * We need to get the segment named
     * "SHM_TEST", created by the server.
     */
    key = "SHM_TEST";

    /*
     * Locate the segment.
     */
    if ((shm_fd = shm_open(key, O_RDWR, 0)) < 0)
    {
        perror("shm_open");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = mmap(0, SHMSZ, PROT_WRITE, MAP_SHARED, shm_fd, 0)) == (char *)-1)
    {
        perror("mmap");
        exit(1);
    }

    /*
     * Now read what the server put in the memory.
     */
    for (s = shm; *s != 0; s++)
        putchar(*s);
    putchar('\n');

    if (shm[27] > 0) shm[27]--;
}