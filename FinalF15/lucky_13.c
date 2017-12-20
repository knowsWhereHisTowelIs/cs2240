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
} threadParamType;
#define N_OF_CHAIRS 13

static int hasChairsByThread[N_OF_CHAIRS];

void* threadCreateArray(void *argument) {
    threadParamType param = *(threadParamType *) argument;
    
    int prevThreadIndex = param.index - 1 < 0 ? N_OF_CHAIRS : param.index - 1;
    int nextThreadIndex = param.index + 1 > N_OF_CHAIRS ? 0 : param.index + 1;
//    hasChairsByThread[prevThreadIndex] = 1;
    
    hasChairsByThread[param.index] = 1;
    hasChairsByThread[nextThreadIndex] = 0;
//    hasChairsByThread[prevThreadIndex] = 0;
//    hasChairsByThread[param.index] = 1;
    
    printf("\nTHread[%d] prev:%d next:%d", param.index, prevThreadIndex, nextThreadIndex);
    
    pthread_exit(NULL);
//    pthread_exit(arr);
}
// ===========

int main(int argc, char *argv[]) {
    pthread_t threads[N_OF_CHAIRS+1];
    int i, j;
//    int *values[N_OF_CHAIRS];
    
    for(i = 0; i < N_OF_CHAIRS; i++){
        hasChairsByThread[i] = 1;
    }
    
    for(j = N_OF_CHAIRS + 1; j > 0; j--) {
        printf("\n\n");
        for (i = 0; i < j; i++) {
            // create a new thread to process this file
            threadParamType *param = malloc(sizeof (threadParamType));
            param->index = i;
            pthread_create(&threads[i], NULL, threadCreateArray, (void*) param);
        }

        for (i = 0; i < j; i++) {
            //pthread_join(threads[i], (void **) &values[i]);
            pthread_join(threads[i], NULL);
        }
    }
    
    printf("\n\nAfter musical chairs");
    for( i = 0; i < N_OF_CHAIRS+1; i++){
        printf("\nThread[%d] has chair ? %d", i, hasChairsByThread[i]);
    }
    
    fprintf(stdout, "\n");
}
