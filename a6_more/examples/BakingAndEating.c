#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 3

//the mutex
pthread_mutex_t Lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t PiesToEat = PTHREAD_COND_INITIALIZER;

//Shared Resources
int Done[NUM_THREADS];
int PiecesOfPie;

void * baker() {
    int i, AllDone;
    while (!AllDone) {
        AllDone = 1;
        for (i = 0; i < NUM_THREADS; i++)
            AllDone = (Done[i] && AllDone); //clever

        if (AllDone) {
            printf("All Eaters are done\n");
        } else {
            pthread_mutex_lock(&Lock);

            if (PiecesOfPie == 0) {
                PiecesOfPie += 2;
                printf("Baked 2 Pieces of Pie\n");
            }

            //Broadcast to all threads that pies have been made
            pthread_cond_broadcast(&PiesToEat);
            pthread_mutex_unlock(&Lock);
        }

        usleep(100);
    }

    printf("Done Baking\n");
    pthread_exit(NULL);
}

void * eater(void * TID) {
    long threadID = (long) TID;
    int PiecesEaten = 0; // Control Pieces Eaten by Thread

    Done[threadID] = 0;

    while (PiecesEaten < 2) {
        pthread_mutex_lock(&Lock);

        if (PiecesOfPie == 0) {
            printf("No pieces of pie for thread %ld\n", threadID);
            pthread_cond_wait(&PiesToEat, &Lock);
            printf("Thread %ld waited for baker\n", threadID);
        } else {
            PiecesOfPie--;
            PiecesEaten++;
            printf("Thread %ld ate a piece of pie\n", threadID);
        }

        pthread_mutex_unlock(&Lock);

        usleep(1);
    }

    printf("Thread %ld has eaten 2 pieces of pie\n", threadID);

    Done[threadID] = 1;

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_t PieBaker;

    PiecesOfPie = 0;

    //Create Eaters
    long i;
    for (i = 0; i < NUM_THREADS; i++) {
        printf("IN MAIN: creating thread #%lu\n", i);
        //create has 4 arguments:
        //1 - Ptr of thread
        //2 - Don't care, NULL
        //3 - Name of Void * method
        //4 - Something to pass as an argument to thread's method
        pthread_create(&threads[i], NULL, eater, (void *) i);
    }

    //Create Baker
    printf("IN MAIN: creating the baker\n");
    pthread_create(&PieBaker, NULL, baker, NULL);

    /* Last thing that main() should do */
    pthread_mutex_destroy(&Lock);
    pthread_join(PieBaker, NULL); //Waits for the baker thread to end.
    //Could wait for eaters as well but Baker waits for eaters anyway.

    printf("Done in main\n");
    return 0;
}
