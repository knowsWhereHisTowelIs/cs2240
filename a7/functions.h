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
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <err.h>

    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_YELLOW  "\x1b[33m"
    #define ANSI_COLOR_BLUE    "\x1b[34m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_CYAN    "\x1b[36m"
    #define ANSI_COLOR_RESET   "\x1b[0m"
    #define max(a,b) (a>b?a:b)

    #define BUFFER_SIZE 4096
    #define HTDOCS_RELATIVE_PATH "./htdocs"

    #define SERVER "webserver/1.0"
    #define PROTOCOL "HTTP/1.0"
    #define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"
    #define PORT 4000

    #define MAX_HITS_UNTIL_BLOCKED 9001
    #define BLOCKED_CLIENT_STATUS -1
    #define ACCEPTED_CLIENT_STATUS 1

    typedef int boolean;
    #define true 1
    #define false 0

    // Define listening port
    #define PORT 4000

    //typedefs

    typedef struct clientAccount {
        int ip;
        int hits;
    } clientAccountType;
    typedef clientAccountType* clientsAcountPtr;

    typedef struct {
        int clientFd;
        struct sockaddr_in *clientAddr;
    } clientRequestType;

    //prototypes
    void sampleHtmlResponse(FILE *responseFile);
    void sampleFileResponse(FILE *responseFile);
    void sendHeaders(FILE *f, int status, char *title, char *extra, char *mime, int length);
    char *getMimeType(char *name);
    void sendError(FILE *responseFile, int status, char *title, char *extra, char *text);
    void sendFile(FILE *responseFile, char *path, struct stat *statbuf);
    void* processRequest(void *argument);
#endif /* nu_malloc_h */
