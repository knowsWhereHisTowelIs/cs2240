/*
 *   Who: Benjamin Masters
 *  What: A program that uses threads to sort the contents of files
 *  When: 2015/11/16
 *   Why: Because somebody thought this would be a good job for c
 *
 *  acknowledgments:
 *  Reading a directory code shamelessly stolen from StackOverflow: http://bit.ly/1HKQyUV
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>
#include "apue.h"

/* because I still like this */
typedef int boolean;
#define true 1
#define false 0

/* account info struct */
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

/* compare function for qsort() */
int compare(const void *p1, const void *p2) {
    Info *a = (Info *) p1;
    Info *b = (Info *) p2;

    if (a->index > b->index) {
        return 1;
    } else if (a->index < b->index) {
        return -1;
    } else {
        return 0;
    }
}

/* function that our threads run */
void *sort_this(void *thread_arg) {

    size_t array_size = 512;

    Path *p = (Path *) thread_arg;
    Info *data = (Info*) calloc(array_size, sizeof(Info));
    Info *temp_data = NULL;

    printf("Handling File: %s\n", p->filename);

    FILE *unsorted;
    char myline[512];
    char tempLine[512];
    char *fgets_status;
    char *saveptr1;
    char *username = NULL;
    char *password = NULL;
    char *bloodtype = NULL;
    char *domain = NULL;
    char *index = NULL;
    int index_num;
    int i = 0;

    // try to open the file
    unsorted = fopen(p->filename, "r");
    if (unsorted == NULL) {
        err_sys("Couldn't open file: %s", p);
        exit(EXIT_FAILURE);
    }

    // prime the loop
    fgets_status = fgets(myline, 100, unsorted);
    memcpy(tempLine, myline, strlen(myline));

    while (fgets_status != NULL) {

        // can't get realloc to work properly for some reason
        // so I just set the array_size to be large enough for our files
        if (i == array_size) {
            array_size *= 2;
            temp_data = realloc(data, array_size * sizeof(Info));
            if (temp_data == NULL) {
                err_sys("realloc() failed!");
                exit(EXIT_FAILURE);
            }

            data = temp_data;
        }

        // extract tokens
        username = strtok_r(tempLine, ",", &saveptr1);
        password = strtok_r(NULL, ",", &saveptr1);
        bloodtype = strtok_r(NULL, ",", &saveptr1);
        domain = strtok_r(NULL, ",", &saveptr1);
        index = strtok_r(NULL, ",", &saveptr1);

        index_num = atoi(index);

        // if we have a valid index, put things in our data[] array
        if (index_num != 0) {
            data[i].index = index_num;
            memcpy(data[i].username, username, strlen(username));
            memcpy(data[i].password, password, strlen(password));
            memcpy(data[i].bloodtype, bloodtype, strlen(bloodtype));
            memcpy(data[i].domain, domain, strlen(domain));
            i++;
        } else {
            printf("Found a bad record...\n");
            printf("--> %s,%s,%s,%s,%d\n", username, password, bloodtype, domain, index_num);
        }

        // read the next line and loop again
        fgets_status = fgets(myline, 100, unsorted);
        memcpy(tempLine, myline, strlen(myline));
    }

    // qsort the data array before returning it
    qsort(data, i, sizeof(Info), compare);

    fclose(unsorted);
    free(temp_data);

    pthread_exit(data);
}

/*
 * Determines if the filename is useful for our purposes
 */
boolean good_filename(char *filename) {
    if ((strcmp(filename, ".DS_Store") != 0) && (strcmp(filename, "sorted.yay") != 0)) {
        return true;
    } else {
        return false;
    }
}

/*
 * Returns the number of regular files in a directory
 */
int get_num_files(const char *path) {
    DIR *dir;
    int count = 0;
    struct dirent *ent;

    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG && (good_filename(ent->d_name))) {
                count++;
            }
        }
    }

    closedir(dir);

    return count;
}

/*
 * does lots of things!
 */
int main(int argc, const char * argv[]) {
    DIR *dir;
    struct dirent *ent;
    int i = 0;
    int j = 0;
    int k = 0;
    int num_files = 0;
    int chdir_status = 0;
    Info *sorted;
    Info *temp = NULL;

    FILE *fp;
    char *out_file = "sorted.yay";

    size_t array_size = 512;

    if (argc != 2) {
        printf("Please provide a single path as an argument.\n");
        printf("(output file will be placed here as well)");
        exit(EXIT_FAILURE);
    }

    // change directory
    chdir_status = chdir(argv[1]);
    if (chdir_status < 0) {
        err_sys("Couldn't open path: %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    printf("sorting the files at: %s\n", argv[1]);

    // get file count
    num_files = get_num_files(".");

    // create an array of structs to hold paths + file names
    Path *files = calloc(num_files, sizeof(Path));

    // number of threads needed
    pthread_t threads[num_files];

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // array of pointers to structs
    Info **accounts = (Info **) malloc(sizeof(Info*) * num_files);

    // our output array
    sorted = calloc(array_size, sizeof(Info));

    //  try to open and read from the directory we were given
    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG && (good_filename(ent->d_name))) {

                memcpy(files[i].filename, ent->d_name, strlen(ent->d_name));
                memcpy(files[i].path, argv[1], strlen(argv[1]));

                // create a new thread to process this file
                pthread_create(&threads[i], &attr, sort_this, (void*) &files[i]);
                i++;
            }
        }

        closedir(dir);
    } else {
        err_sys("Could not open the directory!\n");
        exit(EXIT_FAILURE);
    }

    // wait on our threads
    for (i = 0; i < num_files; i++) {
        pthread_join(threads[i], (void **)&accounts[i]);
    }

    // loop through the arrays our threads have given us and copy the
    // data to our sorted[] array
    for (i = 0; i < num_files; i++) {
        int index = accounts[i][j].index;
        while (index != 0) {

            if (k == array_size) {
                array_size *= 2;
                temp = realloc(sorted, array_size * sizeof(Info));
                sorted = temp;
            }

            sorted[k] = accounts[i][j];
            k++;
            j++;
            index = accounts[i][j].index;
        }

        // free each returned data[] array when we are done with it
        free(accounts[i]);

        j = 0;
    }

    // we've already used qsort once -- why not use it again?
    qsort(sorted, k, sizeof(Info), compare);

    // open a file for output
    fp = fopen(out_file, "w");
    if (fp == NULL) {
        err_sys("Couldn't open output file!");
        exit(EXIT_FAILURE);
    }

    // write data to the output file
    for (i = 0; i < k; i++) {
//        printf("%d,%s,%s,%s,%s\n", sorted[i].index, sorted[i].username, sorted[i].password, sorted[i].bloodtype, sorted[i].domain);
        fprintf(fp, "%s,", sorted[i].username);
        fprintf(fp, "%s,", sorted[i].password);
        fprintf(fp, "%s,", sorted[i].bloodtype);
        fprintf(fp, "%s,", sorted[i].domain);
        fprintf(fp, "%d\n", sorted[i].index);
    }

    // clean up
    fclose(fp);
    free(files);
    free(accounts);
    free(temp);

    pthread_exit(NULL);

    return 0;
}
