/**
* Who: Caleb Slater
* What: A5
* When: 2015-10-31
* Where: CS 2240
* How: "$./a.out"
* Memory play
DUE 11/10

IPC (inter process communication) pipes, signals, shared memory, msg ques
**/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>

#include "shell.h"
#include "makeargv.h"

/********GLOBALS***************/
char *parsedArgs[PARSED_ARGS_SIZE];
char cwd[BUFFER_SIZE];
/******************************/

/**
STD_ERROR doesn't buffer so not dup
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

	// if (getcwd(cwd, sizeof(cwd)) == NULL) {
	// 	err_sys("Failed to get cwd");
	// }
    refreshCwd();

	do {
		printf(ANSI_COLOR_RED "\n%s$ ", cwd);	// wake the user up

        //read input
        // fgets(buffer, BUFFER_SIZE, stdin);
        buffer = get_input();
        pieces = split_line(buffer);
        numberOfArguments = makeargv(buffer, "|>", &argp);

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
            // char *temp = pieces[1];
            // octal_num = strtol(pieces[5], NULL, 8);

            if (numberOfArguments == 1) {
                umask_bits = umask(0);
                printf("%04o\n", umask_bits);
                umask(umask_bits);
            } else if (numberOfArguments == 2) {
                octal_num = strtol(argp[1], NULL, 8);
                umask_bits = umask(octal_num);
            }
        } else { // run execute
            // int num = makeargv(buffer, " |>", &argp);
            // for (int i = 0; i < num; i++) {
            //     printf("arg[%d]= '%s'\n", i, pieces[i]);
            // }
            execute(buffer);
        }
	} while( continueLoop );

	printf("\n" ANSI_COLOR_RESET);

	return 0;
}

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
 *
 */
 char **split_line(char *line) {
     char *token;
     char *tempLine;
     char *saveptr;
     char **tokens = malloc(strlen(line));
     int i = 0;
    //  int j = 0;

     tempLine = strdup(line);
    //  memcpy(tempLine, line, strlen(line));

     token = strtok_r(tempLine, " |>&", &saveptr);
     while (token != NULL) {
         tokens[i] = token;
         i++;
         token = strtok_r(NULL, " |>&", &saveptr);
     }

    //  for (j = 0; j < i; j++) {
    //      printf("%s\n", tokens[j]);
    //  }

    return tokens;
 }

/**
 * Takes buffer from stdin and parses into commands to be then used by shell
 * @param char*buffer
 * @return void
**/
void parseArgs(char *buffer)
{
	int i, j, k, parsedArgsIndex = 0;
	char **argp;
	j = makeargv(buffer, " \n", &argp);

	char tmp[BUFFER_SIZE];
	memset(&tmp[0], '\0', BUFFER_SIZE);
	int hasFoundSeperators = 0;

	for (i = 0; i < j; i++) {
		// printf("\narg[%d]='%s' length:%d", i, argp[i], strlen(argp[i]));
		for (k = 0; k < SEPERATORS_ARR_SIZE; k++) {
			// printf("\nSTRCMP(%s, %s)", seperators[k], argp[i]);
			if (strcmp(seperators[k], argp[i]) == 0) {
				//found seperator
				hasFoundSeperators = 1;
				break;
			}
		}
		// printf("\n '%s' sep:%d, i:%d parsedArgsIndex:%d", argp[i], hasFoundSeperators, i, parsedArgsIndex);
		if (hasFoundSeperators) {
			//clean tmp string
			// printf("\nTMP:%s", tmp);

			parsedArgs[parsedArgsIndex] = malloc(strlen(tmp));
			strcpy(parsedArgs[parsedArgsIndex], tmp);
			parsedArgsIndex++;

			parsedArgs[parsedArgsIndex] =
			    malloc(strlen(argp[i]));
			strcpy(parsedArgs[parsedArgsIndex], argp[i]);
			parsedArgsIndex++;

			memset(&tmp[0], '\0', BUFFER_SIZE);
			hasFoundSeperators = 0;
		} else {
			strcat(tmp, " ");
			strcat(tmp, argp[i]);
			strcat(tmp, " ");
		}

		if ((i + 1) == j) {
			// printf("\ni=j TMP:%s", tmp);

			parsedArgs[parsedArgsIndex] = malloc(strlen(tmp));
			strcpy(parsedArgs[parsedArgsIndex], tmp);
			parsedArgsIndex++;
		}
	}

	for (i = 0; i < PARSED_ARGS_SIZE; i++) {
		if (parsedArgs[i] != 0) {
			printf("\nMYARGS[%d]=%s", i, parsedArgs[i]);
		}
	}

	printf("\n");
}

