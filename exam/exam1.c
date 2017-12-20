#include "exam1.h"
#include "exam1.h"

void main() {
    printf("\n---------------------structsEx----------------\n");
    structsEx();
    printf("\n---------------------writeAndReadBinaryFile----------------\n");
    writeAndReadBinaryFile();
    printf("\n---------------------pipeEx----------------\n");
    pipeEx();
    pipeEx2();
}

void structsEx() {
    TestStructType TestStructNext;
    TestStructNext.FieldOne = 1234567890;
    TestStructNext.FieldTwo = NULL;
    TestStructPtr TestStructNextPtr = &TestStructNext;

    TestStructType TestStruct;
    TestStruct.FieldOne = 1;
    TestStruct.FieldTwo = TestStructNextPtr;
    printf("Hello %d-%d\n",TestStruct.FieldOne, TestStruct.FieldTwo->FieldOne);

    int numOfElements = 5;
    TestStructType **testStructs = (TestStructType **)malloc(sizeof(TestStructType)*numOfElements);
    int i;
    for( i = 0; i < numOfElements; i++ ) {
        TestStructType tempStruct;
        tempStruct.FieldOne = i+100;
        TestStructPtr tempPtr = &tempStruct;

        // testStructs[i] = calloc(0, sizeof(TestStructType *));
        testStructs[i] = tempPtr;
        printf("\n\tStruct[%d] one:%d", i, testStructs[i]->FieldOne);
    }

}

void writeAndReadBinaryFile() {
    int filePointer;
    char fileName[] = "my.bin";
    char str[] = "TestString";
    char buffer[1024];

    if( (filePointer = open(fileName, O_CREAT | O_RDWR, 0777)) > 0 ) {
        int fortyTwo = 42;
        write(filePointer, &fortyTwo, sizeof(fortyTwo));
        write(filePointer, str, strlen(str));
        SimpleStructType SimpleStruct;
        SimpleStruct.one = 56789;
        write(filePointer, &SimpleStruct, sizeof(SimpleStructType));

        close(filePointer);
    } else {
        printf("\nCant create/open\n");
    }

    if( (filePointer = open(fileName, O_RDWR, 0777)) > 0 ) {
        int n;
        if( (n = read(filePointer, buffer, 1024)) > 0 ) {
            int a;
            char *b;
            b = malloc(strlen(str));
            memcpy(&a, &buffer[0], sizeof(int));
            memcpy(b, &buffer[sizeof(int)], strlen(str));
            SimpleStructType SimpleStruct2;
            memcpy(&SimpleStruct2, &buffer[sizeof(int)+strlen(str)], sizeof(SimpleStructType));

            printf("\n%d %s %d\n", a, b, SimpleStruct2.one);
        } else {
            printf("\nCant read\n");
        }
    } else {
        printf("\nCant do shit\n");
    }
    printf("\nEND\n");
}

void pipeEx() {
    int     fd[2], nbytes;
    pid_t   childpid;
    char    string[] = "Hello, world!\n";
    char    readbuffer[80];

    pipe(fd);

    if((childpid = fork()) == -1) {
        perror("fork");
        exit(1);
    }

    if(childpid == 0) {
        /* Child process closes up input side of pipe */
        close(fd[0]);

        /* Send "string" through the output side of pipe */
        write(fd[1], string, (strlen(string)+1));
        exit(0);
    } else {
        /* Parent process closes up output side of pipe */
        close(fd[1]);

        /* Read in a string from the pipe */
        nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
        printf("Received string: %s", readbuffer);
    }
}

void pipeEx2() {
    pid_t childPid;
    int fd[2];
    char buffer[1024];

    pipe(fd);

    if((childPid = fork()) == -1) {
        //error
    }

    if( childPid == 0 ) {
        close(fd[0]); //close input side
        write(fd[1], "asdf", 5);
        exit(0);
    } else {
        close(fd[1]); // close output side
        int bytesRead = read(fd[0], buffer, sizeof(buffer));
        printf("\nDo stuff:%s \n", buffer);
    }
}
