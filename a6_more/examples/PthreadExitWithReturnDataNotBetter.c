// Demo of thread join and exit with data passing
// Mystery of C pointers remains ineffable ? THIS HAS A BUG
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
    DataStruct *MyThreadStructPtr;
    MyThreadStructPtr = (DataStruct *) PassedPtr;

    NumPlaces = ++(MyThreadStructPtr-> count);
    fprintf(stdout, "In Thread with Incremented Count of %d \n", NumPlaces);
    MyThreadStructPtr->DataPtr = malloc(NumPlaces * sizeof (int));

    for (i = 0; i < NumPlaces; i++) MyThreadStructPtr->DataPtr[i] = i;

    fprintf(stdout, "Thread is done and said so \n");
    pthread_exit(MyThreadStructPtr);
}
// ===========

int main(int argc, char *argv[]) {
    pthread_t threadA; // need to name the thread
    pthread_t threadPtr[MaxThread];

    int rc, i; // return code and working index

    DataStruct MainDataStruct; // A Thing 
    DataStruct *MainDataStructPtr; // And a pointer to a thing
    DataStruct ** DataStructPtrArray;

    DataStructPtrArray = malloc(MaxThread * sizeof ( DataStruct *));
    DataStructPtrArray[2] = malloc(sizeof (DataStruct));

    DataStructPtrArray[2]->count = 13;

    // Get a struct pointed to by [2]

    pthread_create(&threadPtr[2], NULL, ThreadA, DataStructPtrArray[2]);
    (void) pthread_join(threadPtr[2], (void *) &DataStructPtrArray[2]);

    fprintf(stdout, "Upon Join %d is Count \n", (int) ((*DataStructPtrArray[2]).count));
    fprintf(stdout, "Back From Join with MainDataStruct Filled ?\n");

    for (i = 0; i < (*DataStructPtrArray[2]).count; i++)
        fprintf(stdout, "%d\n", (*DataStructPtrArray[2]).DataPtr[i]);

    pthread_exit(NULL);

    //	exit(0);
}