/**
 * Executes args including piping
**/
void executeArgs()
{
	// char cmd[] = "ls | grep \"shell\"";
	// char cmd1[] = " ls ";
	// char cmd2[] = " grep \"shell\" ";

	int fd[2];		// an int array to be used by the pipe system call
	pid_t pid, wpid;	// the universal process id variable
	// int j; // the universal integer
	char buf[1024];		// a modest buffer
	int status = 0;
	// char **commands;

	// for( int i = 0; i < PARSED_ARGS_SIZE; i++ ) {
	for (int i = 0; i < 2; i++) {
		printf("\n!!!i=%d -> pid=%d!!!\n", i, (int) pid);

		if (parsedArgs[i] != 0) {
			if (pipe(fd) == -1) {
				err_sys("pipe error");
			}

			if ((pid = fork()) < 0) {
				err_sys("forked badly");
			}

			if (pid == 0) {	//child First
				/* Child process closes up input side of pipe */
				close(fd[0]);

				/* Send "string" through the output side of pipe */
				fd[1] = dup2(fd[1], STDOUT_FILENO);

				write(STDOUT_FILENO, "\n----------->>>In Child\n", 26);	// message to stdout

				write(STDOUT_FILENO, "\nBEFORE LS\n", 11);
				char **cmds;
				makeargv(parsedArgs[i], " ", &cmds);
				// execvp(cmds[0], cmds);
				// write(fd[1], "some characters", 15); //some piped letters
				// fd[1] = dup2(fd[1], STDOUT_FILENO); // this process' Standard Out
				// write(1, " after dup2 stuff", 021);
				// close(fd[0]);
				// write(1, "after stuff", 5);
				execlp("/bin/ls", "ls", "-l", (void *) 0);
				// write(1, "ls failed\n", 10);
				//
				// printf("\nIN CHILD");
				// printf("\n");
				exit(0);

				// is now piped
				// and finally child becomes ls
			}
			// parent Second
			if (pid > 0) {
				/* Parent process closes up output side of pipe */
				close(fd[1]);

				/* Read in a string from the pipe */
				fd[0] = dup2(fd[0], STDIN_FILENO);
				write(STDOUT_FILENO, "\n----------->>>In parent\n", 26);	// message to stdout

				int jkl =
				    read(STDIN_FILENO, buf, sizeof(buf));
				printf
				    ("\n=======START READ BUFFER===============\n");
				for (int a = 0; a < jkl; a++) {
					// printf("buf[%d]='%c'", a, buf[a]);
					printf("%c", buf[a]);
				}
				printf
				    ("\n=======END READ BUFFER===============\n");

				printf("\nThe parent read[%d]:"
				       ANSI_COLOR_GREEN " \\%s\\"
				       ANSI_COLOR_RED "\n", jkl, buf);
				// // This turns parent into Upcaser
				char *cmd[] = { "./Upcase", "./Upcase" };

				// write(STDOUT_FILENO, "\n\n-----\n\n", 9);
				// write(STDOUT_FILENO, i, sizeof(int));
				// write(STDOUT_FILENO, " => ", 4);
				// write(STDOUT_FILENO, parsedArgs[i], strlen(parsedArgs[i]));
				// write(STDOUT_FILENO, "\n\n-----\n\n", 9);

				// execvp(cmd[0], cmd); // WORKING
				execlp(cmd[0], cmd[0], (void *) 0);


				// char **cmds;
				// makeargv(parsedArgs[i], " ", &cmds);
				// execvp(cmds[0], cmds); // WORKING

				// printf("\ncmds[%d]=%s\n", 0, cmds[0]);
				// printf("\ncmds[%d]=%s\n", 1, cmds[1]);

				// char *asdf[2];
				// asdf[0] = "ls";
				// asdf[1] = 0;
				// execvp(asdf[0], asdf);
				// execvp(cmds[0], cmds);
				// execvp(parsedArgs[0], parsedArgs);

				// execvp(argvp[place], &argvp[place])s where
				// you have modified argvp array with NULL
				// in appropriate places (like where the | is)

				exit(0);
			}

			while ((wpid = wait(&status)) > 0) {
				printf(ANSI_COLOR_MAGENTA
				       "\nExit status of (WPID) %d was (STATUS)%d (%s)\n"
				       ANSI_COLOR_RED, (int) wpid, status,
				       (status > 0) ? "accept" : "reject");
			}
		}
	}
}

