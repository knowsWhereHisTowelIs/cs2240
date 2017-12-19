/**
 * Who: Caleb Slater
 * What: A3 B
 * When: 2015-10-01
 * Where: CS 2240
 * How: "$./b.out"
 * Reads file. writes binary. reads binary on user request and outputs data
**/
#include "functions.h"

/*************GLOBALS*************/
dataRowType **allDataRows; // array of all datarows that will be sorted

#define ARRAYSIZE 17
#define NUMTHREADS 4
//TODO REMOVE EXAMPLES
struct ThreadData {
    int start, stop;
    int* array;
    int offset, total, index;
};

/* puts i^2 into array positions i=start to stop-1 */
void squarer(struct ThreadData* td) {
    struct ThreadData* data=(struct ThreadData*) td;
    int start=data->start;
    int stop=data->stop;
    int offset = data->offset;
    int* array=data->array;
    int i;
    int total = 0;
    for (i = start; i < stop; i++) {
        array[i] = i + offset;
        total += array[i];
    }

    data->total = total;

    // printf("\n\n Data[%d] offset:%d total:%d \n", data->index, offset, total);
    // return (void *) &total;
    return;
}

void threadEx() {
    int array[ARRAYSIZE];
    pthread_t thread[NUMTHREADS];
    struct ThreadData data[NUMTHREADS];
    int i;
    /*
        this has the effect of rounding up the number of tasks
        per thread, which is useful in case ARRAYSIZE does not
        divide evenly by NUMTHREADS.
    */
    int tasksPerThread=(ARRAYSIZE+NUMTHREADS-1)/NUMTHREADS;

    /* Divide work for threads, prepare parameters */
    for (i=0; i<NUMTHREADS; i++) {
        data[i].index = i;
        data[i].start=i*tasksPerThread;
        data[i].stop=(i+1)*tasksPerThread;
        data[i].array=array;
        data[i].offset=i*10;
    }
    /* the last thread must not go past the end of the array */
    data[NUMTHREADS-1].stop=ARRAYSIZE;

    /* Launch Threads */
    for (i = 0; i<NUMTHREADS; i++) {
        pthread_create(&thread[i], NULL, (void *) squarer, &data[i]);
    }
    /* Wait for Threads to Finish */
    int total = 0;
    for (i = 0; i < NUMTHREADS; i++) {
        pthread_join(thread[i], NULL);
        total += data[i].total;
        printf("\ndata[%d] total:%d", i, data[i].total);
    }
    printf("\n\nNUMTHREADS:%d TOTAL:%d", NUMTHREADS, total);


    /* Display Result */
    for (i=0; i < ARRAYSIZE; i++) {
        printf("\nArr[%d]:%d ", i, array[i]);
    }
    printf("\n");
}



/*
 *
 *
 *
 *  Reading a directory code shamelessly stolen from StackOverflow: http://bit.ly/1HKQyUV
 */
void sortDataFilesFromDir(){
    DIR *dir;
    struct dirent *fileDirEnt;
    char *dirName = "./data";
    int i;
    int returnCode;

    if ((dir = opendir(dirName)) != NULL) {
        int numOfFilesInDir = countFilesInDirectory(dir); //also num of threads
        pthread_t *threads = malloc( sizeof(pthread_t) * numOfFilesInDir );
        threadDataType data[numOfFilesInDir];
        for( i = 0; i < numOfFilesInDir; i++ ){
            threadDataType emptyData = *(threadDataType *) malloc( sizeof(threadDataType) );
            emptyData.test = 'h';
            data[i] = emptyData;
        }

        while ((fileDirEnt = readdir(dir)) != NULL) {
            if (fileDirEnt->d_type == DT_REG) {
                //TODO run as thread
                sprintf(data[i].filePath, "%s/%s", dirName, fileDirEnt->d_name);

                if ((returnCode = pthread_create(&threads[i], NULL, (void *) threadSortFile, &data[i]))) {
                    fprintf(stderr, "ERROR; return pthread_create() on sort:%s", data[i].filePath);
                    exit(-1);
                }
                i++;
            }
        }
        closedir(dir);

        int total = 0;
        for( i = 0; i < numOfFilesInDir; i++) {
            pthread_join(threads[i], NULL);
            //TODO FIX SHIT
            fprintf(stderr, "\nData[%d] test:%c \n", i, data[i].test );
            dataRowPtr rowPtr = data[i].dataRows[0];
            dataRowType row = *rowPtr;
            fprintf(stderr, "dataRows[0].id:%d", row.id);
            fprintf(stderr, "\nData[%d]:%d", i, data[i].dataRowsSize);
            total += data[i].dataRowsSize;
        }

        dataRowType **allDataRows = (dataRowType  **) malloc(total * sizeof(dataRowType));
        int allRowsIndex = 0;
        for( i = 0; i < numOfFilesInDir; i++ ){
            for(int j = 0; j < data[i].dataRowsSize; j++) {
                dataRowType row = *(dataRowType *) data[i].dataRows[j];
                printf("\nData[%d] ROW[%d] row.id:%d", i, j, row.id);
                allDataRows[allRowsIndex] = &row;
                allRowsIndex++;
            }
        }
        // int dataIndex = 0;
        // for( int allIndex = 0; allIndex < total; allIndex++ ){
        //     if ( dataIndex == data[allIndex].dataRowsSize ) {
        //         printf("\nDataIndex:%d == data[all:%d]:%d", dataIndex, allIndex, data[allIndex].dataRowsSize );
        //
        //         dataIndex = 0;
        //     }
        //     allDataRows[allIndex] = (dataRowType *) data[allIndex].dataRows[dataIndex];
        // }

        sortDataRows(allDataRows, total);

        printf("\n-----------------------------TOTAL!:%d", total);
        for(i = 0; i < total; i++) {
            printf("\nData[%d]:%d", i, allDataRows[i]->id);
        }
    } else {
        err_sys("Could not open the directory!\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, const char * argv[]) {
    allDataRows = (dataRowType  **) malloc(DATA_ROWS_INITIAL_SIZE * sizeof(dataRowType));

    sortDataFilesFromDir();
    // threadEx();
    printf("\n");
    return 0;
}
