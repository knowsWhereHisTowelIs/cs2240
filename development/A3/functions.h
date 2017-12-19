#ifndef MY_FUNCTIONS_H
    #define MY_FUNCTIONS_H

    #define BUFFER_SIZE 512
    #define NEW_LINE_SEPARATER 10

    typedef struct country{
        // int id;
        char code[3];
        char *name;
        float lifeExpectancy;
        int population;
        // char continent[20];
        // char region[26];
    } countryType;
    typedef countryType *countryPtr;

    //prototypes
    void printToStandardOutput(char *text, int textSize, char errorMsg[]);
    int myOpenFile(char *fileName, int type);
    int myCloseFile(int filePointer);
    void prepend(char *s, const char *t);
    countryType* fillAStruct(char *Line);
    char* gimmeALine(int filePointer);

#endif

#ifndef bool
    typedef int bool;
    #define true 1
#endif
    #define false 0