/**
void execute() {
    pid_t pid, wpid;
	int status = 0;	// for wait()
	// int fdl[2];
	// int fdr[2];	// Pipe File Descriptors
    int masterFD;
    if( (masterFD = open("MYBUFFER", O_CREAT | O_RDWR, 0777)) < 0 ) {
		err_sys("Error opening file");
        return;
	}

    char readBuffer[BUFFER_SIZE];
	// 0 for reading, 1 for writing ?
	write(STDOUT_FILENO, "Top Level\n", 10);	// just a landmark

	// pid = fork();		// First fork a copy of 'shell'
    int numOfArgs = 1;
    int offset = 0;

    for(int i = 0; i < numOfArgs; i++ ) {
        // if( (i+1) == numOfArgs ) {
        //     //last row
        //
        //     // Parent process closes up output side of pipe
        //     // close(fd[1]);
        //
        //     // Read in a string from the pipe
        //     read(masterFD, readBuffer, sizeof(readBuffer));
        //
        //     //last command
        //     char str[] = "\n-------Last LINE----\n";
        //     write(STDOUT_FILENO, str, strlen(str));
        //     printf("\n%s\n", readBuffer);
        //     write(STDOUT_FILENO, readBuffer, strlen(readBuffer));
        //     char str2[] = "\n------------END LAST LINE-----------\n";
        //     write(STDOUT_FILENO, str2, strlen(str2));
        //     exit(0);
        // } else {
            pid = fork();
            int fd[2];
            pipe(fd);
            char str[80];

            if( pid == 0 ) { //execute child
                // redirection

                // Child process closes up input side of pipe
                close(fd[0]);

                // Send "string" through the output side of pipe

                sprintf(str, "\nChild[%d]\n", i);
                write(STDOUT_FILENO, str, strlen(str));

                sprintf(str, "\n\tRow:%d < %d => %d\n", i, numOfArgs, (int) pid);
                write(fd[1], str, strlen(str));

                write(masterFD, str, strlen(str));

                exit(0);
            } else {
                while ((wpid = wait(&status)) > 0) {
                    printf("Exit status of %d was %d (%s)\n", (int) wpid, status, (status > 0) ? "accept" : "reject");
                }

                // read(fd[0], readBuffer, sizeof(readBuffer));
                // printf("\nfd[0]:%s\n", i, readBuffer);

                sprintf(str, "\nParent[%d]", i);
                write(STDOUT_FILENO, str, strlen(str));

                // read(fd[0], readBuffer, sizeof(readBuffer));
                // sprintf(str, "\nfd[1]:%s\n", i, readBuffer);
                // write(STDOUT_FILENO, str, strlen(str));

                //continue w/loop
                // Parent process closes up output side of pipe
                close(fd[1]);

                //Read in a string from the pipe
                read(fd[0], readBuffer, sizeof(readBuffer));
                printf("\nLine[%d]:''''''''%s''''''''\n", i, readBuffer);
                printf(ANSI_COLOR_BLUE "\nBUFF[%d]:%s\n" ANSI_COLOR_RED, i, readBuffer);
                // lseek(fd[0], offset, SEEK_SET);
                write(masterFD, readBuffer, strlen(readBuffer));
                // offset += strlen(readBuffer);
            }
        // }
    }
    printf("END");
}
**/

