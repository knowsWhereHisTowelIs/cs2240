// demo of raw write
// and random access of file
// along with struct , union referencing.
// we write to a file and read it back
// after some lseek() games
// Robert Trenary, 9/22/15
#include "apue.h" // for Stevens
#include <stdio.h> // the usual
#include <stdlib.h> // for STDOUT_FILENO define
#include <fcntl.h> // for open

typedef union {
    char Bytes[4];
    int X;
} BoxOBits;
// we can see this two different ways
typedef struct {
    BoxOBits BOB;
    char Code[4];
    float F;
} teststructtype;
// and the union can be used in a struct
// watch how we reference
int main() {
    teststructtype S1, S2; // a couple of structs reading and writing
    int FD = open("TestBinaryFile", O_CREAT | O_RDWR, 0777);
    // a file for same purpose --
    // note alleged protection on create
    char * Buff = malloc(strlen("a literal\n"));
    strncpy(Buff, "a literal\n", 10); // we can change Buff if we want
    write(STDOUT_FILENO, & Buff[2], 8); // just a simple write
    // Fill the structure with values
    S1.F = -3.14;
    S1.BOB.X = 0x46414345; // this is 'F''A''C''E' in 4 bytes
    teststructtype * Xptr = (teststructtype * )( & S1.BOB); // teststructtype
    write(STDOUT_FILENO, & S1.BOB.Bytes[2], 1); // One letter in BOB
    write(STDOUT_FILENO, "<skip1>\n", 8); // new line demarcation

    write(1, & (Xptr -> BOB.Bytes[0]), 4); // Pointer demo
    write(1, "<skip2>\n", 8); // new line demarcation
    lseek(FD, 100, SEEK_SET); // arbitrary place in file
    write(FD, & S1, sizeof(teststructtype)); // S1 won't work
    // we wrote the whole struct byte by
    write(1, "<skip3>\n", 8); // new line demarcation
    write(FD, & Buff[2], 8); // put some of buffer "literal" after S1
    write(1, "<skip4>\n", 8); // new line demarcation
    lseek(FD, -(8 + sizeof(S1)), SEEK_CUR);
    // should be where we began with
    //lseek(FD,100,SEEK_SET);

    read(FD, & S2, sizeof(S2)); // Fill a new struct
    write(STDOUT_FILENO, S2.BOB.Bytes, 4); // and prove it
    write(1, "<skip5>\n", 8); // new line demarcation
    read(FD, Buff, 3); // now read part of buffer we wrote
    write(STDOUT_FILENO, Buff, 10); // on top of former buffer
    write(1, "<skip6>\n", 8); // new line demarcation

}
