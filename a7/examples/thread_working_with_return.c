#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <limits.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <err.h>

//==================================
//    A THREAD
//==================================

typedef struct {
    int index;
    int offset;
    int *offset2;
} threadParamType;
#define ARR_SIZE 5
int numOfThreads = 3;

void* threadCreateArray(void *argument) {
    threadParamType param = *(threadParamType *) argument;
    int startingOffset = param.offset;
    int *arr = malloc(ARR_SIZE * sizeof (int));

    int i;
    for (i = 0; i < ARR_SIZE; i++) {
        arr[i] = i + startingOffset;
    }
    
    pthread_exit(arr);
}
// ===========

int main(int argc, char *argv[]) {
    pthread_t threads[numOfThreads];
    int i;
    int *values[numOfThreads];
    
    for (i = 0; i < numOfThreads; i++) {
        // create a new thread to process this file
        threadParamType *param = malloc(sizeof (threadParamType));
        int offset = i * 10;
        param->index = i;
        param->offset = offset;
        param->offset2 = &offset;
        fprintf(stdout, "\n[%d] starting %d %d", i, param->offset, *param->offset2);
        pthread_create(&threads[i], NULL, threadCreateArray, (void*) param);
    }

    for (i = 0; i < numOfThreads; i++) {
        pthread_join(threads[i], (void **) &values[i]);
    }
    
    for( i = 0; i < numOfThreads; i++){ 
        int *arr = values[i];
        int j;
        printf("\n---------------");
        for ( j = 0; j < ARR_SIZE; j++){ 
            int val = arr[j];
            fprintf(stdout, "\nThread[%d] j[%d] %d", i, j, val);
        }
    }
    
    fprintf(stdout, "\n");
}
