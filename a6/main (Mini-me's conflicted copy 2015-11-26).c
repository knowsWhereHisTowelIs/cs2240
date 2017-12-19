/**
 * Who: Caleb Slater
 * What: A6
 * When: 2015-11-24
 * Where: CS 2240
 * How: "$./a.out data"
 * Takes a directory full of files that are chunks from database
 * Using threads to handle each file, parse each entry into a struct then sort chunk
 * Finally sort all chuncks and write to 'sorted.yay' in passed directory
 * Then celebrate
 **/
#include "functions.h"

/*
 * Dispatcher
 */
int main(int argc, char * argv[]) {
    DIR *dir;
    struct dirent *ent;
    int i = 0, j = 0, k = 0;
    int numOfFilesInDir = 0;
    int chdirStatus = 0;
    Info *sorted;
    Info *temp = NULL;

    FILE *fp;
    char *outputFilename = "sorted.yay";
    char *dirName = "data";

    size_t dbArraySize = 512;

    if (argc != 2) {
        printf("Please provide a single path as an argument in future (defaulting to ./data/)\n");
        printf("(output file will be placed here as well)\n");
        // exit(EXIT_FAILURE);
    } else {
        dirName = argv[1];
    }

    // IMPORTANT change cwd to passed directory
    chdirStatus = chdir(dirName);
    if (chdirStatus < 0) {
        err_sys("Couldn't open path: %s", dirName);
        exit(EXIT_FAILURE);
    }
    printf("Sorting the files at: %s\n", dirName);

    // get file count in cwd
    numOfFilesInDir = getNumFilesInDir(".");

    // create an array of structs to hold paths + file names
    Path *files = calloc(numOfFilesInDir, sizeof(Path));

    // number of threads needed
    pthread_t threads[numOfFilesInDir];

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // array of pointers to Info structs
    Info **dbChunks = (Info **) malloc(sizeof(Info*) * numOfFilesInDir);

    // Initialize the output array
    sorted = calloc(dbArraySize, sizeof(Info));

    //  try to open and read from the directory we were given
    if ( (dir = opendir(".")) != NULL ) {
        while ( (ent = readdir(dir)) != NULL ) {
            if (ent->d_type == DT_REG && isValidFileForUse(ent->d_name)) {
                memcpy(files[i].filename, ent->d_name, strlen(ent->d_name));
                memcpy(files[i].path, dirName, strlen(dirName));
                
                // create a new thread to process this file
                pthread_create(&threads[i], &attr, sortDbChunck, (void*) &files[i]);
                i++;
            }
        }

        closedir(dir);
    } else {
        err_sys("Could not open the directory!\n");
        exit(EXIT_FAILURE);
    }

    // wait on our threads
    for (i = 0; i < numOfFilesInDir; i++) {
        pthread_join(threads[i], (void **)&dbChunks[i]);
    }

    // loop through the arrays our threads have given us and copy the
    // data to our sorted[] array
    for (i = 0; i < numOfFilesInDir; i++) {
        int index = dbChunks[i][j].index;
        while (index != 0) {
            if (k == dbArraySize) {
                dbArraySize *= 2;
                temp = realloc(sorted, dbArraySize * sizeof(Info));
                sorted = temp;
            }

            sorted[k] = dbChunks[i][j];
            k++;
            j++;
            index = dbChunks[i][j].index;
        }

        // free db chunk since we are done with it
        free(dbChunks[i]);

        j = 0;
    }

    // resort all the merged db chunks
    qsort(sorted, k, sizeof(Info), compare);

    // open a file for output
    if ( (fp = fopen(outputFilename, "w")) == NULL) {
        err_sys("Couldn't open output file!");
        exit(EXIT_FAILURE);
    }

    // write data to the output file
    for (i = 0; i < k; i++) {
        Info row = sorted[i];
        fprintf(fp, "%s,%s,%s,%s,%d\n", row.username, row.password, row.bloodtype, row.domain, row.index);
    }
    
    printf("\nWrote sorted data to '%s/%s'!\n", dirName, outputFilename);

    // clean up KILL ALL THE THINGS
    fclose(fp);
    free(files);
    free(dbChunks);
    free(temp);

    //make sure all threads are quit
    pthread_exit(NULL);

    return 0;
}
