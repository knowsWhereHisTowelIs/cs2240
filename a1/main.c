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

#include "functions.c"

//global variables
static char DEFAULT_FILE[] = "AllCountries.dat";
//end global variables

int main(int argc, char *argv[]) {
	char *fileName;
	if( argc > 1 ) {
		fileName = argv[1];
	} else {
		fileName = DEFAULT_FILE;
	}
	int i, filePointer;

	if( (filePointer = myOpenFile(fileName, O_RDONLY)) > 0 ) {
		for( i = 0; i < 245; i++ ) {
			// char *line = gimmeALine(fileName);
			printf("\n\n-----CALL GIMMEALINE----");
			char *line = gimmeALine(filePointer);
			countryPtr country;
			country = fillAStruct(line);
			printf("\n\t----STRUCT----\n\tCode:%s\n\tName:%s\n\tpopulation:%d\n\tlifeExpectancy:%f\n\t----END STRUCT----\n",
			 	country->code, country->name, country->population, country->lifeExpectancy
			);
			printf("\n\tLine[%d] currentLineNum:%d\n\t\t'%s'\n", i+1, currentLineNum, line);
		}
	} else {
		printToStandardOutput("\nProgram failed. Invalid file.\n", 8, "Failed outputing failed error");
	}
	printf("\n---------------------\nDONE\n");

	return 0;
} //end main()
