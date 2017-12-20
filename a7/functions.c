#include "functions.h"

//---------GLOBALS----------
static int clientsSize = 0;
static int clientsLastIndex = 0;
static clientAccountType **clients;
//---------END GLOBALS----------

/**
 * NOTES
 * use stderr instead of stdout so nothing is buffered and spits out in the correct order
 */

/**
 * Handles client reques
 * @param argument Struct with client file and sockaddr_in
 * @return 
 */
void* processRequest(void *argument) {
    //parse out argument
    clientRequestType clientRequestStruct = *(clientRequestType *) argument;
    int clientFd = clientRequestStruct.clientFd;
    struct sockaddr_in cliAddr = *(clientRequestStruct.clientAddr);

    //output client IPv4 address
    fprintf(stderr, "\n----------------------------------------------------------\nHandling ip:' %d.%d.%d.%d '",
            (cliAddr.sin_addr.s_addr & 0xFF),
            (cliAddr.sin_addr.s_addr & 0xFF00) >> 8,
            (cliAddr.sin_addr.s_addr & 0xFF0000) >> 16,
            (cliAddr.sin_addr.s_addr & 0xFF000000) >> 24
            );

    //find out if client has been blocked
    int clientStatus = updateClients(cliAddr.sin_addr.s_addr);
    char *msg;
    //DDOS PROTECTION
    if (clientStatus == ACCEPTED_CLIENT_STATUS) {
        msg = "ACCEPTED";
        
        //client accepeted prepare their response
        FILE *responseFile = fdopen(clientFd, "a+");
        char buffer[BUFFER_SIZE];
        char *method, *path, *protocol;
        struct stat statbuf;

        //get REQUEST
        if (!fgets(buffer, sizeof (buffer), responseFile)) {
            fprintf(stderr, "\nError with buffer");
            pthread_exit(NULL);
        }

        char *requestCopy = malloc(strlen(buffer) + 1);
        strncpy(requestCopy, buffer, strlen(buffer) + 1);

        //parse REQUEST
        method = strtok_r(requestCopy, " ", &requestCopy);
        path = strtok_r(requestCopy, " ", &requestCopy);
        protocol = strtok_r(requestCopy, "\r", &requestCopy);

        if (!method || !path || !protocol) {
            fprintf(stderr, "\nError with method/path/protocol");
            pthread_exit(NULL);
        }

        if (strstr(path, "..") != NULL) {
            //error user trying to access files outside of htdocs
            msg = "Very rude snooping round my PC.";
            sendError(responseFile, 403, "Forbidden", NULL, msg);
            fprintf(stderr, "\nUser requested:%s and it is forbidden!!!", path);
        } else {
            //valid REQUEST
            if (path[0] == '/') {
                //set index file if no file selected
                if (strlen(path) == 1) {
                    strcpy(path, "/index.html");
                }

                //set htdocs to be directory where C looks for files
                //so localhost/[FILE] becomes ./htdocs/[FILE]
                char *tmp = malloc(sizeof(HTDOCS_RELATIVE_PATH) + strlen(path));
                strcpy(tmp, HTDOCS_RELATIVE_PATH);
                strcat(tmp, path);
                strcpy(path, tmp);
                free(tmp);
            }
            fprintf(stderr, "\nRequested File: %s", path);

            sendFile(responseFile, path, &statbuf);
        }
        //end connection
        fclose(responseFile);
    } else {
        msg = "BLOCKED";
        //blocked user... 
        //must close file so process isn't stuck
        close(clientFd); 
    }
    fprintf(stderr, "\n%s connection\n", msg);

    //done w/thread
    pthread_exit(NULL);
}

/**
 * Stops DDOS attacks.
 * @param ip
 * @return status -1 blocked, else 1 for success
 */
int updateClients(int ip) {
    int status = 0;
    boolean hasFoundClient = false;

    for (int i = 0; i < clientsLastIndex; i++) {
        if (clients[i]->ip == ip) {
            hasFoundClient = true;
            fprintf(stderr, "\nFound client w/hits %d", clients[i]->hits);
            // if too many hits blacklist
            if (clients[i]->hits > MAX_HITS_UNTIL_BLOCKED) {
                fprintf(stderr, " CLIENT BLOCKED");
                //report as blocked
                status = BLOCKED_CLIENT_STATUS;
            } else {
                fprintf(stderr, " CLIENT ACCEPTED");
                //update hits
                clients[i]->hits++;
                status = ACCEPTED_CLIENT_STATUS;
            }
            break;
        }
    }

    //hasn't found client so create client
    if (hasFoundClient == false) {
        //reallocate client list if need to
        if (clientsLastIndex == clientsSize) {
            clientsSize += 50;
            clients = (clientAccountType **) realloc(clients, (clientsSize * sizeof (clientAccountType*)));
        }

        fprintf(stderr, "\nAdding Client[%d] clientSize:%d IP:' %d.%d.%d.%d ' ",
                clientsLastIndex,
                clientsSize,
                (ip & 0xFF),
                (ip & 0xFF00) >> 8,
                (ip & 0xFF0000) >> 16,
                (ip & 0xFF000000) >> 24
                );

        // setup client info
        clientAccountType client;
        client.ip = ip;
        client.hits++;

        //assign client info
        clientsAcountPtr clientPtr = malloc(sizeof (clientAccountType));
        *clientPtr = client;
        clients[clientsLastIndex] = clientPtr;

        status = ACCEPTED_CLIENT_STATUS;
        clientsLastIndex++;
    }

    return status;
}

