/**
* Who: Caleb Slater
* What: A3 B
* When: 2015-10-01
* Where: CS 2240
* How: "$./b.out"
* Reads file. writes binary. reads binary on user request and outputs data
**/
#include "apue.h"
#include <fcntl.h>
// #include "error.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "functions.c"

//global variables
//end global variables

#define BIG_NUMBER 1024
#define SMALL_NUMBER 16
struct msg
{
    int code;
    char message[BIG_NUMBER];
};

int main(int argc, char *argv[]) {
    char* buffer;
    struct msg* messagelist;

    /* Allocate some memory from the heap */
    buffer = (char*)malloc(BIG_NUMBER);
    if (buffer == NULL) {
        fprintf(stderr, "Error - unable to allocate required memoryn");
    } else {
        strcpy(buffer, "This is the big number with a lot of space blah blah blah");
        printf("Buffer: %sn", buffer);
    }

    /* Reduce the size of the memory */
    char* smallbuffer = (char*)realloc(buffer, SMALL_NUMBER);
    if (smallbuffer == NULL) {
        fprintf(stderr, "Error - unable to allocate required memoryn");
    } else {
        printf("Small Buffer: %sn", smallbuffer);
        printf("Uh Oh, ran out of memoryn");
    }


    /* Allocate some memory from the heap */
    messagelist = (struct msg*)calloc(SMALL_NUMBER, sizeof(struct msg));
    if (messagelist == NULL) {
        fprintf(stderr, "Error - unable to allocate required memoryn");
    } else {
        printf("And Then Message and Code: %s%dn", messagelist->message, messagelist->code);
        printf("Buffer: %sn", buffer);
        printf("Small Buffer: %sn", smallbuffer);
        printf("Uh Oh, ran out of memoryn");

    }

    free(smallbuffer);
    free(messagelist);

    return EXIT_SUCCESS;
} //end main()
