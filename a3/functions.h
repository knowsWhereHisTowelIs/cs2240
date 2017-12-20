#ifndef MY_FUNCTIONS_H
    #define MY_FUNCTIONS_H

    #define BUFFER_SIZE 512
    #define NEW_LINE_SEPARATER 10
    #define COUNTRIES_DEFAULT_SIZE 50

    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_YELLOW  "\x1b[33m"
    #define ANSI_COLOR_BLUE    "\x1b[34m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_CYAN    "\x1b[36m"
    #define ANSI_COLOR_RESET   "\x1b[0m"

    #define COUNTRY_CODE_LENGTH 4
    #define COUNTRIES_BIN "BINARYCOUNTRIES.bin"
    #define COUNTRIES_DIR "DIRECTORY"
    #define COUNTRY_NOT_AVAILABLE_TEXT "N/A"

    typedef struct country{
        // int id;
        char code[COUNTRY_CODE_LENGTH];
        float lifeExpectancy;
        int population;
        // char continent[20];
        // char region[26];
        char *name;
    } countryType;
    typedef countryType *countryPtr;

    //prototypes
    void printToStandardOutput(char *text, int textSize, char errorMsg[]);
    int myOpenFile(char *fileName, int type);
    int myCloseFile(int filePointer);
    void prepend(char *s, const char *t);
    countryType* fillAStruct(char *Line);
    char* gimmeALine(int filePointer);
    countryPtr getCountryByCode(countryType  **countries, int numOfCountries, char *code);
    void writeAStruct(countryPtr country);
    countryType* readCountriesFromDirectory(char code[]);
    int myOpenCreateableFile(char *fileName, int permissions);
    void myWrite(int handle, void  *buffer, int nbyte);

#endif

#ifndef bool
    typedef int bool;
    #define true 1
#endif
    #define false 0
