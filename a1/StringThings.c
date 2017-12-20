// Demo of strtok_r() and other string things
// along with some struct matters.
#include <stdio.h>
#include <string.h>
#include "apue.h"

char * Line1= "Line,Of,7,Comma,Separated,-7,8";

typedef struct LineStruct
          {char Field1[5];
           char Field2 ;
           int Field3;
           char Field5[12];
           int Field6;
          } LineStructType;
typedef LineStructType * LineStructPtr;

int main() {
    struct LineStruct Struct1; // struct LineStruct
    LineStructType Struct2; // and LineStructType are same
    // and could be named identically !
    LineStructType * StructPtr2;
    LineStructPtr StructPtr; // and used to declare pointers.
    int FieldCount;
    char * token, * SavePtr; // for da loop with strtok
    // Now let's use that linechar *Name
    char * LineCopy = malloc(strlen(Line1) + 1);
    strncpy(LineCopy, Line1, strlen(Line1) + 1); // puts null at endLineStructPtr
    // can't strtok with string literal -- why ?
    token = strtok_r(LineCopy, ",\n", & SavePtr);
    strncpy(Struct1.Field1, token, 5); // NO NULL
    Struct1.Field2 = 'A';
    FieldCount = 1;
    while ((token = strtok_r(NULL, ",\n", & SavePtr)) != NULL) {
        ++FieldCount;
        switch (FieldCount) {
            case 3:
                Struct1.Field3 = atoi(token);
                break;
            case 5:
                strncpy(Struct1.Field5, token, 12);
                break;
            case 6:
                Struct1.Field6 = atoi(token);
                break;
            default:
                break;
        }
    }
    printf("Field5 %s\n Field3 %d\n Field6 %d\n Field1 %s\n",
        Struct1.Field5, Struct1.Field3, Struct1.Field6, Struct1.Field1);
    Struct2 = Struct1; // !? Really !
    Struct2.Field2 = (char) 0; // NULL
    printf("Field5 %s\n Field3 %d\n Field6 %d\n Field1 %s\n",
        Struct2.Field5,
        Struct2.Field3,
        Struct2.Field6,
        Struct2.Field1);
    StructPtr = malloc(sizeof(Struct1));
    ( * StructPtr) = Struct1;
    StructPtr -> Field2 = (char) 0; // NULL
    printf("Field5 %s\n Field3 %d\n Field6 %d\n Field1 %s\n",
        StructPtr -> Field5,
        StructPtr -> Field3, // N.B. -> when referencing with pointer
        StructPtr -> Field6,
        StructPtr -> Field1);
    // Self Confession -- i have wastefully not free()ed malloced
    // memory.
}