void execute(char *rawInputORIGANALLLLL) {
    char *processOutput = "\0"; //initialize to null string

    // char *rawInput = "ls | grep .c";
    char *rawInput = "ls | ./Upcase";
    char **argp;

    int numOfProcesses = makeargv(rawInput, "<|>", &argp);
    printf("\nNumOfProcesses:%d\n", numOfProcesses);
    for( int i = 0; i < numOfProcesses; i++ ) {
        char *cmdStr = argp[i];
        // strcat(cmdStr, "   "); //add null to end for better parsing

        printf("\n-------------\nCMDSTR:!!!%s!!!\n---------------\n", cmdStr);
        printf("\nprocessOutput[%d]:" ANSI_COLOR_BLUE "%s\n" ANSI_COLOR_RED, i, processOutput);
        char *tmp;
        handleSingleProcess(cmdStr, processOutput, &tmp);
        processOutput = tmp;
    }
    printf("\n-------------FINAL-----------\n" ANSI_COLOR_BLUE "%s" ANSI_COLOR_RED "\n------------------", processOutput);
}

/**
 * Handles a single segment of a command
 * Ex: ls | grep "file" will take 'ls' as input and return ls output
**/
void handleSingleProcess(char *input, char *previousOutput, char **output) {
    pid_t pid, wpid;
    int status = 0;	// for wait()
    int fd[2], masterFD;
    pipe(fd);
    char buffer[BUFFER_SIZE];
    memset(&buffer[0], '\0', sizeof(buffer)); //clean buffer incase buffer isn't filled completly -> not bad memory

    if( (masterFD = open("MYBUFFER", O_CREAT | O_RDWR, 0777)) < 0 ) {
		err_sys("Error opening file");
        return;
	}

    char **args;
    int processArgc = makeargv(input, " ", &args);

    if( (pid = fork()) == -1 ) {
        //error
    } else if( pid == 0 ) { //child FIRST
        // fd[0] = dup2(fd[0], STDIN_FILENO);

        // dup2(fd[0], STDIN_FILENO);
        // write(STDIN_FILENO, previousOutput, strlen(previousOutput));
        close(fd[0]); //close input side

        // write(STDOUT_FILENO, previousOutput, strlen(previousOutput));
        dup2(masterFD, STDIN_FILENO);

        //redirect input??.
        // write(fd[0], previousOutput, strlen(previousOutput));
        write(fd[1], "test", 4);
        // fd[0] = dup2(fd[0], STDIN_FILENO); //redirect fd[0] as input
        // read(STDIN_FILENO, buffer, sizeof(buffer));
        // printf("\nCHILD INPUT:" ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RED, previousOutput);
        // close(0);

        // printf("printfTestBefore");
        fd[1] = dup2(fd[1], STDOUT_FILENO); //redirect output to pipe buffer
        // close(fd[1]);
        // printf("printfTestAfter");
        // write(STDOUT_FILENO, "asdf", 4);
        // write(fd[1], "fdsa", 4);

        // char *cmd[] = { "./Upcase", "./Upcase" };
        // execvp(cmd[0], cmd); // WORKING
        // execvp(args[0], args);
        if( strcmp(input, "ls ") == 0 ) {
            write(STDOUT_FILENO, "match1", 6);
            execlp("/bin/ls", "ls", "-l", (void * ) 0);
        } else {
            write(STDOUT_FILENO, "match2", 6);
            write(STDIN_FILENO, previousOutput, strlen(previousOutput) );
            execlp("./Upcase", "./Upcase", (void * ) 0);
        }

        exit(0);
    } else { //parent SECOND
        while ( (wpid = wait(&status)) > 0 ) { //force waiting for child to complete
            printf("Exit status of %d was %d (%s)\n", (int) wpid, status, (status > 0) ? "accept" : "reject");
        }
        close(fd[1]); // close output side
        fd[0] = dup2(fd[0], STDIN_FILENO); //redirect stdin to fd[0]
        read(fd[0], buffer, sizeof(buffer));

        // close(fd[0]);
        printf("\nBuffer[%d]:\n", sizeof(buffer));

        char my[1];
        for( int i = 0; i < sizeof(buffer); i++) {
            my[0] = buffer[i];
            write(STDOUT_FILENO, my, 1);
        }
        close(fd[0]);
    }
    write(masterFD, buffer, strlen(buffer));
    *output = buffer;
}

