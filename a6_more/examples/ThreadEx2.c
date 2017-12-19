/// Modifications to Code Taken from Posix Tutorial 
//  https://computing.llnl.gov/tutorials/pthreads/
// Example demonstrates join and thread creating thread
// R. Trenary, 1/26/12
//
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *WorkerStart(void *threadid);

void *BossStart(void *threadid) {
    pthread_t Worker;
    int rc;
    long tid, t;
    void *status;
    t = (long) threadid;
    printf("I am the boss and will create a worker\n");

    rc = pthread_create(&Worker, NULL, WorkerStart, (void *) t);
    if (rc) {
        printf("failed to create woiker\n");
        exit(-1);
    }

    pthread_join(Worker, &status);
    printf("I am the boss and the worker %ld has finished \n", tid);
    pthread_exit(NULL);
}

void *WorkerStart(void *threadid) {
    ;
    long tid;

    tid = (long) threadid;
    printf("I am the worker %ld and will work then meet the Boss again\n", tid);
    usleep(1000);
    printf("Off to meet the boss ! \n");
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t Boss;
    int rc;
    long t;
    t = 10;
    printf("In main: creating thread %ld\n", t);
    rc = pthread_create(&Boss, NULL, BossStart, (void *) t);
    if (rc) {
        printf("failed to create boss\n");
        exit(-1);
    }

    /* Last thing that main() should do */
    pthread_exit(NULL);
}

