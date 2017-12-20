/**
 * Who: Caleb Slater
 * What: Read file then output it line by line
 * When: 2015-09-16
 * Where: CS 2240
 * How: "$./a.out"
 * Reads file line by line and outputs line + struct
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
static char DEFAULT_FILE[] = "AllCountries.dat";
//end global variables

int main(int argc, char *argv[]) {
	char *fileName, input[3];
	int i, j, hasGetInput = 1, foundCountries = 0, filePointer, linesToRead = 239, numOfCountries = COUNTRIES_DEFAULT_SIZE;
	countryType  **countries = (countryType  **)malloc(COUNTRIES_DEFAULT_SIZE * sizeof(countryType));
	if( argc > 1 ) {
		fileName = argv[1];
	} else {
		fileName = DEFAULT_FILE;
	}

	if( (filePointer = myOpenFile(fileName, O_RDONLY)) > 0 ) {
		for( i = 0; i < linesToRead; i++ ) {
			// printf("\n\n-----CALL GIMMEALINE----");
			char *line = gimmeALine(filePointer);
			countryPtr country;
			country = fillAStruct(line);
			if( i >= numOfCountries ) {
				numOfCountries = (ceil(i / COUNTRIES_DEFAULT_SIZE) + 1) * COUNTRIES_DEFAULT_SIZE;
				*countries = realloc(*countries, numOfCountries * sizeof(countryType));
				// printf("\n\t" ANSI_COLOR_RED "Countries: %d" ANSI_COLOR_RESET, numOfCountries);
			}
			// printf("\n\tLine[%d] currentLineNum:%d\n\t\t'%s'\n", i+1, currentLineNum, line);
			countries[i] = calloc(numOfCountries, sizeof(countryType * ));
			countries[i] = country;
		}
		foundCountries = i;
	} else {
		// printToStandardOutput("\nProgram failed. Invalid file.\n", 8, "Failed outputing failed error");
	}

	for (i = 1; i < foundCountries; i++) {
      	for (j = 0; j < foundCountries - i; j++) {
			countryPtr countryA = countries[j];
			countryPtr countryB = countries[j+1];
         	if (strcmp(countryA->code, countryB->code) > 0) {
            	countryPtr temp = countries[j];
				countries[j] = countries[j+1];
				countries[j+1] = temp;
			}
     	}
  	}

	while( hasGetInput ) {
		printf("\n\nEnter 3 Letter Code:");
  		scanf("%s", input);
		if( strcmp(input, "ZZZ") != 0 ) {
			countryPtr country = (countryPtr) getCountryByCode( countries, linesToRead, input);
			printf("\nRetrieved:");
			printf("\nCountry:%s", country->name);
			printf("\nCode:%s", country->code);
			printf("\nMTTF:%f", country->lifeExpectancy);
			printf("\nPopulation:%d", country->population);
		} else {
			hasGetInput = 0;
		}
	}

	return 0;
} //end main()
