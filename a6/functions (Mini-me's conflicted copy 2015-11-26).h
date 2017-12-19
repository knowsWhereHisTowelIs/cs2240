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
    /* db row data info struct */
    typedef struct _info {
        char username[25];
        char password[25];
        char bloodtype[5];
        char domain[50];
        int index;
    } Info;

    /* file / path name struct */
    typedef struct _path {
        char path[1024];
        char filename[1024];
    } Path;

    #define DATA_ROW_SIZE sizeof(dataRowType)
    typedef int (*compfn)(const void*, const void*);

    typedef int boolean;
    #define true 1
    #define false 0

    //prototypes
    int compare(const void *p1, const void *p2);
    void *sortDbChunck(void *threadArg);
    boolean isValidFileForUse(char *filename);
    int getNumFilesInDir(const char *path);
#endif /* nu_malloc_h */
