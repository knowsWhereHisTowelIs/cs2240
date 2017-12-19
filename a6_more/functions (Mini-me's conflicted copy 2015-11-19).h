#ifndef FUNCTIONS_H
    #define FUNCTIONS_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <assert.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <limits.h>
    #include <dirent.h>
    #include <pthread.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <math.h>
    #include <limits.h>

    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_YELLOW  "\x1b[33m"
    #define ANSI_COLOR_BLUE    "\x1b[34m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_CYAN    "\x1b[36m"
    #define ANSI_COLOR_RESET   "\x1b[0m"
    #define max(a,b) (a>b?a:b)

    typedef int boolean;
    #define true 1
    #define false 0
    #define SHELL_BUFFER_FILE_NAME "MY_SHELL_BUFFER"

    #define BUFFER_SIZE 4096
    #define HUGE_BUFFER_SIZE 8192
    #define PARSED_ARGS_SIZE 32
    #define DATA_ROWS_INITIAL_SIZE 1024

    //structures
    typedef struct dataRow {
        char username[256];
        char password[256];
        char bloodType[64];
        char domain[256];
        int id; //dataBaseIndex;
    } dataRowType;
    typedef dataRowType *dataRowPtr;

    typedef struct threadData {
        int id;
        char filePath[512];
        dataRowType **dataRows;
        int dataRowsSize;
        char test;
    } threadDataType;
    typedef threadDataType *threadDataPtr;

    #define DATA_ROW_SIZE sizeof(dataRowType)
    typedef int (*compfn)(const void*, const void*);

    typedef int boolean;
    #define true 1
    #define false 0

    typedef struct _info {
        char username[50];
        char password[50];
        char bloodtype[5];
        char domain[100];
        short index;
    } Info;

    //prototypes
    void threadSortFile(threadDataPtr data);
    char* gimmeALine(int filePointer);
    void fillDataRowFromLine(char *line, dataRowPtr row);
    void sortDataFilesFromDir();
    void sortDataRows(dataRowType **rows, int numInArr);
    int countFilesInDirectory(DIR *dir);
#endif /* nu_malloc_h */