/**
 * Checks if file exists in folder if it does then output it else handle errors
 * @param responseFile
 * @param path
 * @param statbuf
 */
void sendFile(FILE *responseFile, char *path, struct stat *statbuf) {
    FILE *file = fopen(path, "r");
    int n;
    char buffer[BUFFER_SIZE];
    char *msg;

    if (!file) {
        //error if can't open file
        msg = "404 File not found.";
        sendError(responseFile, 404, "File Not Found", NULL, msg);
    } else if (stat(path, statbuf) < 0) {
        //error if file not found
        msg = "404 File not found.";
        sendError(responseFile, 404, "File Not Found", NULL, msg);
    } else if (S_ISDIR(statbuf->st_mode)) {
        //error if looking at directory
        msg = "Don't look at my directory.";
        sendError(responseFile, 403, "Forbidden", NULL, msg);
    } else {
        //found file
        msg = "Found File";
        int length = S_ISREG(statbuf->st_mode) ? statbuf->st_size : -1;
        //send http header
        sendHeaders(responseFile, 200, "OK", NULL, getMimeType(path), length);

        //send http content
        while ((n = fread(buffer, 1, sizeof (buffer), file)) > 0) {
            fwrite(buffer, 1, n, responseFile);
        }
        fclose(file);
    }
    fprintf(stderr, "\n%s", msg);
}

/**
 * Output errors ex 404, 403
 * @param responseFile
 * @param status
 * @param title
 * @param extra
 * @param text
 */
void sendError(FILE *responseFile, int status, char *title, char *extra, char *text) {
    char *output = malloc(BUFFER_SIZE * sizeof (char));
    sprintf(output,
            "<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\r\n"
            "<BODY><H4>%d %s</H4>\r\n"
            "%s\r\n"
            "</BODY></HTML>\r\n",
            status, title,
            status, title,
            text
            );
    sendHeaders(responseFile, status, title, extra, getMimeType(".html"), strlen(output));
    fprintf(responseFile, "%s", output);
}

/**
 * Send http headers
 * @param f
 * @param status
 * @param title
 * @param extra
 * @param mime
 * @param length
 */
void sendHeaders(FILE *f, int status, char *title, char *extra, char *mime, int length) {
    int headerLength = 0;
    headerLength += fprintf(f, "%s %d %s\r\n", PROTOCOL, status, title);
    headerLength += fprintf(f, "Server: %s\r\n", SERVER);
    if (extra)          headerLength += fprintf(f, "%s\r\n", extra);
    if (mime)           headerLength += fprintf(f, "Content-Type: %s\r\n", mime);
    if (length >= 0)    headerLength += fprintf(f, "Content-Length: %d\r\n", length);
    headerLength += fprintf(f, "Connection: close\r\n");
    headerLength += fprintf(f, "\r\n");
    
    char *header = malloc(headerLength);
    int tmpLength = 0;
    tmpLength += sprintf(header+tmpLength, "%s %d %s\r\n", PROTOCOL, status, title);
    tmpLength += sprintf(header+tmpLength, "Server: %s\r\n", SERVER);
    if (extra)          tmpLength += sprintf(header+tmpLength, "%s\r\n", extra);
    if (mime)           tmpLength += sprintf(header+tmpLength, "Content-Type: %s\r\n", mime);
    if (length >= 0)    tmpLength += sprintf(header+tmpLength, "Content-Length: %d\r\n", length);
    tmpLength += sprintf(header+tmpLength, "Connection: close\r\n");
    tmpLength += sprintf(header+tmpLength, "\r\n");
    
    printf("\n-----RAW RESPONSE----------\n%s\n----- END RAW RESPONSE----------\n", 
            header
    );
}

/**
 * Return mime type based on filename
 * @param name
 * @return 
 */
char *getMimeType(char *name) {
    char *ext = strrchr(name, '.');
    if (!ext) return NULL;
    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".au") == 0) return "audio/basic";
    if (strcmp(ext, ".wav") == 0) return "audio/wav";
    if (strcmp(ext, ".avi") == 0) return "video/x-msvideo";
    if (strcmp(ext, ".mpeg") == 0 || strcmp(ext, ".mpg") == 0) return "video/mpeg";
    if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
    return NULL;
}