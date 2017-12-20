
/*
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

        while ((fileDirEnt = readdir(dir)) != NULL) {
            if (fileDirEnt->d_type == DT_REG) {
                //TODO run as thread
                sprintf(data[i].filePath, "%s/%s", dirName, fileDirEnt->d_name);

                if ((returnCode = pthread_create(&threads[i], NULL, (void *) threadSortFile, &data[i]))) {
                    fprintf(stdout, "ERROR; return pthread_create() on sort:%s", data[i].filePath);
                    exit(-1);
                }
                i++;
            }
        }
        closedir(dir);
        int total = 0;
        for( i = 0; i < numOfFilesInDir; i++) {
            pthread_join(threads[i], (void **) &data[i]);
            printf("\nData[%d]", i);
            // printf("dataRows[0].id:%d", data[i].dataRows[0].id);
            printf("\nData[%d]:%d", i, data[i].dataRowsSize);
            total += data[i].dataRowsSize;
        }

        dataRowType **allDataRows = (dataRowType  **) malloc(total * sizeof(dataRowType));
        int allRowsIndex = 0;
        for( i = 0; i < numOfFilesInDir; i++ ){
            for(int j = 0; j < data[i].dataRowsSize; j++) {
                dataRowPtr row = data[i].dataRows[j];
                printf("\nData[%d] ROW[%d] row.id:%d", i, j, row->id);
                allDataRows[allRowsIndex] = row;
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

        printf("\nTOTAL!:%d", total);
        for(i = 0; i < total; i++) {
            printf("\nData[%d]:%d", i, allDataRows[i]->id);
        }
    } else {
        err_sys("Could not open the directory!\n");
        exit(EXIT_FAILURE);
    }
}