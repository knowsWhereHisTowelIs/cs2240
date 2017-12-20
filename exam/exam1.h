#ifndef TEST
    #define TEST
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdlib.h>
    #include <fcntl.h>

    typedef struct TestStruct {
        int FieldOne;
        struct TestStruct *FieldTwo;
    } TestStructType;
    typedef TestStructType *TestStructPtr;

    typedef struct SimpleStruct{
        int one;
    } SimpleStructType;
    typedef SimpleStructPtr;

    void structsEx();
    void writeAndReadBinaryFile();
    void pipeEx();
    void pipeEx2();
#endif
