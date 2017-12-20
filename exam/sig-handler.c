#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static void
handle_sigint(int signum) {
    printf("\nSIGINT(%d) has been raised in %d!\n", signum, getpid());
}

void register_signals() {
    struct sigaction sig_handler;

    // ctrl-c
    sig_handler.sa_handler = handle_sigint;
    sigemptyset(&sig_handler.sa_mask);
    sig_handler.sa_flags = 0;

    sigaction(SIGINT, &sig_handler, NULL);

    /*
    // Child has exited
    sig_handler.sa_handler = handle_sigchld;
    sigemptyset(&sig_handler.sa_mask);
    sig_handler.sa_flags = SA_RESTART;

    sigaction(SIGCHLD, &sig_handler, NULL);
    */
}

int
main(int argc, char *argv[]) {
    register_signals();
    char *line_buffer = malloc(300);

    while (1) {
        fgets(line_buffer, 300, stdin);
        if (strcmp(line_buffer, "exit\n") == 0) {
            printf("Exiting\n");
            exit(1);
        }
    }
}
