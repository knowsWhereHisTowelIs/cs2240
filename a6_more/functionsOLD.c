
#include "functions.h"

/*************GLOBALS*************/
int currentLineNum = 1;
int currentLineInBuffer = 0;
int currentLineInBufferNum = 0;
int bufferIndex = -1;
char buffer[BUFFER_SIZE], leftOverLine[BUFFER_SIZE];
/*************END GLOBALS*************/

/*****************END MISC*******/

/*****************END MISC*******/

/**
 * Sorts data in file in threaded way
 * @return int # of rows in file
 **/
void* threadSortFile(void *arg) {
    int fd;
    fprintf(stderr, "\nHERE\n");
//    threadDataPtr data = malloc(sizeof(threadDataType));
//    char *filePath = (char *) arg;
//    threadDataType data2 = *(threadDataType *) &arg;
    threadDataType *data = (threadDataType *) arg;
    
//    fprintf(stderr, "\nFILEPATH:%s", filePath);
//    memcpy(data->filePath, filePath, strlen(filePath));
    data->dataRows = (dataRowType  **) malloc(DATA_ROWS_INITIAL_SIZE * sizeof(dataRowType));
    boolean hasMoreData = true;
    int numOfRows = 0;

//    fprintf(stderr, "\nthreadSortFile(): file:'%s' ", data->filePath);
    if ((fd = open(data->filePath, O_CREAT | O_RDWR, 0777)) < 0) {
        //TODO error msg
        fprintf(stderr, "\n==================>Error opening: '%s' ", data->filePath);
        exit(-1);
    }
    //TODO END DEBUG
    while (hasMoreData && 0) {
//        char *line = gimmeALine(fd);
        char *line = "Youting,Eziecai7u,O+,PrivacyUser.com,2176";
        if( numOfRows == 0 ){
            fprintf(stderr, "\n File:%s Line: %s ", data->filePath, line);
        }
        if (line[0] == '\0') {
            hasMoreData = false;
        } else {
//            dataRowPtr row = malloc(DATA_ROW_SIZE);
            dataRowType row;
            fprintf(stderr, "\nFile:%s LINE:%s", data->filePath, line);
            fillDataRowFromLine(line, &row);
            if (row.id > 0) {
                data->dataRows[numOfRows] = &row;
                numOfRows++;
            } else {
                fprintf(stderr, "\n=============>Bad data! File:%s id:%d line:%s", data->filePath, row.id, line);
//                free(row);
            }
        }
    }
    data->test = 'H';
    data->dataRowsSize = numOfRows;
    sortDataRows(data->dataRows, numOfRows);
    
    pthread_exit(data);
}

void sortDataRows(dataRowType **rows, int numInArr) {
    int j, i;
    for (i = 1; i < numInArr; i++) {
        for (j = 0; j < numInArr - i; j++) {
            // fprintf(stderr, "\nCMP i:%d j:%d rows[j]->id:%d rows[j+1]->id:%d", i, j, rows[j]->id, rows[j+1]->id);
            if (rows[j]->id > rows[j + 1]->id) {
                dataRowPtr temp = rows[j];
                rows[j] = rows[j + 1];
                rows[j + 1] = temp;
            }
        }
    }
}

int countFilesInDirectory(char *dirPath) {
    int fileCount = 0;
    struct dirent *entry;
    DIR *dir;
    if ((dir = opendir(dirPath)) == NULL) {
        err_sys("Could not open the directory!\n");
        exit(EXIT_FAILURE);
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            fileCount++;
        }
    }
    return fileCount;
}

/**
 * From a3
 * Returns a line at a time from a file
 * Note lines must be less than BUFFER_SIZE in order to return a full line
 **/
char* gimmeALine(int filePointer) {
    
    char *line = malloc(BUFFER_SIZE), continueReadingFile = 1, tmpLine[BUFFER_SIZE];
    int lineNum = 1, n;
    memset(&tmpLine[0], '\0', strlen (tmpLine)); //clean buffer incase buffer isn't filled completly -> not bad memory

    while (continueReadingFile == 1) {
        if (bufferIndex == -1 || (bufferIndex) >= (int) BUFFER_SIZE) { //if is first run or need more buffer
            memset(&buffer[0], '\0', sizeof (buffer)); //clean buffer incase buffer isn't filled completly -> not bad memory
            if ((n = read(filePointer, buffer, BUFFER_SIZE)) > 0) {
                //read next buffer length
                currentLineInBufferNum = 0;
            } else {
                continueReadingFile = 0;
            }
        }
        int lastLineIndex = 0;

        currentLineInBuffer = 0;
        for (bufferIndex = 0; bufferIndex < ((int) BUFFER_SIZE); bufferIndex++) {
            if (buffer[bufferIndex] == '\n') { //split on new line
                //clear next line so buffer fills up for next iteration
                if (currentLineInBuffer == currentLineInBufferNum) {
                    int lineStartIndex = lastLineIndex;
                    if (buffer[lineStartIndex] == '\n') {
                        lineStartIndex++; //if not first line curreint index is \n
                    }
                    int lineLength = max(0, bufferIndex - lineStartIndex);

                    memcpy(tmpLine, &buffer[lineStartIndex], lineLength);
                    strcpy(line, leftOverLine);
                    strcat(line, tmpLine);

                    //increment and break
                    currentLineNum++;
                    currentLineInBufferNum++;
                    continueReadingFile = 0;
                    break;
                } else {
                    lastLineIndex = bufferIndex;
                }
                lineNum++;
                currentLineInBuffer++;
                memset(&leftOverLine[0], '\0', sizeof (leftOverLine)); //clean buffer incase buffer isn't filled completly -> not bad memory
            }
        }
        //strcpy(leftOverLine, tmp);
        int leftOverLineStartIndex = lastLineIndex;
        if (lastLineIndex + 1 < (int) BUFFER_SIZE) { //ignore newline char if possilbe
            leftOverLineStartIndex++;
        }
        fprintf(stderr, "\t !!! BuffIndex:%d  leftOverLineStartIndex:%d \n", bufferIndex, leftOverLineStartIndex);
        write(STDOUT_FILENO, "\n", 1);
        memcpy(leftOverLine, &buffer[leftOverLineStartIndex], bufferIndex - leftOverLineStartIndex);
    }
    return line;
}

void fillDataRowFromLine(char *line, dataRowPtr row) {
    //set defaults incase error
    row->id = -1;
    memset(&row->username[0], '\0', strlen(row->username));
    memset(&row->password[0], '\0', strlen(row->password));
    memset(&row->bloodType[0], '\0', strlen(row->bloodType));
    memset(&row->domain[0], '\0', strlen(row->domain));

    char *token;
    char *lineCopy = malloc(strlen(line) + 1);
    strncpy(lineCopy, line, strlen(line) + 1);
    int fieldNum = 0;
    while ((token = strtok_r(lineCopy, ",", &lineCopy)) != NULL) {
        switch (fieldNum) {
            case 0:
                strncpy(row->username, token, strlen(token));
                break;
            case 1:
                strncpy(row->password, token, strlen(token));
                break;
            case 2:
                strncpy(row->bloodType, token, strlen(token));
                break;
            case 3:
                strncpy(row->domain, token, strlen(token));
                break;
            case 4:
                row->id = atoi(token);
                break;
        }
        fieldNum++;
    }
}
