/**
* Who: Caleb Slater
* What: A5
* When: 2015-10-31
* Where: CS 2240
* How: "$./a.out"
* Custom shell loop
* Ex "ls | ./Upcase", "./Upcase < error.c", "ls | ./Upcase > test_output_file"
* backgrounding is clunky but works ex "ls &"
* DUE 11/10
**/

/**
 * NOTES
 * IPC (inter process communication) pipes, signals, shared memory, msg ques
**/

#include "shell.h"
#include "makeargv.h"

/********GLOBALS***************/
char *parsedArgs[PARSED_ARGS_SIZE];
char cwd[BUFFER_SIZE];
/******************************/

/**
 * Shell logic
**/
int main() {
    boolean continueLoop = true;
    char *buffer;
    char **pieces;
    char **argp;
    int chdir_status;
    mode_t umask_bits;
    long octal_num;
    int numberOfArguments;
    pid_t pid;

    refreshCwd(); //set cwd
	do {
		printf(ANSI_COLOR_RED "\n%s$ ", cwd);	// prompt in custom color
        //read input
        buffer = get_input();
        pieces = split_line(buffer);
        numberOfArguments = makeargv(buffer, " |>", &argp);

		//reset data
		for (int i = 0; i < PARSED_ARGS_SIZE; i++) {
			parsedArgs[i] = 0;
		}

        if (strcasecmp(pieces[0], "exit") == 0) { //exit to bash
            continueLoop = false;
        } else if (strcasecmp(argp[0], "cd") == 0) { //change directory
            chdir_status = chdir(argp[1]);
            if (chdir_status < 0) {
                int err = errno;
                if (err == ENOENT) {
                    printf("-bsh: cd: %s: No such file or directory\n", pieces[1]);
                }
            } else {
                refreshCwd();
            }
        } else if (strcasecmp(pieces[0], "pwd") == 0) { // print current working directory
            printf("\n%s\n", cwd);
        } else if (strcasecmp(pieces[0], "umask") == 0) { //umask
            if (numberOfArguments == 1) {
                umask_bits = umask(0);
                printf("%04o\n", umask_bits);
                umask(umask_bits);
            } else if (numberOfArguments == 2) {
                octal_num = strtol(argp[1], NULL, 8);
                umask_bits = umask(octal_num);
            }
        } else { // run execute
            boolean isRunInBackground = false;
            int lastIndex = strlen(buffer)-1;
            if( buffer[lastIndex] == '&' ) {
                isRunInBackground = true;
                buffer[lastIndex] = '\0'; //set to ignore
            }

            if( isRunInBackground ) {
                if( (pid = fork()) == -1 ) {
                    //error
                } else if( pid == 0 ) { //child FIRST
                    execute(buffer);
                    exit(0);
                } else { // parent
                    //continue w/o waiting
                }
            } else {
                execute(buffer);
            }
        }
	} while( continueLoop );

	printf("\n" ANSI_COLOR_RESET);

	return 0;
}

/**
 * refresh current working directory
**/
void refreshCwd()  {
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        err_sys("Failed to get cwd");
    }
}

/*
 * Reads console input and returns the line
 */
char *get_input() {
    int buffer_size = 64;
    char ch;
    char *buffer = malloc(sizeof(char) * buffer_size);
    int i = 0;

    while ((ch = getchar()) != '\n') {
        if (ch > 31 && ch < 127) {
            if (i < buffer_size) {
                buffer[i] = ch;
                i++;
                buffer[i] = '\0';
            } else {
                buffer_size *= 2;
                buffer = realloc(buffer, buffer_size);
                buffer[i] = ch;
                i++;
                buffer[i] = '\0';
            }
        }
    }

    if (i == 0) {
        memcpy(buffer, "empty\0", 6);
    }

    return buffer;
}

/*
 * Splits line into args and cmds
 */
 char **split_line(char *line) {
     char *token;
     char *tempLine;
     char *saveptr;
     char **tokens = malloc(strlen(line));
     int i = 0;

     tempLine = strdup(line);

     token = strtok_r(tempLine, " |>&", &saveptr);
     while (token != NULL) {
         tokens[i] = token;
         i++;
         token = strtok_r(NULL, " |>&", &saveptr);
     }

    return tokens;
 }

