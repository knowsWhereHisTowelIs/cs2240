#ifndef nu_malloc_h
    #define nu_malloc_h

    #include <stdio.h>
    #include <assert.h>
    #include <string.h>
    #include <sys/types.h>
    #include <unistd.h>

    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_YELLOW  "\x1b[33m"
    #define ANSI_COLOR_BLUE    "\x1b[34m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_CYAN    "\x1b[36m"
    #define ANSI_COLOR_RESET   "\x1b[0m"

    //prototypes
    void *nu_malloc(size_t size);
    void nu_free(void *ptr);
    void *nu_realloc(void *ptr, size_t size);
    void *nu_calloc(size_t nelem, size_t elsize);

    typedef struct blockHeader {
        size_t size;
        struct blockHeader *next;
        int free;
    } blockHeaderType;
    typedef blockHeaderType *blockHeaderPtr;
    #define BLOCK_HEADER_SIZE sizeof(struct blockHeader)


    //prototypes
    blockHeaderPtr findFreeBlock(struct blockHeader **last, size_t size);
    blockHeaderPtr requestSpace(struct blockHeader* last, size_t size);
    blockHeaderPtr getBlockPtr(void *ptr);

#endif /* nu_malloc_h */
