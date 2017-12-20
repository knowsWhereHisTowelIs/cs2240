//
//  main.c
//  a5_project
//
//  Created by Benjamin Masters on 10/20/15.
//  Copyright © 2015 Benjamin Masters. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include "makeargv.h"
#include "apue.h"

/* I like this */
typedef int boolean;
#define true 1
#define false 0

char **split_line(char*);

/*
 *
 */
char *get_redir_filename(char *line) {
    char *token;
    char *tempLine;
    char *saveptr;

    tempLine = strdup(line);

    token = strtok_r(tempLine, ">", &saveptr);
    token = strtok_r(NULL, " ", &saveptr);

    return token;
}

/*
 *
 */
boolean do_magic(char **args, int num_args, int num_pipes, boolean redirection, char *redir_filename) {
    boolean success = false;
    int output_fd;
    int fd[2];
    int pipe_result;
    int pid;

    if (redirection) {
        output_fd = open(redir_filename, O_CREAT | O_TRUNC | O_WRONLY, 0777);
        if (output_fd == -1) {
            err_sys("Couldn't open %s\n", redir_filename);
        }

//        pipe_result = pipe(fd);
//        if (pipe_result == -1) {
//            err_sys("Couldn't create pipe!\n");
//        }

        pid = fork();
//        kill(0, SIGSTOP);
        if (pid == -1) {
            err_sys("Couldn't fork!\n");
        }

        if (pid == 0) {
//            close(fd[0]);
            dup2(STDOUT_FILENO, output_fd);
            execvp(args[0], args);
        }

    } else {

    }

    return success;
}

/*
 *
 */
int execute_commands(char *line, char **args, int num_args) {
    int num_pipes = 0;
    int num_redirs = 0;
    int background = false;
    unsigned long command_length;
    char **pieces;
    int i;
    char *redirFileName = NULL;
    pid_t child_pid;

    boolean success;

    command_length = strlen(line);
    pieces = split_line(line);

    for (i = 0; i < command_length; i++) {
        if (line[i] == '|') {
            num_pipes++;
        } else if (line[i] == '>') {
            num_redirs++;
            redirFileName = get_redir_filename(line);
        }
    }

    if (line[command_length - 1] == '&') {
        background = true;
        num_args--;
    }

//    success = do_magic(args, num_args, num_pipes, num_redirs, redirFileName);

    if (background) {

    } else {
        if (num_pipes == 0) {
            child_pid = fork();
            if (child_pid == 0) {
                execvp(args[0], args);
//                exit(0);
            } else {
                wait(0);
            }
        } else if (num_pipes == 1) {

        } else if (num_pipes > 1) {

        }

        wait(NULL);
    }
//    printf("num pipes: %d\n", num_pipes);
//    printf("num redirection: %d\n", num_redirs);
//    printf("background enabled: %d\n", background);

//    printf("command length: %d\n", command_length);

    return 0;
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
//    int j = 0;

    tempLine = strdup(line);
//    memcpy(tempLine, line, strlen(line));

    token = strtok_r(tempLine, " |>&", &saveptr);
    while (token != NULL) {
        tokens[i] = token;
        i++;
        token = strtok_r(NULL, " |>&", &saveptr);
    }

//    for (j = 0; j < i; j++) {
//        printf("%s\n", tokens[j]);
//    }

    return tokens;
}

/*
 *
 */
int num_commands(char *line) {
    char *token;
    char *tempLine;
    char *saveptr;
    int numTokens = 0;

    tempLine = strdup(line);
    token = strtok_r(tempLine, "|", &saveptr);
    while (token != NULL) {
        numTokens++;
        token = strtok_r(NULL, "|", &saveptr);
    }

    return numTokens;
}

/*
 * Reads console input and returns the line
 */
char *get_input() {
    int buffer_size = 64;
    char ch;
    char *buffer = malloc(sizeof(char) * buffer_size);
    int i = 0;

    while ((ch = getchar()) != '\n')
    {
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
 * Loop until we get an exit command
 */
void shell_loop() {
    int loop = 1;
    int num_arguments;
    char *line;
    char **pieces;
    char **derp;
    int chdir_status;
    mode_t umask_bits;
    long octal_num;

    char* cwd;
    char buff[PATH_MAX + 1];

    int i;

    do {
        printf("bsh> ");
        line = get_input();
        pieces = split_line(line);
        num_arguments = makeargv(line, " |>", &derp);
        if (strcasecmp(line, "exit") == 0) {
            loop = 0;
        } else if (strcasecmp(pieces[0], "cd") == 0) {
            chdir_status = chdir(pieces[1]);
            if (chdir_status < 0) {
                int err = errno;
                if (err == ENOENT) {
                    printf("-bsh: cd: %s: No such file or directory\n", pieces[1]);
                }
            }
        } else if (strcasecmp(pieces[0], "pwd") == 0) {
            cwd = getcwd( buff, PATH_MAX + 1 );
            if( cwd != NULL ) {
                printf("%s\n", cwd );
            }
        } else if (strcasecmp(pieces[0], "umask") == 0) {
            //char *temp = pieces[1];
//            octal_num = strtol(pieces[5], NULL, 8);

            if (num_arguments == 1) {
                umask_bits = umask(0);
                printf("%04o\n", umask_bits);
                umask(umask_bits);
            } else if (num_arguments == 2) {
                octal_num = strtol(pieces[1], NULL, 8);
                umask_bits = umask(octal_num);
            }

        } else if (strcasecmp(line, "empty") != 0) {
//            makeargv(line, " |>", &derp);
//            num_arguments--;
            for (i = num_arguments - 1; i >= 0; i--) {
                printf("arg %d = %s\n", i, derp[i]);
            }
            execute_commands(line, derp, num_arguments);
        }

        free(line);
    } while (loop);
}

/*
 * Simple main
 */
int main(int argc, const char * argv[]) {

    shell_loop();

    return 0;
}
