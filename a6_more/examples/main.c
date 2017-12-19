/*
 *
 *
 *
 *  Reading a directory code shamelessly stolen from StackOverflow: http://bit.ly/1HKQyUV
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pthread.h>
#include "apue.h"

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

int main(int argc, const char * argv[]) {
    DIR *dir;
    struct dirent *ent;

    char *out_file = "sorted.yay";

    if (argc != 2) {
        printf("Please provide a single path as an argument.\n");
        exit(EXIT_FAILURE);
    }

    printf("sorting the files at: %s...\n", argv[1]);

    if ((dir = opendir(argv[1])) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                printf("%s\n", ent->d_name);
            }
        }

        closedir(dir);
    } else {
        err_sys("Could not open the directory!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
