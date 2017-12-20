//simple demo of using dup2 for redirection
// 10.6.15
// r. trenary
// and now it has been elaborated into demo
// of execl, execvp, and makeargv with the latter
// 10.8.15
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "apue.h"
int makeargv(char *s, char *delimiters, char ***argvp);
int main(){
	int fd2,fd = open("DummyFile",O_CREAT | O_TRUNC | O_RDWR  ,0777);
    char * argv[10];
    argv[0]="ls";argv[1]="-l";argv[2]=NULL;
    argv[3]="ls";argv[4]="-l";argv[5]="-a";argv[6]=NULL;
    ssize_t RetValue;
    if (fd < 0) err_sys("bad open");
    write(STDOUT_FILENO,"some lettersn",strlen("some lettersn"));
    fd= dup2( fd, STDOUT_FILENO);// Atomic !!!!
    RetValue = write(STDOUT_FILENO,"some letters2n",strlen("some letters2n"));
    if (RetValue <= 0) err_sys("bad write");
    //execvp(argv[0],&argv[0]);
    // execvp(argv[3],&argv[3]);
    //    execlp("ls","ls","-l","-a",NULL) ;

    write(1,"nPrompt>",8); // wake the user up
    char **argp;
    int i,j; // the usual suspects
    char buf[80];
    if (strcmp(fgets(buf, 80, stdin), "exitn")!=0)
    // and if no "exit"
    j=makeargv(buf,"t n",&argp);
    execvp(argp[0],&argp[0]);
    err_sys("failed exec");
}
