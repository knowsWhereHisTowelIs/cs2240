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
    struct ThreadData* data = (struct ThreadData*) td;
    int start = data->start;
    int stop = data->stop;
    int offset = data->offset;
    int* array = data->array;
    int i;
    int total = 0;
    for (i = start; i < stop; i++) {
        array[i] = i + offset;
        total += array[i];
    }

    data->total = total;

    fprintf(stderr, "\n\n Data[%d] offset:%d total:%d \n", data->index, offset, total);
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
    int tasksPerThread = (ARRAYSIZE + NUMTHREADS - 1) / NUMTHREADS;

    /* Divide work for threads, prepare parameters */
    for (i = 0; i < NUMTHREADS; i++) {
        data[i].index = i;
        data[i].start = i*tasksPerThread;
        data[i].stop = (i + 1) * tasksPerThread;
        data[i].array = array;
        data[i].offset = i * 10;
    }
    /* the last thread must not go past the end of the array */
    data[NUMTHREADS - 1].stop = ARRAYSIZE;

    /* Launch Threads */
    for (i = 0; i < NUMTHREADS; i++) {
        pthread_create(&thread[i], NULL, (void *) squarer, &data[i]);
    }
    /* Wait for Threads to Finish */
    int total = 0;
    for (i = 0; i < NUMTHREADS; i++) {
        pthread_join(thread[i], NULL);
        fprintf(stderr, "\ndata[%d] total:%d", i, data[i].total);
    }
    fprintf(stderr, "\n\nNUMTHREADS:%d TOTAL:%d", NUMTHREADS, total);


    /* Display Result */
    for (i = 0; i < ARRAYSIZE; i++) {
        fprintf(stderr, "\nArr[%d]:%d ", i, array[i]);
    }
    fprintf(stderr, "\n");
}

/*
 *  Reading a directory code shamelessly stolen from StackOverflow: http://bit.ly/1HKQyUV
 */
void sortDataFilesFromDir() {
    DIR *dir;
    struct dirent *fileDirEnt;
    char *dirName = "./data2";
    int i = 0, returnCode;

    if ((dir = opendir(dirName)) == NULL) {
        err_sys("Could not open the directory!\n");
        exit(EXIT_FAILURE);
    }

    int numOfFilesInDir = countFilesInDirectory(dirName); //also num of threads
    pthread_t *threads = malloc(sizeof (pthread_t) * numOfFilesInDir);
    threadDataType *data = calloc(numOfFilesInDir, sizeof(threadDataType));
    
    fprintf(stderr, "\nNum of files in dir:%s has %d\n", dirName, numOfFilesInDir);

    while ((fileDirEnt = readdir(dir)) != NULL) {
        fprintf(stderr, "\n-----File:%s d_type:%hho == DT_REG:%hho", fileDirEnt->d_name, fileDirEnt->d_type, DT_REG);
        if (fileDirEnt->d_type == DT_REG) {
            //TODO run as thread
//            char *filePath = malloc(sizeof(char) * 512);
//            data[i] = malloc( sizeof(threadDataType) );
            
//            threadDataType dataRow;
//            dataRow.dataRowsSize = 0;
//            dataRow.id = -1;
//            dataRow.test = 'h';
//            sprintf(dataRow.filePath, "%s/%s", dirName, fileDirEnt->d_name);
            fprintf(stderr, "\n i:%d", i);
            if( i >= numOfFilesInDir ) {
                fprintf(stderr, "\n=============>OH CRAP index outta bound");
            }
            data[i].test = '1';
            sprintf(data[i].filePath, "%s/%s", dirName, fileDirEnt->d_name);
            
//            fprintf(stderr, "\nFILEPATH:%s", data[i].filePath);
            if ( (returnCode = pthread_create(&threads[i], NULL, (void *) threadSortFile, (void *) &data[i])) ) {
//                fprintf(stdout, "ERROR; return pthread_create() on sort:%s", filePath);
                exit(-1);
            }
            i++;
        } else {
            fprintf(stderr, "\n===========>Invalid file:%s", fileDirEnt->d_name);
        }
    }
    closedir(dir);
    
    int total = 0;
    for (i = 0; i < numOfFilesInDir; i++) {
        pthread_join(threads[i], (void **) &data[i]);
        fprintf(stderr, "\nData[%d]:%d", i, &data[i]);
        dataRowPtr rowPtr;
        rowPtr = data[i].dataRows[0];
        dataRowType row = *(dataRowType *) rowPtr;
        fprintf(stderr, "dataRows[0].id:%d", row.id);
//        fprintf(stderr, "\nData[%d]:%d", i, data[i].dataRowsSize);
//        total += data[i].dataRowsSize;
    }
    /**
    dataRowType **allDataRows = (dataRowType **) malloc(total * sizeof (dataRowType));
    int allRowsIndex = 0;
    for (i = 0; i < numOfFilesInDir; i++) {
        for (int j = 0; j < data[i].dataRowsSize; j++) {
            dataRowPtr row = data[i].dataRows[j];
            fprintf(stderr, "\nData[%d] ROW[%d] row.id:%d", i, j, row->id);
            allDataRows[allRowsIndex] = row;
            allRowsIndex++;
        }
    }
     int dataIndex = 0;
     for( int allIndex = 0; allIndex < total; allIndex++ ){
         if ( dataIndex == data[allIndex].dataRowsSize ) {
             fprintf(stderr, "\nDataIndex:%d == data[all:%d]:%d", dataIndex, allIndex, data[allIndex].dataRowsSize );
    
             dataIndex = 0;
         }
         allDataRows[allIndex] = (dataRowType *) data[allIndex].dataRows[dataIndex];
     }

    sortDataRows(allDataRows, total);

    fprintf(stderr, "\nTOTAL!:%d", total);
    for (i = 0; i < total; i++) {
        fprintf(stderr, "\nData[%d]:%d", i, allDataRows[i]->id);
    }
    **/
}

int main(int argc, const char * argv[]) {
    allDataRows = (dataRowType **) malloc(DATA_ROWS_INITIAL_SIZE * sizeof (dataRowType));

    sortDataFilesFromDir();
    // threadEx();
    fprintf(stderr, "\n------------END-----------\n");
    return 0;
}
