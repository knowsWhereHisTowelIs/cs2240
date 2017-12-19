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

#ifndef functions_h
#define functions_h

#include <stdio.h>

struct Country {
    char code[4];
    char *name;
    float mttf;
    int population;
};

char *gimmeALine(int);
struct Country *fillAStruct(char *);

#endif /* functions_h */
