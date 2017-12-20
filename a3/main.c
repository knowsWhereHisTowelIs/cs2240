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
static char DEFAULT_FILE[] = "AllCountries.dat";
//end global variables

int main(int argc, char *argv[]) {
	char *fileName, input[3];
	int i, j, hasGetInput = 1, foundCountries = 0, filePointer, linesToRead = 240, numOfCountries = COUNTRIES_DEFAULT_SIZE;
	countryType  **countries = (countryType  **)malloc(COUNTRIES_DEFAULT_SIZE * sizeof(countryType));
	if( argc > 1 ) {
		fileName = argv[1];
	} else {
		fileName = DEFAULT_FILE;
	}

	//read data in and write to binaries
	if( (filePointer = myOpenFile(fileName, O_RDONLY)) > 0 ) {
		for( i = 0; i < linesToRead; i++ ) {
			char *line = gimmeALine(filePointer);
			countryPtr country;
			country = fillAStruct(line);
			if( i >= numOfCountries ) {
				//allocate more memory for countries
				numOfCountries = (ceil(i / COUNTRIES_DEFAULT_SIZE) + 1) * COUNTRIES_DEFAULT_SIZE;
				*countries = realloc(*countries, numOfCountries * sizeof(countryType));
			}
			countries[i] = calloc(numOfCountries, sizeof(countryType * ));
			countries[i] = country;
			writeAStruct(country);
			// if( i > (linesToRead-6) ) { printf("\nCode:%s name[%d]:%s", country->code, strlen(country->name), country->name); }
			// if( i > (linesToRead-4) ) { break; }
		}
		foundCountries = i;
	} else {
		printf("\nProgram failed. Invalid file.\n");
	}

	//sort countries
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

	//Get user input and output country data
	while( hasGetInput ) {
		printf("\nEnter 3 Letter Code:");
  		scanf("%s", input);
		if( strcmp(input, "ZZZ") != 0 ) {
			readCountriesFromDirectory(input);
			countryPtr country = (countryPtr) getCountryByCode( countries, linesToRead, input);
			printf("Retrieved:");
			printf("\nCountry:%s", country->name);
			printf("\nCode:%s", country->code);
			printf("\nMTTF:%f", country->lifeExpectancy);
			printf("\nPopulation:%d", country->population);
			printf("\n");
		} else {
			hasGetInput = 0;
		}
	}

	return 0;
} //end main()
