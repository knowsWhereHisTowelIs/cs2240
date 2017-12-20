/**
 * Who: Caleb Slater
 * What: A7
 * When: 2015-12-15
 * Where: CS 2240
 * How: "$./a.out"
 * http://127.0.0.1:4000
 * Stress with: $ ab -n 50 -c 10 http://127.0.0.1:4000/
 **/

/**
 * Sources http://ods.com.ua/win/eng/program/socket/socketprg.html
 * http://stackoverflow.com/questions/28763469/display-image-from-web-server-in-c
 * http://www.jbox.dk/sanos/webserver.htm
 * http://stackoverflow.com/questions/3060950/how-to-get-ip-address-from-sock-structure-in-c
 */

#include "functions.h"

int main() {
    int one = 1, client_fd;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof (cli_addr);

    // create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        err(1, "can't open socket");
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof (int));

    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(PORT);

    //bind sock
    if (bind(sock, (struct sockaddr *) &svr_addr, sizeof (svr_addr)) == -1) {
        close(sock);
        err(1, "Can't bind");
    }

    listen(sock, 5);
    fprintf(stderr, "\nStarted Server... listening\n");
    while (1) {
        //Once has recieved connection assign it to the client file
        client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
        if (client_fd == -1) {
            perror("Can't accept");
            continue;
        }
        
        //assign thread data
        clientRequestType *clientRequestStruct = malloc(sizeof(clientRequestType));
        clientRequestStruct->clientFd = client_fd;
        clientRequestStruct->clientAddr = malloc( sizeof(cli_addr) );
        memcpy(clientRequestStruct->clientAddr, &cli_addr, sizeof(cli_addr));
        
        //create thread and pass off data. Continue w/o response
        pthread_t thread;
        pthread_create(&thread, NULL, processRequest, (void*) clientRequestStruct);
    }
}
