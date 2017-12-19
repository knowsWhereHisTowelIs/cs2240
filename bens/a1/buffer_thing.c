//
//  buffer_thing.c
//  a1
//
//  Created by Benjamin Masters on 9/15/15.
//  Copyright (c) 2015 Benjamin Masters. All rights reserved.
//

#include <stdio.h>
#include "buffer_thing.h"
#include "apue.h"

// the size of our arrays
// and how many characters we're going to read at once
#define BUFFER_SIZE 1024

// I like this
typedef int bool;
#define true 1
#define false 0

char *gimmeALine(int fd) {
    static char buffer[BUFFER_SIZE];        // data is read into this
    static bool read_the_damn_file = true;  // should we read the file?
    char *line;                             // lines are copied into this for returning
    static char *buffer_ptr = 0;            // some pointer magic will be involved
    char *lf_location;                      // location of the next line feed char
    int line_end = 0;                       // integer location of the next LF
    static int chars_processed = 0;         // number of chars processed from the current buffer
    static ssize_t bytes_in;                // needed for read()

    if (read_the_damn_file == true) {
        bytes_in = read(fd, &buffer, BUFFER_SIZE);
        read_the_damn_file = false;
    }

    if (bytes_in > 0) {
        // we need a less-temporal residence for this line
        line = malloc(sizeof(BUFFER_SIZE));
        memset(line, '\0', BUFFER_SIZE);

        buffer_ptr = &buffer[chars_processed];

        // try to find the location of the next new line
        lf_location = strchr(buffer_ptr, '\n');

        if (lf_location != NULL) {
            // we're not at the end of the buffer
            // calculate how long the line is
            line_end = (int) (lf_location - buffer_ptr) + 1;

        } else {
            // we're at the end of the buffer
            // calculate how long we have until the end of the buffer
            line_end = (int) BUFFER_SIZE - chars_processed;
        }

        strncpy(line, buffer_ptr, line_end);
        chars_processed += (line_end);

        if (chars_processed == bytes_in) {
            chars_processed = 0;
            read_the_damn_file = true;
        }

    } else {
        // we've hit the EOF
        line = "EOF";
    }

    return line;

}
