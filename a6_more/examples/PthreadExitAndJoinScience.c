// Demo of thread join and exit with data passing
// Testing synchronization of join / exit with return
// and as a bonus the declaration of space for returns
// R. Trenary, 3/26/15
//  
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//==================================
//    OUR TEST THREAD
//    THIS RECEIVES AND RETURNS VALUES
//==================================

void *Thread(void * PassedValPtr) {
    long int V, *Vptr;
    // PROVE WE RECEIVE VALUE        
    V = *((long *) PassedValPtr);
    fprintf(stdout, "In Thread with Value Passed %ld \n", V);
    //  AND NOW RETURN VALUE 
    // *** LOOK HERE FOR VARIATIONS OF HOW WE DECLARE SPACE 
    Vptr = (long *) malloc(sizeof (long));
    // HERE WE HAVE USED MALLOC FOR THE RETURN VALUE FROM THREAD
    // WHICH MUST BE THE ADDRESS OF THE PLACE WE PUT A VALUE
    // OPTIONS WOULD BE 
    //  a) DECLARING V AS static AND RETURNING &V
    // WRONG
    //  b) USING V AS ABOVE AND RETURNING &V
    // *** FOR SOME INSIGHT INTO RETURNING VALUES 
    ++V; // CHANGE COPY OF VALUE SENT 
    (*Vptr) = V; // AND PUT IN SPACE ALLOCATED
    sleep(V); // SLEEP BEFORE RETURNING VALUE WITH EXIT
    fprintf(stdout, "Thread is done and said so returning %ld\n", V);
    pthread_exit((void *) Vptr);
}
// ========= MAIN ============

int main(int argc, char *argv[]) {
    pthread_t threadA, threadB; // need to name the thread
    int rc; // RETURN CODE 
    long MainVA, MainRetVA, MainVB, MainRetVB;
    // LOCAL VALUES SENT AND RECEIVED
    void * MainRetVAPtr, *MainRetVBPtr;
    // AND ADDRESSES TO SAME 
    MainVA = 10;
    MainVB = 1; // TEST VALUES 
    // CREATE TWO THREADS SENDING TWO VALUES BY REFERENCE 
    // THE THREADS WILL SLEEP ACCORDING TO VALUES SENT 
    fprintf(stdout, "In main: creating two threads with Values %ld %ld \n", MainVA, MainVB);
    if ((rc = pthread_create(&threadA, NULL, Thread, &MainVA))) {
        fprintf(stdout, "ERROR; return pthread_create()");
        exit(-1);
    }
    if ((rc = pthread_create(&threadB, NULL, Thread, &MainVB))) {
        fprintf(stdout, "ERROR; return pthread_create() ");
        exit(-1);
    }
    // Two threads created -- now we join each
    // But B exits before we join it -- IS THAT OK ? 
    (void) pthread_join(threadA, (void *) &MainRetVAPtr);
    MainRetVA = *((long *) MainRetVAPtr);
    fprintf(stdout, "Upon Joining A %ld is Count \n", MainRetVA);
    (void) pthread_join(threadB, (void *) &MainRetVBPtr);
    MainRetVB = *((long *) MainRetVBPtr);
    fprintf(stdout, "Upon Joining B %ld is Count \n", MainRetVB);

    // Let's make sure we exit main 
    pthread_exit(NULL);

}
