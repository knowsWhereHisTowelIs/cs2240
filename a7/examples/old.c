
typedef struct httpResponse {
    char *code;
    char *contentType;
    char *content;
} httpResponseType;
typedef httpResponseType * httpResponsePtr;

char responseDefault[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
"<style>body { background-color: #111 }"
"h1 { font-size:4cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1>Goodbye, world!</h1></body></html>\r\n";
char responseImgDefault[] = "";

void getResponse(char *str, char *content);

void getResponse(char *str, char *content){
    
}

//, httpResponseType *response, int contentSize
int httpResponseToString(char **str) {
    httpResponseType response;
    response.code = "HTTP/1.1 200 OK\r\n";
//    response.contentType = "Content-Type: text/html; charset=UTF-8\r\n\r\n";
//    response.contentType = "Content-Type: image/jpeg;\r\n\r\n";
    response.contentType = "Content-Type: image/gif;\r\n\r\n";
    
    response.content = "<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
        "<style>body { background-color: #111 }"
        "h1 { font-size:4cm; text-align: center; color: black;"
        " text-shadow: 0 0 2mm red}</style></head>"
        "<body><h1>Goodbye, world!</h1></body></html>\r\n";
    
    char buffer[BUFFER_SIZE];
    int fp, n, contentLength = 0;
    char filename[] = "2x2.gif";
    if( (fp = open(filename, O_RDONLY)) > 0 ) {
        if( (n = read(fp, buffer, BUFFER_SIZE)) > 0 ) {
            //TODO FIX ONLY ONE ITERATION
            response.content = (char *) malloc(n * sizeof(char));
            memset(&response.content[0], '\0', sizeof(n)); //clean buffer incase buffer isn't filled completly -> not bad memory
            contentLength += n;
            //memcpy(response.content, &buffer[0], n);
            //strncpy(response.content, buffer, n);
            printf("\nBUFFER[%d]:", n);
            for(int i = 0; i < n; i++){ 
                printf("%c", buffer[i]);
                response.content[i] = buffer[i];
            }
        } else {
            printf("\nFailed to read n:%d \n", n);
        }
    } else {
        printf("\nFAILED to open %s\n", filename);
    }
//    printf("\n CONTENT!!!:'''''''''''' %s '''''''''''\n", (char *) response.content);
    printf("\n CONTENT!!!:''''''''''''");
    for(int i = 0; i < n; i++){ 
        printf("\n[%d]:%c", i, response.content[i]);
    }
    printf("\n '''''''''''\n");
    /**
     * 
    for(int i = 0; i < n; i++){ 
        printf("\n[%d]:%c", i, ((char*) response.content)[i]);
    }
    printf("\n");
    write(STDOUT_FILENO, "\nContent:''''''''''''", 21);
    write(STDOUT_FILENO, (char *) response.content, strlen((char *) response.content));
    write(STDOUT_FILENO, "'''''''''''\n", 12);
     */
    
    int responseLength = strlen(response.code) + strlen(response.contentType) + contentLength + 2;
    
    *str = malloc(responseLength * sizeof(char));
    strncat(*str, response.code, strlen(response.code));
    strncat(*str, response.contentType, strlen(response.contentType));
    strncat(*str, response.content, strlen(response.content));
    
    
    printf("\n-------__RESPONSE__[%d]-----------", responseLength);
    printf("\nTotal:%d code:%d, type:%d, content:%d \n" ANSI_COLOR_GREEN, responseLength, 
            (int)strlen(response.code), (int)strlen(response.contentType), contentLength
    );
    for(int i = 0; i < responseLength; i++){ 
        printf("%c", (*str)[i]);
    }
    printf(ANSI_COLOR_RESET "\n-------END__RESPONSE__[%d]-----------\n\n\n", responseLength);
    
    return responseLength;
}

void sampleHtmlResponse(FILE *responseFile) {
    char *content = "<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
            "<style>body { background-color: #111 }"
            "h1 { font-size:4cm; text-align: center; color: black;"
            " text-shadow: 0 0 2mm red}</style></head>"
            "<body><h1>Goodbye, world!</h1></body></html>";
    sendHeaders(responseFile, 200, "OK", NULL, getMimeType(".html"), strlen(content));
    fprintf(responseFile, "%s", content);
    //printf("SampleHTMLNEW()\n%s", content);
}

void sampleFileResponse(FILE *responseFile) {
    //char *path = "/home/mini/Dropbox/current_semester/cs2240/a7/sound.wav";
    char *path = "sound.wav";
    FILE *file = fopen(path, "r");
    struct stat statbuf;
    int n;
    char buffer[BUFFER_SIZE];

    if (!file) {
        char *msg = "Access denied.";
        sendHeaders(responseFile, 403, "Forbidden", NULL, getMimeType(".html"), strlen(msg));
        fprintf(responseFile, "%s", msg);
    } else if (stat(path, &statbuf) < 0) {
        printf("\n\nERROR\n\n");
    } else if (S_ISDIR(statbuf.st_mode)) {
        printf("\nIs directory");
    } else {
        int length = S_ISREG(statbuf.st_mode) ? statbuf.st_size : -1;
        sendHeaders(responseFile, 200, "OK", NULL, getMimeType(path), length);

        //printf("\n-------%s length:%d-------\n", path, length);
        while ((n = fread(buffer, 1, sizeof (buffer), file)) > 0) {
            fwrite(buffer, 1, n, responseFile);
            //printf("%s", buffer);
        }
        //printf("\nEND\n");
        fclose(file);
    }
}