/**
 *
**/
void execute2()
{
	pid_t pid;
	int Status;		// for wait()
	int fdl[2];
	int fdr[2];		// Pipe File Descriptors
	// 0 for reading, 1 for writing ?
	write(STDOUT_FILENO, "Top Level\n", 10);	// just a landmark

	pid = fork();		// First fork a copy of 'shell'
	if (pid == 0)		// first child named "RM" for Rightmost

	{
		write(1, "RM\n", 3);
		pipe(fdl);	// Pipe to share between M and RM
		pid = fork();	// and now it is

		if (pid > 0)	// RM is new parent; everybody is a parent
			// sometime
			// RM's child is "M" for Middle
			// And M and RM share a pipe
			// Here we are still in RM
			// (you need to keep track of context)
		{
			close(fdl[1]);	// do the plumbing
			// RM does not write to "left"
			fdl[0] = dup2(fdl[0], STDIN_FILENO);
			// But reads from left as stdin
			write(1, "Ready to Exec\n", 13);
			if (((execl("./Lower", "Lower", NULL)) != 0))
				write(1, "Lower fail\n", 15);
			// and transmogrifies
		}		// exit RM

		if (pid == 0)	// M , the child
			// M's world -- first make fdl <== fdr copy N.B. ****
		{
			write(1, "M\n", 2);
			fdr[1] = fdl[1];	// making
			// M's right <==> RM's left
			fdr[1] = dup2(fdr[1], STDOUT_FILENO);
			// and M's stdout is the pipe out
			fdr[0] = fdl[0];
			close(fdr[0]);
			// for symmetry
			pipe(fdl);	// Science Q ? We can pipe and repipe
			// fdl because they are just integers ? Yup

			pid = fork();	// M hatches LM (leftmost)

			if (pid > 0)	// Still M's world
			{
				close(fdl[1]);
				// No write on left
				// but read from pipe as from stdin
				fdl[0] = dup2(fdl[0], STDIN_FILENO);	// MIDDLE
				// and transmogrify
				write(1, "Ready to Exec in M\n", 19);
				if ((execl("Upper", "Upper", (char *) 0) !=
				     0))
					write(1, "Upper fail\n", 12);

			}	// exit M
			/* UNTIL HERE FOR MIDDLE PROCESS */

			if (pid == 0)	// LM's world
				// LM has fdl and fdr but
				// and M has messed with fdl
				// so for symmetry ....
			{
				write(1, "LM\n", 3);
				fdr[0] = fdl[0];
				fdr[1] = fdl[1];
				close(fdr[0]);
				fdr[1] = dup2(fdr[1], STDOUT_FILENO);
				// and LM has no tricks on stdin because
				// LM will not fork and no redirection '>'
				if ((execlp("ls", "./ls", "-l", (char *) 0)
				     != 0))
					write(1, "ls fail\n", 8);
				// transmogrify
			}	// exit LM

		}		// which was within M from up there at ****

	}			// back to the top level
	waitpid(pid, &Status, 0);
	write(1, "Top Level Again\n", 16);
	// end of the show, unless we have user input
}
