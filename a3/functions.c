#include "functions.h"

//global variables
int currentLineNum = 1;
int currentLineInBuffer = 0;
int currentLineInBufferNum = 0;
static int bufferIndex = -1;
const int COUNTRY_BIN_ROW_LENGTH = COUNTRY_CODE_LENGTH + 2 * sizeof (int); //length of code+countryDirOffset+dataLength
char buffer[BUFFER_SIZE], leftOverLine[BUFFER_SIZE];

static int countryBinOffset = 0;
static int countryDirOffset = 0;
//end global variables

/**
 * Note lines must be less than BUFFER_SIZE in order to return a full line
 **/
char* gimmeALine(int filePointer) {
    char *line = malloc(BUFFER_SIZE), continueReadingFile = 1, tmpLine[BUFFER_SIZE];
    int n, lineNum = 1;
    memset(&tmpLine[0], '\0', sizeof (tmpLine)); //clean buffer incase buffer isn't filled completly -> not bad memory

    while (continueReadingFile == 1) {
        if (bufferIndex == -1 || (bufferIndex) >= (int) BUFFER_SIZE) { //if is first run or need more buffer
            memset(&buffer[0], '\0', sizeof (buffer)); //clean buffer incase buffer isn't filled completly -> not bad memory
            if ((n = read(filePointer, buffer, BUFFER_SIZE)) > 0) {
                //read next buffer length
                currentLineInBufferNum = 0;
            } else {
                continueReadingFile = 0;
            }
        }
        int lastLineIndex = 0;

        currentLineInBuffer = 0;
        for (bufferIndex = 0; bufferIndex < ((int) BUFFER_SIZE); bufferIndex++) {
            if (buffer[bufferIndex] == '\n') { //split on new line
                //clear next line so buffer fills up for next iteration
                if (currentLineInBuffer == currentLineInBufferNum) {
                    int lineStartIndex = lastLineIndex;
                    if (buffer[lineStartIndex] == '\n') {
                        lineStartIndex++; //if not first line curreint index is \n
                    }
                    int lineLength = max(0, bufferIndex - lineStartIndex);

                    memcpy(tmpLine, &buffer[lineStartIndex], lineLength);
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
                memset(&leftOverLine[0], '\0', sizeof (leftOverLine)); //clean buffer incase buffer isn't filled completly -> not bad memory
            }
        }
        //strcpy(leftOverLine, tmp);
        int leftOverLineStartIndex = lastLineIndex;
        if (lastLineIndex + 1 < (int) BUFFER_SIZE) { //igonre newline char if possilbe
            leftOverLineStartIndex++;
        }
        memcpy(leftOverLine, &buffer[leftOverLineStartIndex], bufferIndex - leftOverLineStartIndex);
    }
    return line;
}

void writeAStruct(countryPtr country) {
    int FD = myOpenCreateableFile(COUNTRIES_BIN, O_CREAT | O_RDWR);
    if (FD > 0) {
        // declare all variables
        // note vars ending in 'Size' are bytes of var for writing
        int codeSize = sizeof (country->code);
        int popSize = sizeof (country->population);
        int lifeSize = sizeof (country->lifeExpectancy);
        int nameSize = strlen(country->name);
        int countryBufferLength = codeSize + popSize + lifeSize + nameSize;
        int countryBufferSize = sizeof (countryBufferLength);
        int countryDirOffsetSize = sizeof (countryDirOffset);

        //after file is open set index to where we left off
        lseek(FD, countryBinOffset, SEEK_SET);
        //write binary data
        myWrite(FD, country->code, codeSize);
        myWrite(FD, &countryDirOffset, countryDirOffsetSize);
        myWrite(FD, &countryBufferLength, countryBufferSize);
        countryBinOffset += COUNTRY_BIN_ROW_LENGTH;
        myCloseFile(FD);
        //close binary file after incrementing offset

        //if found in COUNTRIES_BIN then write into directory
        int countryBinaryFilePointer = myOpenCreateableFile(COUNTRIES_DIR, O_CREAT | O_RDWR);
        if (countryBinaryFilePointer > 0) {
            //set index to where we left off
            lseek(countryBinaryFilePointer, countryDirOffset, SEEK_SET);
            //write binary data
            myWrite(countryBinaryFilePointer, country->code, codeSize);
            myWrite(countryBinaryFilePointer, &country->population, popSize);
            myWrite(countryBinaryFilePointer, &country->lifeExpectancy, lifeSize);
            myWrite(countryBinaryFilePointer, country->name, nameSize);

            // 	printf("\nname[%d]:%s\n", nameSize, country->name);
            // if( strcmp(country->code, "AGO") == 0 ) {
            // 	exit(1);
            // }
            myCloseFile(countryBinaryFilePointer);
            countryDirOffset += countryBufferLength;
            //close binary file after incrementing offset
        }
    }
}

countryType* readCountriesFromDirectory(char code[]) {
    // declare all variables
    // note vars ending in 'Size' are bytes of var for writing
    countryType country;
    int codeSize = COUNTRY_CODE_LENGTH;
    int popSize = sizeof (int);
    int lifeSize = sizeof (float);
    int n;
    int bufferSize = 1;
    int offset = -1;
    int dataLength = -1;
    int continueReadingBin = 1;

    char buffer[bufferSize];
    memset(&buffer[0], '\0', COUNTRY_BIN_ROW_LENGTH); //clean buffer incase buffer isn't filled completly -> not bad memory
    char currentCode[COUNTRY_CODE_LENGTH];
    memset(&currentCode[0], '\0', COUNTRY_CODE_LENGTH); //clean buffer incase buffer isn't filled completly -> not bad memory

    //START READING bin to fine code/offset/length data
    int FD = myOpenFile(COUNTRIES_BIN, O_CREAT | O_RDWR);
    if (FD > 0) {
        //loop thru binary till we find requested country code
        while (continueReadingBin == 1) {
            if ((n = read(FD, buffer, COUNTRY_BIN_ROW_LENGTH)) > 0) {
                //save vars into c so can use binary data
                memcpy(currentCode, &buffer[0], COUNTRY_CODE_LENGTH);
                if (strcasecmp(currentCode, code) == 0) {
                    //we have the country copy rest of data
                    memcpy(&offset, &buffer[COUNTRY_CODE_LENGTH], sizeof (int));
                    memcpy(&dataLength, &buffer[COUNTRY_CODE_LENGTH + sizeof (int)], sizeof (int));
                    continueReadingBin = 0;
                }
            } else {
                //reached EOF
                continueReadingBin = 0;
            }
        }
        myCloseFile(FD);
        //close binary

        int directoryFD = myOpenFile(COUNTRIES_DIR, O_CREAT | O_RDWR);
        if (offset >= 0 && dataLength > 0 && directoryFD > 0) {
            lseek(directoryFD, offset, SEEK_SET);
            if ((n = read(directoryFD, buffer, dataLength)) > 0) {
                //read binary file into struct
                memcpy(country.code, &buffer[0], codeSize);
                country.code[3] = '\0'; //only save 3 chars to binary make last null so can be printed w/'%s'
                memcpy(&country.population, &buffer[codeSize], popSize);
                memcpy(&country.lifeExpectancy, &buffer[codeSize + popSize], lifeSize);

                int nameIndexStart = codeSize + popSize + lifeSize; //  where it begins
                int nameLength = n - nameIndexStart; // calculate length
                country.name = malloc(nameLength); //allocate memory
                memcpy(country.name, &buffer[nameIndexStart], nameLength); //save to struct
            } else {
            }
            myCloseFile(directoryFD);
        } else {
            // country wasn't found
        }
    } else {
        printf("Error opening binary\n");
    }

    countryPtr countryPtr = malloc(sizeof (country));
    *countryPtr = country;
    return countryPtr; //save to pointer and return
}

/**
 * Fill a struct w/csv line
 **/
countryType* fillAStruct(char *line) {
    countryType country;
    //set defaults
    strcpy(country.code, COUNTRY_NOT_AVAILABLE_TEXT);
    country.name = malloc(strlen(COUNTRY_NOT_AVAILABLE_TEXT));
    strcpy(country.name, COUNTRY_NOT_AVAILABLE_TEXT);
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

    countryPtr countryPtr = malloc(sizeof (country));
    *countryPtr = country;
    return countryPtr;
}

/**
 * Searches thru countries for country code returns countryPtr, if not in countries returns empty struct
 **/
countryPtr getCountryByCode(countryType **countries, int numOfCountries, char *code) {
    int i, countryIndex = -1;
    countryPtr country;
    for (i = 0; i < numOfCountries; i++) {
        country = countries[i];
        if (strcasecmp(country->code, code) == 0) {
            countryIndex = i;
            break;
        }
    }

    if (countryIndex == -1) { //not found
        countryType countryNull;
        strncpy(countryNull.code, "N/A\0", 4);

        countryNull.name = malloc(sizeof (char) * 4);
        strncpy(countryNull.name, "N/A\0", 4);

        countryNull.population = 0;
        countryNull.lifeExpectancy = 0;

        country = malloc(sizeof (countryNull));
        *country = countryNull;
    } else {
        country = countries[countryIndex];
    }
    return country;
} // end getCountryByCode()

/**
 * Prints to standard output
 **/
void printToStandardOutput(char *text, int textSize, char errorMsg[]) {
    if (write(STDOUT_FILENO, text, textSize) != textSize) {
        err_sys(errorMsg);
    }
    return;
} //end printToStandardOutput()

/**
 * Prints to file
 **/
void myWrite(int handle, void *buffer, int nbyte) {
    if (write(handle, buffer, nbyte) != nbyte) {
        err_sys("Error writing to file");
    }
    return;
} //end printToStandardOutput()

/**
 * Open file and return its file pointer
 **/
int myOpenFile(char *fileName, int permissions) {
    int filePointer;
    if ((filePointer = open(fileName, permissions)) < 0) {
        err_sys("Error opening file");
    }
    return filePointer;
} //end myOpenFile()

/**
 * Open file and return its file pointer, creates file if doesn't exist
 **/
int myOpenCreateableFile(char *fileName, int permissions) {
    int filePointer;
    if ((filePointer = open(fileName, permissions, 0777)) < 0) {
        err_sys("Error opening file");
    }
    return filePointer;
} //end myOpenFile()

/**
 * Close file opened
 **/
int myCloseFile(int filePointer) {
    int success;

    if (close(filePointer) < 0) {
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
void prepend(char* s, const char* t) {
    size_t len = strlen(t);
    size_t i;

    memmove(s + len, s, strlen(s) + 1);

    for (i = 0; i < len; ++i) {
        s[i] = t[i];
    }
} //end prepend()
