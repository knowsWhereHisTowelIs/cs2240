#include "functions.h"

/* compare function for qsort() */
int compare(const void *p1, const void *p2) {
    Info *a = (Info *) p1;
    Info *b = (Info *) p2;
    return a->index - b->index;
}

/* function that our threads run */
void *sortDbChunck(void *threadArg) {
    size_t chunkArraySize = 512;

    Path *p = (Path *) threadArg;
    Info *data = (Info*) calloc(chunkArraySize, sizeof(Info));
    Info *tempData = NULL;

    printf("Handling File: %s\n", p->filename);

    FILE *unsortedDbChunk;
    char myline[512];
    char tempLine[512];
    char *fgetsStatus;
    char *tokenPtr;
    
    //struct tmp fields
    char *username = NULL, *password = NULL, *bloodtype = NULL, *domain = NULL, *index = NULL;
    
    int indexAsInt;
    int i = 0;

    // try to open the file
    if ( (unsortedDbChunk = fopen(p->filename, "r")) == NULL) {
        err_sys("Couldn't open file: %s", p);
        exit(EXIT_FAILURE);
    }
    
    while ( (fgetsStatus = fgets(myline, 100, unsortedDbChunk)) != NULL) { //read line from file
        memcpy(tempLine, myline, strlen(myline));
        
        //realloc if file is larger than chunkArraySize entries
        if (i == chunkArraySize) {
            chunkArraySize *= 2;
            tempData = realloc(data, chunkArraySize * sizeof(Info));
            if (tempData == NULL) {
                err_sys("realloc() failed!");
                exit(EXIT_FAILURE);
            }
            data = tempData;
        }

        // copy csv to to tmp strings
        username = strtok_r(tempLine, ",", &tokenPtr);
        password = strtok_r(NULL, ",", &tokenPtr);
        bloodtype = strtok_r(NULL, ",", &tokenPtr);
        domain = strtok_r(NULL, ",", &tokenPtr);
        index = strtok_r(NULL, ",", &tokenPtr);

        //str to int to compare if valid row
        indexAsInt = atoi(index);
        
        // if we have a valid index, put things in our data[] array
        if (indexAsInt != 0) {
            data[i].index = indexAsInt;
            memcpy(data[i].username, username, strlen(username));
            memcpy(data[i].password, password, strlen(password));
            memcpy(data[i].bloodtype, bloodtype, strlen(bloodtype));
            memcpy(data[i].domain, domain, strlen(domain));
            i++;
        } else {
            printf("Bad line:\n\t%s\n", myline);
        }
    }

    // sort the data array before returning it
    qsort(data, i, sizeof(Info), compare);

    //KILL ALL THE THREAD THINGS
    fclose(unsortedDbChunk);
    free(tempData);

    pthread_exit(data);
}

/*
 * Determines if the filename is useful for our purposes
 */
boolean isValidFileForUse(char *filename) {
    return (strcmp(filename, ".DS_Store") != 0) && (strcmp(filename, "sorted.yay") != 0);
}

/*
 * Returns the number of regular files in a directory
 */
int getNumFilesInDir(const char *path) {
    DIR *dir;
    int count = 0;
    struct dirent *ent;

    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG && isValidFileForUse(ent->d_name)) {
                count++;
            }
        }
    }

    closedir(dir);

    return count;
}
