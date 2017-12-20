/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Race.h"

int numOfThreads = 50;

void* threadCreateArray(void *argument) {
    threadParamType param = *(threadParamType *) argument;
    
    char msg[10];
    sprintf(msg, "\nRow[%d]", param.index);
    
    lseek(param.fd, param.offset, SEEK_SET);
    write(param.fd, msg, strlen(msg));
    
    pthread_exit(NULL);
}
// ===========

int main(int argc, char *argv[]) {
    pthread_t threads[numOfThreads];
    int i;
    
    printf("\nPlease Clear race_file.txt between each run");
    
    int fd;
    if( (fd = open("no_race_file.txt", O_CREAT | O_RDWR, 0777 )) < 0 ){
        fprintf(stderr, "\nERROR");
        return 1;
    }
    
    for (i = 0; i < numOfThreads; i++) {
        // create a new thread to process this file
        threadParamType *param = malloc(sizeof (threadParamType));
        int offset = i * 10;
        param->index = i;
        param->fd = fd;
        param->offset = offset;
        pthread_create(&threads[i], NULL, threadCreateArray, (void*) param);
        
        pthread_join(threads[i], NULL);
    }

    for (i = 0; i < numOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    close(fd);
    fprintf(stdout, "\n");
}

