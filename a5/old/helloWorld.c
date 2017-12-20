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

main() {
    // printf("\nHello World\n");
    char *str = "\nHello World\n";
    write(STDOUT_FILENO, str, strlen(str));
}
