#ifndef MY_FUNCTIONS_H
    #define MY_FUNCTIONS_H

    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_YELLOW  "\x1b[33m"
    #define ANSI_COLOR_BLUE    "\x1b[34m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_CYAN    "\x1b[36m"
    #define ANSI_COLOR_RESET   "\x1b[0m"

    //prototypes
    void* nu_malloc(size_t size);
    void* nu_calloc(size_t count, size_t size);
    void* nu_realloc(void* ptr, size_t size);
    void nu_free(void* ptr);

#endif

#ifndef bool
    typedef int bool;
    #define true 1
#endif
    #define false 0
