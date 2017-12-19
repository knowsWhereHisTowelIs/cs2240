/**
 * Benjamin Masters
 * 2015/09/22
 * CS2240 | Project A3 | part a
 *
 * This assignment will weave together the two modules that
 * read lines from the AllCountries.dat file, creating a CountryStruct
 * for each country found in the buffered lines.
 * (Straight from the director's mouth)
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "apue.h"

/**
 * Take a string of data, pick off tokens, 
 * fill a struct with those tokens and return 
 * a pointer to it.
 */
struct Country *fillAStruct(char *data) {
    char *token;                // holds the data from strtok_r
    char *savePtr;              // needed by strtok_r
    char *temp = strdup(data);  // copy the string to this before tokenizing
    short i = 0;                // token number (position in the string)

    struct Country *c1 = malloc(sizeof(struct Country));
    if (c1 == NULL) {
        err_sys("struct_thing.c -> c1 malloc failed");
    }

    // prime the loop
    token = strtok_r(temp, ",", &savePtr);
    while (token != NULL) {
        switch (i) {
            case 1:
                memcpy(c1->code, token, 4);
                token = strtok_r(NULL, ",", &savePtr);
                i += 1;
                break;
            case 2:
                c1->name = strdup(token);
                token = strtok_r(NULL, ",", &savePtr);
                i += 1;
                break;
            case 7:
                c1->population = atoi(token);
                token = strtok_r(NULL, ",", &savePtr);
                i += 1;
                break;
            case 8:
                c1->mttf = atof(token);
                token = strtok_r(NULL, ",", &savePtr);
                i += 1;
                break;
            default:
                token = strtok_r(NULL, ",", &savePtr);
                i += 1;
                break;
        }
    }

    free(temp);
    return c1;
}
