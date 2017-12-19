// Demo of thread join and exit with data passing
// Mystery of C pointers remains ineffable ? 
// R. Trenary, 3/13/14
//
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define MaxThread 100 

typedef struct DataStruct {
    int count; // I say DataStruct and mean struct DataStruct
    int * DataPtr;
}
DataStruct;

//==================================
//    A THREAD
//==================================

void *ThreadA(void * PassedPtr) {
    int NumPlaces, i;
    DataStruct *MyThreadStructPtr; // Contrived ?
    MyThreadStructPtr = (DataStruct *) PassedPtr;

    NumPlaces = ++(MyThreadStructPtr-> count);
    fprintf(stdout, "In Thread with Incremented Count of %d \n", NumPlaces);
    MyThreadStructPtr->DataPtr = malloc(NumPlaces * sizeof (int));

    for (i = 0; i < NumPlaces; i++) MyThreadStructPtr->DataPtr[i] = i;

    fprintf(stdout, "Thread is done and said so \n");
    pthread_exit(MyThreadStructPtr);
}

int main(int argc, char *argv[]) {
    pthread_t threadA; // need to name the thread
    pthread_t *threadPtr;
    int rc, i; // return code and working index

    DataStruct MainDataStruct; // A Thing 
    DataStruct *MainDataStructPtr; // And a pointer to a thing

    MainDataStruct.count = 5; // filled with data used in Thread 

    // Send to a thread for use

    fprintf(stdout, "In main: creating thread with count %d \n", MainDataStruct.count);
    if ((rc = pthread_create(&threadA, NULL, ThreadA, &MainDataStruct))) {
        fprintf(stdout, "ERROR; return code from pthread_create() for ThreadA is %d\n", rc);
        exit(-1);
    }
    // Send it off to do some work 


    // and block until it finishes

    (void) pthread_join(threadA, (void *) &MainDataStructPtr);
    fprintf(stdout, "Upon Join %d is Count \n", (int) ((*MainDataStructPtr).count));
    fprintf(stdout, "Back From Join with MainDataStruct Filled ?\n");

    for (i = 0; i < (*MainDataStructPtr).count; i++)
        fprintf(stdout, "%d\n", (*MainDataStructPtr).DataPtr[i]);



    //	exit(0);
}
