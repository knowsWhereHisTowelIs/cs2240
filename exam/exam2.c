#include "exam2.h"

void floatingPointShit();
void qsortExample();
int intsCmpFunc(const void * a, const void * b);
int structsCmpFunc(const void * a, const void * b);

typedef struct {
    int id;
} sampleType;

int main(void) {
    printf("\nBEGIN EXAM 2\n");
    floatingPointShit();
    
    qsortExample();
    
    printf("\n");
    return 0;
}

void floatingPointShit() {
    int floating = 0x42507df4;
    printf("\nFloating point %f === 0x%x ", (double) floating, (unsigned int) floating);
    int floating2 = 0b1;
    // 32 bit
    // 1 bit - 5 bits - 23 bits
    // (-1)^sign * 2^exponent * 2^exponent (left to write ex 1000 = 2^1/2)
    printf("\nFloating point %f === 0x%x ", (double) floating2, (unsigned int) floating2);
    //should be 52.123 = 0x42507df4
}

void qsortExample() {
    printf("\n---------SORT INT ARRY--------\n");
    int ints[10] = { 9, 6, 7, 8, 5, 4, 3, 2, 1, 0 };
    
    qsort(ints, 10, sizeof(int), intsCmpFunc);
    
    int i = 0;
    for( i = 0; i < 10; i++) {
        printf("\nints[%d]:%d", i, ints[i]);
    }
    
    printf("\n---------SORT STRUCT ARRY--------\n");
    
    sampleType structs[10];
    for(i = 0; i < 10; i++){
        sampleType *tmp = malloc( sizeof(sampleType) );
        tmp->id = rand() & 100;
        //printf("\nStruct[%d]:%d", i, tmp->id);
        structs[i] = *tmp;
    }
    
    qsort(structs, 10, sizeof(sampleType), structsCmpFunc);
    
    for( i = 0; i < 10; i++){
        printf("\nstructs[%d]:%d", i, structs[i].id);
    }
}

int intsCmpFunc(const void * a, const void * b){
    int A = *(int *)a;
    int B = *(int *)b;
    if( A > B ) {
        return 1;
    } else if ( A < B ) {
        return -1;
    } else {
        return 0;
    }
}

int structsCmpFunc(const void * a, const void * b){
    sampleType A = *(sampleType *)a;
    sampleType B = *(sampleType *)b;
    if( A.id > B.id ) {
        return 1;
    } else if ( A.id < B.id ) {
        return -1;
    } else {
        return 0;
    }
}

