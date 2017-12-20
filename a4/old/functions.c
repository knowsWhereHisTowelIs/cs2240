#include "functions.h"

//global variables
//end global variables

/**
 * Note lines must be less than BUFFER_SIZE in order to return a full line
**/
void* nu_malloc(size_t size) {
	return &size;
}

/**
 * Note lines must be less than BUFFER_SIZE in order to return a full line
**/
void* nu_calloc(size_t count, size_t size) {
	return &count;
}

/**
 * Note lines must be less than BUFFER_SIZE in order to return a full line
**/
void* nu_realloc(void* ptr, size_t size) {
	return ptr;
}

/**
 * Note lines must be less than BUFFER_SIZE in order to return a full line
**/
void nu_free(void* ptr) {

}

/**
 * Prints to standard output
**/
void printToStandardOutput(char *text, int textSize, char errorMsg[]) {
	if ( write(STDOUT_FILENO, text, textSize) != textSize ) {
		err_sys(errorMsg);
	}
	return;
} //end printToStandardOutput()

/**
 * Prints to file
**/
void myWrite(int handle, void  *buffer, int nbyte) {
	if ( write(handle, buffer, nbyte) != nbyte ) {
		err_sys("Error writing to file");
	}
	return;
} //end printToStandardOutput()

/**
 * Open file and return its file pointer
**/
int myOpenFile(char *fileName, int permissions) {
	int filePointer;
	if( (filePointer = open(fileName, permissions)) < 0 ) {
		err_sys("Error opening file");
	}
	return filePointer;
} //end myOpenFile()

/**
 * Open file and return its file pointer, creates file if doesn't exist
**/
int myOpenCreateableFile(char *fileName, int permissions) {
	int filePointer;
	if( (filePointer = open(fileName, permissions, 0777)) < 0 ) {
		err_sys("Error opening file");
	}
	return filePointer;
} //end myOpenFile()

/**
 * Close file opened
**/
int myCloseFile(int filePointer) {
	int success;

	if(close(filePointer) < 0) {
		success = 0;
		err_sys("Error closing file");
	} else {
		success = 1;
	}

	return success;
} //end myCloseFile()

/*
 * Prepends t into s. Assumes s has enough space allocated
 * for the combined string.
*/
void prepend(char* s, const char* t){
    size_t len = strlen(t);
    size_t i;

    memmove(s + len, s, strlen(s) + 1);

    for (i = 0; i < len; ++i)
    {
        s[i] = t[i];
    }
} //end prepend()
