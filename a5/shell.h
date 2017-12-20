#ifndef nu_malloc_h
    #define nu_malloc_h

    #include <stdio.h>
    #include <stdlib.h>
    #include <assert.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <limits.h>
    #include <errno.h>
    #include <fcntl.h>

    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_YELLOW  "\x1b[33m"
    #define ANSI_COLOR_BLUE    "\x1b[34m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_CYAN    "\x1b[36m"
    #define ANSI_COLOR_RESET   "\x1b[0m"

    typedef int boolean;
    #define true 1
    #define false 0
    #define SHELL_BUFFER_FILE_NAME "MY_SHELL_BUFFER"

    #define BUFFER_SIZE 4096
    #define PARSED_ARGS_SIZE 32

    char *seperators[] = {"|", "<", ">", "&"};
    #define SEPERATORS_ARR_SIZE 4

    //structures
    //int main(int argc, char *argv[]);

    typedef struct blockHeader {
        size_t size;
        struct blockHeader *next;
        int free;
    } blockHeaderType;
    typedef blockHeaderType *blockHeaderPtr;
    #define BLOCK_HEADER_SIZE sizeof(struct blockHeader)


    //prototypes
    void parseArgs( char *buffer );
    void executeArgs();
    void execute(char *rawInput);
    void execute2();
    char **split_line(char *line);
    char *get_input();
    void refreshCwd();
    // void pipeEx2();
    void handleSingleProcess(char *input, char *previousOutput, char **output);
    void trim(char * s);
    char *trimAfterChar(char *string, char *pattern);
#endif /* nu_malloc_h */
