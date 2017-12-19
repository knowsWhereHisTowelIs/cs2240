// Two Threads that (safely) count to argv[1] 
#include <stdio.h>
#include <pthread.h>
#include "apue.h"
#include <stdlib.h>
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static int glob = 0;

static void *threadFunc1(void *arg) {
    /* Loop 'arg' times incrementing 'glob' */

    int loops = *((int *) arg);
    int loc, j, s;

    for (j = 0; j < loops; j++) {
        s = pthread_mutex_lock(&mtx);
        if (s != 0)err_sys("pthread_mutex_lock");
        printf("In Thread1\n");
        sleep(1);
        loc = glob;
        loc++;
        glob = loc;
        s = pthread_mutex_unlock(&mtx);
        if (s != 0)err_sys("pthread_mutex_unlock");

    }

    return NULL;
}

static void *threadFunc2(void *arg) {
    /* Loop 'arg' times incrementing 'glob' */

    int loops = *((int *) arg);
    int loc, j, s;

    for (j = 0; j < loops; j++) {
        s = pthread_mutex_lock(&mtx);
        if (s != 0)err_sys("pthread_mutex_lock");
        printf("In Thread2\n");
        sleep(3);
        loc = glob;
        loc++;
        glob = loc;
        s = pthread_mutex_unlock(&mtx);
        if (s != 0)err_sys("pthread_mutex_unlock");
    }


    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2;
    int loops, s;
    loops = atoi(argv[1]);

    s = pthread_create(&t1, NULL, threadFunc1, &loops);
    if (s != 0)err_sys("first pthread_create");
    s = pthread_create(&t2, NULL, threadFunc2, &loops);
    if (s != 0)err_sys("second pthread_create");
    s = pthread_join(t1, NULL);
    if (s != 0)
        err_sys("pthread_join");
    s = pthread_join(t2, NULL);
    if (s != 0)
        err_sys("pthread_join");
    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}

