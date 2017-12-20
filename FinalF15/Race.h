#ifndef RACE_H
    #define RACE_H

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
    
    typedef struct {
        int index;
        int fd;
        int offset;
    } threadParamType;
    #define ARR_SIZE 5

#endif /* RACE_H */

