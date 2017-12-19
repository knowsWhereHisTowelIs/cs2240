//simple demo of using dup2 for redirection
// 10.6.15
// r. trenary
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "apue.h"

int main() 
{int fd2,fd = open("DummyFile",O_CREAT | O_TRUNC | O_RDWR  ,0777);
 ssize_t RetValue;
	if (fd < 0) err_sys("bad open");
write(STDOUT_FILENO,"some letters\n",strlen("some letters\n"));

fd= dup2( fd, STDOUT_FILENO);
RetValue = write(STDOUT_FILENO,"some letters2\n",strlen("some letters2\n"));
if (RetValue <= 0)		err_sys("bad write");

execlp("ls","ls","-l","-a",NULL) ;err_sys("failed exec");}
