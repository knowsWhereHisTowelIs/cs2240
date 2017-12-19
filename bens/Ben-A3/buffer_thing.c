/**
 * Benjamin Masters
 * 2015/09/22
 * CS2240 | Project A3 | part a
 *
 * This assignment will weave together the two modules that
 * read lines from the AllCountries.dat file, creating a CountryStruct
 * for each country found in the buffered lines.
 * (Straight from the director's mouth)
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "apue.h"

// the size of our arrays
// and how many characters we're going to read at once
#define BUFFER_SIZE 1024

// I like this
typedef int bool;
#define true 1
#define false 0

/**
 * This function takes a file descriptor and reads 1024 bytes into memory.
 * It then returns 1 line from the buffer per call 
 * (rereading from the file as necessary).
 * I will admit now that while I like how I went about solving this problem,
 * this code needs to be refactored a bit.
 * I am guilty of violating the DRY principle and I shall spend
 * the weekend in deep shame.
 * If I get the chance, I'd like to pretty this up.
 */
char *gimmeALine(int fd) {
    static char buffer[BUFFER_SIZE];        // data is read into this
    static bool read_the_damn_file = true;  // should we read the file?
    static char *nextLine = NULL;           // lines are copied into this for returning
    char *buffer_ptr;                       // some pointer magic will be involved
    char *lf_location;                      // location of the next line feed char
    char *breadcrumb;                       // used when we have a partial nextLine
    short line_end = 0;                     // integer location of the next LF
    static int chars_processed = 0;         // number of chars processed from the current buffer
    static ssize_t bytes_in;                // needed for read()

    // only read the file if the buffer has been exhausted
    if (read_the_damn_file == true) {
        bytes_in = read(fd, &buffer, BUFFER_SIZE);
        read_the_damn_file = false;
    }

    // if we got data back from the file
    if (bytes_in > 0) {
        if (nextLine == NULL) {
            // we need a less-temporal residence for this line
            nextLine = (char *) malloc( (int) BUFFER_SIZE);
            if (nextLine == NULL) {
                err_sys("buffer_thing.c -> nextLine malloc failed");
            }
            memset(nextLine, '\0', BUFFER_SIZE);
        } else {
            memset(nextLine, '\0', BUFFER_SIZE);
        }

        // a new line should start at this position in the buffer
        buffer_ptr = &buffer[chars_processed];

        // try to find the location of the next newline
        lf_location = strchr(buffer_ptr, '\n');

        if (lf_location != NULL) {
            if (chars_processed == bytes_in) {
                return "EOF";
            }
            // we're not at the end of the buffer
            // calculate how long the line is
            line_end = (lf_location - buffer_ptr) + 1;
            memcpy(nextLine, buffer_ptr, line_end);
            chars_processed += (line_end);

        } else {
            // we're at the end of the buffer
            // calculate how long we have until the end of the buffer
            line_end = BUFFER_SIZE - chars_processed;
            memcpy(nextLine, buffer_ptr, line_end);
            chars_processed += (line_end);

            // create a pointer to where we left off in nextLine
            breadcrumb = &nextLine[line_end];

            if (chars_processed == bytes_in) {
                chars_processed = 0;
                // read the file again--
                bytes_in = read(fd, &buffer, BUFFER_SIZE);
            }

            // a new line should start at this position in the buffer--
            buffer_ptr = &buffer[chars_processed];

            // try to find the location of the next newline--
            lf_location = strchr(buffer_ptr, '\n');

            // we're not at the end of the buffer
            // calculate how long the line is
            line_end = (lf_location - buffer_ptr) + 1;
            memcpy(breadcrumb, buffer_ptr, line_end);
            chars_processed += (line_end);

        }

    } else {
        // we've hit the EOF
        nextLine = "EOF";
    }

    return nextLine;

}
