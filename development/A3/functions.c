#include "functions.h"

//global variables
int currentLineNum = 1, currentLineInBuffer = 0, currentLineInBufferNum = 0;
static int bufferIndex = -1;
char buffer[BUFFER_SIZE], leftOverLine[BUFFER_SIZE];

countryType **countries[50];
//end global variables

/**
 * Note lines must be less than BUFFER_SIZE in order to return a full line
**/
char* gimmeALine(int filePointer) {
	char *line = malloc(BUFFER_SIZE), continueReadingFile = 1, tmpLine[BUFFER_SIZE];
	int n, lineNum = 1;
	memset(&tmpLine[0], '\0', sizeof(tmpLine)); //clean buffer incase buffer isn't filled completly -> not bad memory

	while ( continueReadingFile == 1 ) {
		if( bufferIndex == -1 || (bufferIndex) >= (int) BUFFER_SIZE ) { //if is first run or need more buffer
			memset(&buffer[0], '\0', sizeof(buffer)); //clean buffer incase buffer isn't filled completly -> not bad memory
			if( (n = read(filePointer, buffer, BUFFER_SIZE)) > 0 ) {
				//read next buffer length
				currentLineInBufferNum = 0;
			} else {
				continueReadingFile = 0;
			}
		}
		int lastLineIndex = 0;

		currentLineInBuffer = 0;
		for( bufferIndex = 0; bufferIndex < ((int) BUFFER_SIZE); bufferIndex++ ) {
			if( buffer[bufferIndex] == '\n' ) { //split on new line
				//clear next line so buffer fills up for next iteration
				if( currentLineInBuffer == currentLineInBufferNum ) {
					int lineStartIndex = lastLineIndex;
					if( buffer[lineStartIndex] == '\n' ) {
						lineStartIndex++; //if not first line curreint index is \n
					}
					int lineLength = max(0, bufferIndex-lineStartIndex);

					memcpy(tmpLine, &buffer[lineStartIndex], lineLength );
					strcpy(line, leftOverLine);
					strcat(line, tmpLine);

					//increment and break
					currentLineNum++;
					currentLineInBufferNum++;
					continueReadingFile = 0;
					break;
				} else {
					lastLineIndex = bufferIndex;
				}
				lineNum++;
				currentLineInBuffer++;
				memset( &leftOverLine[0], '\0', sizeof(leftOverLine) ); //clean buffer incase buffer isn't filled completly -> not bad memory
			}
		}
		//strcpy(leftOverLine, tmp);
		int leftOverLineStartIndex = lastLineIndex;
		if( lastLineIndex + 1 < (int) BUFFER_SIZE ) { //igonre newline char if possilbe
			leftOverLineStartIndex++;
		}
		memcpy(leftOverLine, &buffer[leftOverLineStartIndex], bufferIndex-leftOverLineStartIndex);
	}
	return line;
}

/**
 * Fill a struct w/csv line
**/
countryType* fillAStruct(char *line){
	countryType country;
	//set defaults
	strcpy(country.code, "   ");
	country.name = NULL;
	country.population = 0;
	country.lifeExpectancy = 0;

	int fieldCount;
    char *token;
    // Now let's use that linechar *Name
    char *lineCopy = malloc(strlen(line) + 1);
    strncpy(lineCopy, line, strlen(line) + 1);
    while ((token = strtok_r(lineCopy, ",", &lineCopy)) != NULL) {
        switch (fieldCount) {
            // case 0:
            //     country.id = atoi(token);
            //     break;
            case 1:
                strncpy(country.code, token, strlen(token));
                break;
            case 2:
				country.name = malloc(strlen(token));
                strncpy(country.name, token, strlen(token));
                break;
            // case 3:
            //     strncpy(country.continent, token, strlen(token));
            //     break;
            // case 4:
            //     strncpy(country.region, token, strlen(token));
            //     break;
            case 7:
                country.population = atoi(token);
                break;
            case 8:
                country.lifeExpectancy = atof(token);
                break;
            default:
                break;
        }
        fieldCount++;
    }

	countryPtr countryPtr = malloc(sizeof(country));
	*countryPtr = country;
	return countryPtr;
}

/**
 * Prints to standard output
**/
void printToStandardOutput(char *text, int textSize, char errorMsg[]) {
	if ( write(STDOUT_FILENO, text, textSize) != textSize ) {
		err_sys(errorMsg);
	}
	return;
} //end printToStandardOutput()

/**
 * Open file and return its file pointer
**/
int myOpenFile(char *fileName, int permissions) {
	int filePointer;
	if( (filePointer = open(fileName, permissions)) < 0 ) {
		err_sys("Error opening file");
	}
	return filePointer;
} //end myOpenFile()

/**
 * Close file opened
**/
int myCloseFile(int filePointer) {
	int success;

	if(close(filePointer) < 0) {
		success = 0;
		err_sys("Error closing file");
	} else {
		success = 1;
	}

	return success;
} //end myCloseFile()

/*
 * Prepends t into s. Assumes s has enough space allocated
 * for the combined string.
*/
void prepend(char* s, const char* t){
    size_t len = strlen(t);
    size_t i;

    memmove(s + len, s, strlen(s) + 1);

    for (i = 0; i < len; ++i)
    {
        s[i] = t[i];
    }
} //end prepend()