void execute(char *rawInput) {
    char *previousOutput = "\0"; //initialize to null string
    char **argp;

    int numOfProcesses = makeargv(rawInput, "|", &argp);
    for( int i = 0; i < numOfProcesses; i++) {
        char *cmdStr = argp[i];
        boolean isRedirectingOutput = false;

        if( i == 0 ) {
            //read from file logic
            char *subString = strchr(cmdStr, '<');
            if (subString) { // < is in str
                char *fileName = &subString[1]; //ignore '<'
                trim(fileName);

                //open and read file
                int fd;
                if( (fd = open(fileName, O_RDWR, 0777)) < 0 ) {
            		err_sys("Error opening input file");
                    return;
            	}
                char buffer[BUFFER_SIZE];
                read(fd, buffer, sizeof(buffer));

                //write to "stdin"
                int masterFD;
                if( (masterFD = open(SHELL_BUFFER_FILE_NAME, O_CREAT | O_RDWR | O_TRUNC, 0777)) < 0 ) {
                    err_sys("Error opening file");
                    return;
                }
                write(masterFD, buffer, strlen(buffer));

                //not necessary... but not harmful
                //save file contents into "stdin" for handleSingleProcess()
                previousOutput = buffer;

                // parse cmd string and get everything before '<'
                char *tmpCmd = malloc(strlen(cmdStr));
                for( int j = 0; j < strlen(cmdStr); j++ ){
                    if( cmdStr[j] == '<' ) {
                        break;
                    } else {
                        tmpCmd[j] = cmdStr[j];
                    }
                }
                cmdStr = tmpCmd;
            }
        }

        // is last row in for loop
        if( (i+1) == numOfProcesses ) {
            char *subString = strchr(cmdStr, '>');
            if (subString != 0) {
                // '>' is in string
                isRedirectingOutput = true;

                // get everything before '>'
                char *tmpCmd = malloc(strlen(cmdStr));
                for( int j = 0; j < strlen(cmdStr); j++ ){
                    if( cmdStr[j] == '>' ) {
                        break;
                    } else {
                        tmpCmd[j] = cmdStr[j];
                    }
                }
                cmdStr = tmpCmd;
            }
        }

        // printf("\n-------------\nCMDSTR:!!!%s!!!\n---------------\n", cmdStr);
        // printf("\npreviousOutput[%d]:" ANSI_COLOR_BLUE "%s\n" ANSI_COLOR_RED, i, previousOutput);

        //handle single segment of full cmd
        char *processOutput;
        handleSingleProcess(cmdStr, previousOutput, &processOutput);
        previousOutput = processOutput;

        // is last row
        if( (i+1) == numOfProcesses ) {
            if (isRedirectingOutput) {
                // < is in str
                //write to file specified
                char *subString = strchr(argp[i], '>'); //make sure get unedited str
                char *fileName = &subString[1]; //ignore '<'
                trim(fileName);

                int fd;
                if( (fd = open(fileName, O_CREAT | O_RDWR, 0777)) < 0 ) {
                    err_sys("Error opening redirect file");
                    return;
                }
                write(fd, processOutput, strlen(processOutput));
            } else {
                //write out to stdout
                write(STDOUT_FILENO, processOutput, strlen(processOutput));
            }
        }
    }
    // printf("\n-------------FINAL-----------\n" ANSI_COLOR_BLUE "%s" ANSI_COLOR_RED "\n------------------", previousOutput);
}

/**
 * Handles a single segment of a command
 * Ex: ls | grep "file" will take 'ls' as input and return ls output
 * @param char *input cmd string
 * @param char *previousOutput last result (redirections from stdin)
 * @param char **output Where to save output
**/
void handleSingleProcess(char *input, char *previousOutput, char **output) {
    pid_t pid, wpid;
    int status = 0;	// for wait()
    int fd[2], masterFD;
    pipe(fd);
    char buffer[BUFFER_SIZE];
    memset(&buffer[0], '\0', sizeof(buffer)); //clean buffer incase buffer isn't filled completly -> not bad memory

    if( (masterFD = open(SHELL_BUFFER_FILE_NAME, O_CREAT | O_RDWR, 0777)) < 0 ) {
		err_sys("Error opening file");
        return;
	}

    char **args;
    makeargv(input, " ", &args);
    // write(masterFD, previousOutput, strlen(previousOutput));

    if( (pid = fork()) == -1 ) {
        //error
    } else if( pid == 0 ) { //child FIRST
        close(fd[0]); //close input side
        dup2(masterFD, STDIN_FILENO); //redirect previous input as STDIN
        fd[1] = dup2(fd[1], STDOUT_FILENO); //redirect output to pipe buffer
        execvp(args[0], args);

        exit(0);
    } else { //parent SECOND
        while ( (wpid = wait(&status)) > 0 ) {
            //force waiting for child to complete
            // printf("Exit status of %d was %d (%s)\n", (int) wpid, status, (status > 0) ? "accept" : "reject");
        }
        close(fd[1]); // close output side
        read(fd[0], buffer, sizeof(buffer));
        close(fd[0]);
    }

    //reset buffer. first delete buffer contents then repopulate w/new data
    close(masterFD);
    if( (masterFD = open(SHELL_BUFFER_FILE_NAME, O_CREAT | O_RDWR | O_TRUNC, 0777)) < 0 ) {
		err_sys("Error opening file");
        return;
	}
    write(masterFD, buffer, strlen(buffer));
    *output = buffer;
}

void trim(char * s) {
    char * p = s;
    int l = strlen(p);

    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;

    memmove(s, p, l + 1);
}
