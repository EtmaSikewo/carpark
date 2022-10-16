#include <stdio.h>
#include "shared_mem_access.h"

int main(void){
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    //  access the shared memory pool
    memoryAccess();
    return 0;
}