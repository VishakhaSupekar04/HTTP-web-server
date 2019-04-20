#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


#if defined(_WIN32) || defined(_WIN364) || defined(__CYGWIN__)
const char* os = "Windows";
#elif defined(unix) || defined(__unix__) || defined(__unix)
const char* os = "Unix";
#elif defined(__APPLE__) && defined(__MACH__)
const char* os = "Mac OS X";
#elif defined(linux) || defined(__linux__) || defined(__linux)
const char* os = "Linux";
#elif defined(__FreeBSD__)
const char* os = "FreeBSD";
#else
const char* os = "UnKnows OS";
#endif


#define MAXLINE    1024
const int backlog = 4;

struct {
    char *ext;
    char *filetype;
} extensions [] = {
    {"gif", "image/gif" },
    {"jpg", "image/jpeg"},
    {"jpeg","image/jpeg"},
    {"png", "image/png" },
    {"zip", "image/zip" },
    {"gz",  "image/gz"  },
    {"tar", "image/tar" },
    {"htm", "text/html" },
    {"html","text/html" },
    {"php", "image/php" },
    {"cgi", "text/cgi"  },
    {"asp","text/asp"   },
    {"jsp", "image/jsp" },
    {"xml", "text/xml"  },
    {"js","text/js"     },
    {"css","test/css"   },
    {"txt","text/plain" },
    {"mp4","video/mp4" },
    
    {0,0} };


// https://cboard.cprogramming.com/c-programming/72671-displaying-file-permissions.html
int checkFileExistsAndPerm(char *fileName)
{
    printf("Need to verify %s file present or not.\n", fileName);
    struct stat buffer;
    int perm = 0;
    
    int exist = stat(fileName, &buffer);
    
    //if exists is 0 then file is present
    if(exist == 0)
    {
        //1 means permission present rwx
        if ((buffer.st_mode & S_IROTH) != 0)
            perm += 100;
        
        if ((buffer.st_mode & S_IWOTH) != 0)
            perm += 10;
        
        if ((buffer.st_mode & S_IXOTH) != 0)
            perm += 1;
    }
    else
        perm = -1;
    
    return perm;
}

char *getDateAndTime()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char s[64];
    strftime(s, sizeof(s), "%c", tm);
    return s;
}

char *getFileModifiedTime(char *path) {
    struct stat attr;
    stat(path, &attr);
    // printf("Last modified time: %s", ctime(&attr.st_mtime));
    return ctime(&attr.st_mtime);
}

off_t getFileSize(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    
    fprintf(stderr, "Cannot determine size of %s: %s\n",
            filename, strerror(errno));
    return -1;
}

void handelGetRequest(char *buffer, int fd)
{
    printf("In handleGetRequest\n");
    char *fstr;
    int buflen, len, file_fd, j, hit=1;
    long i, ret;
    char sndLine[MAXLINE];
    
    for(i=4;i<MAXLINE;i++)
    {
        if(buffer[i] == ' ' || buffer[i] == '\r')
        {
            buffer[i] = 0;
            break;
        }
    }
    
    if(!strncmp(&buffer[0], "GET /\0", 6) || !strncmp(&buffer[0], "get /\0", 6))
        (void)strcpy(buffer, "GET /index.html");
    
    buflen = strlen(buffer);
    fstr = (char *)0;
    for(i=0; extensions[i].ext != 0; i++)
    {
        len = strlen(extensions[i].ext);
        if(!strncmp(&buffer[buflen-len], extensions[i].ext, len))
        {
            fstr = extensions[i].filetype;
            break;
        }
    }
    
    bzero(sndLine, MAXLINE);
    if(fstr == 0)
    {
        (void)sprintf(sndLine, "HTTP/1.0 400 Bad Request\r\nContent-Type: %s\r\n\r\n", fstr);
        (void)write(fd, sndLine, strlen(sndLine));
        close(fd);
        return;
    }
    
    printf("File requested: %s\n", &buffer[5]);
    int exists = checkFileExistsAndPerm(&buffer[5]);
    if (exists == -1)
    {
        printf("File not found\n");
        (void)sprintf(sndLine, "HTTP/1.0 404 Not Found\r\nContent-Type: %s\r\n\r\n", fstr);
        write(fd, sndLine, strlen(sndLine));
        close(fd);
        
        return;
    }
    if (((exists/100)%10) == 0)
    {
        printf("File available, but permission denied\n");
        (void)sprintf(sndLine, "HTTP/1.0 401 Unauthorized\r\nContent-Type: %s\r\n\r\n", fstr);
        write(fd, sndLine, strlen(sndLine));
        close(fd);
        
        return;
    }
    if (((exists/100)%10) == 1)
    {
        printf("%s. file found and it have read permission\n", &buffer[5]);
        if((file_fd = open(&buffer[5], O_RDONLY)) == -1)
        {
            printf("Failed to open file.\n");
            (void)sprintf(sndLine, "HTTP/1.0 500 Internal Server Error\r\nContent-Type: %s\r\n\r\n", fstr);
            write(fd, sndLine, strlen(sndLine));
            close(fd);
            
            return;
        }
        
        bzero(sndLine, MAXLINE);
        (void)sprintf(sndLine, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
        (void)write(fd, sndLine, strlen(sndLine));
        
        printf("Going to send file data.\n");
        while ((ret = read(file_fd, sndLine, MAXLINE)) > 0)
        {
            (void)write(fd, sndLine, ret);
            bzero(sndLine, MAXLINE);
        }
    }
    close(fd);
    
}

void notSupportedResponse(char *buffer, int fd)
{
    return;
}

void handleDeleteRequest(char *buffer, int fd)
{
    printf("In handleDeleteRequest\n");
    char *fileName[MAXLINE];
    int fileNameLength, i;
    char sndLine[MAXLINE];
    
    printf("Delete Request:%s.\n", buffer);
    //DELETE /fileName
    i=8;
    fileNameLength = 0;
    while(buffer[i] != ' ')
    {
        if(buffer[i] == '\r'){
            break;
        }
        
        i++;
        fileNameLength++;
    }
    strncpy(fileName, buffer+8, fileNameLength);
    printf("File requested to delete: %s\n", fileName);
    
    int perm = checkFileExistsAndPerm(fileName);
    printf("Permission received:%d.\n", perm);
    if (perm == -1)
    {
        printf("File not found\n");
        (void)sprintf(sndLine, "HTTP/1.0 404 Not Found\r\n\r\n");
        write(fd, sndLine, strlen(sndLine));
        close(fd);
        return;
    }
    if (perm%10 == 0) //checking for other's execute permission only
    {
        printf("File available, but permission denied\n");
        (void)sprintf(sndLine, "HTTP/1.0 401 Unauthorized\r\n\r\n");
        write(fd, sndLine, strlen(sndLine));
        close(fd);
        return;
    }
    if (perm%10 == 1)
    {
        printf("%s. file found and others have execute permission\n", fileName);
        printf("Deleting file: %s\n", fileName);
        if (remove(fileName) == 0)
        {
            printf("File deleted successfully\n");
            bzero(sndLine, MAXLINE);
            (void)sprintf(sndLine, "HTTP/1.0 200 OK\r\n\r\n");
            (void)write(fd, sndLine, strlen(sndLine));
        }
        else
        {
            printf("Error deleting file\n");
            bzero(sndLine, MAXLINE);
            (void)sprintf(sndLine, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
            (void)write(fd, sndLine, strlen(sndLine));
        }
    }
    close(fd);
    return;
}

void handlePutRequest(char *buffer, int fd)
{
    char rcvLine[MAXLINE], *fileName[MAXLINE];
    int ret, i, fileNameLength;
    FILE *opfp;
    char *body;

    i=5;
    fileNameLength = 0;
    while(buffer[i] != ' ')
    {
        if(buffer[i] == '\r'){
            break;
        }
        i++;
        fileNameLength++;
    }
    strncpy(fileName, buffer+5, fileNameLength);
    printf("File Name:%s.\n", fileName);

    // printf("In Put, Request received:%s.\n", buffer);
    opfp = fopen(fileName, "w+");
    if (opfp == NULL) {
        fprintf(stderr, "Can't open output file %s!\n", fileName);
        fclose(opfp);
        // exit(1);
        return;
    }

    bzero(rcvLine, MAXLINE - 1);
    
    //ret = read(fd, rcvLine, MAXLINE);
    if (ret = read(fd, rcvLine, MAXLINE) > 0)
    {
        rcvLine[strlen(rcvLine) - 58] = 0;
        (void)fprintf(opfp, rcvLine);
        (void)sprintf(rcvLine, "HTTP/1.0 200 Ok\r\n\r\n");
        rcvLine[MAXLINE-1] = 0;
        write(fd, rcvLine, strlen(rcvLine));
        printf("Received");
        //bzero(rcvLine, MAXLINE);
    }
    
    fclose(opfp);
    close(fd);
    return;
}

void handelHeadRequest(char *buffer, int fd)
{
    printf("In Handle Head Request\n");
    char *fstr, sndLine[MAXLINE], *tempStr[MAXLINE];
    int buflen, len;
    long i;
    
    for(i=5;i<MAXLINE;i++)
    {
        if(buffer[i] == ' ' || buffer[i] == '\r' )
        {
            buffer[i] = 0;
            break;
        }
    }
    
    if(!strncmp(&buffer[0], "HEAD /\0", 7) || !strncmp(&buffer[0], "head /\0", 7))
        (void)strcpy(buffer, "HEAD /index.html");
    
    printf("Head Request received for file: %s.\n", &buffer[6]);
    
    int exists = checkFileExistsAndPerm(&buffer[6]);
    if (exists == -1)
    {
        printf("File not found\n");
        (void)sprintf(sndLine, "HTTP/1.0 404 Not Found\r\nContent-Type: %s\r\n\r\n", fstr);
        write(fd, sndLine, strlen(sndLine));
        return;
    }
    if (exists%10 == 0) //considering other's execute permission as collecting metadata of file
    {
        printf("File available, but permission denied\n");
        (void)sprintf(sndLine, "HTTP/1.0 401 Unauthorized\r\nContent-Type: %s\r\n\r\n", fstr);
        write(fd, sndLine, strlen(sndLine));
        close(fd);
        return;
    }
    if (exists%10 == 1)
    {
        printf("%s. file found and other's have execute permission\n", &buffer[6]);
        buflen = strlen(buffer);
        fstr = (char *)0;
        for(i=0; extensions[i].ext != 0; i++)
        {
            len = strlen(extensions[i].ext);
            if(!strncmp(&buffer[buflen-len], extensions[i].ext, len))
            {
                fstr =extensions[i].filetype;
                break;
            }
        }
        
        printf("File ext found:%s\n", os);
        //Get OS details
        bzero(sndLine, MAXLINE);
        strcat(sndLine, "HTTP/1.0 200 OK\r\n");
        printf("Got Http status\n");
        
        (void)sprintf(tempStr, "Content-Type: %s\r\n", fstr);
        strcat(sndLine, tempStr);
        printf("Got Content-type\n");
        
        bzero(tempStr, MAXLINE);
        (void)sprintf(tempStr, "Server: %s\r\n", os);
        strcat(sndLine, tempStr);
        printf("Got Server\n");
        
        bzero(tempStr, MAXLINE);
        (void)sprintf(tempStr, "Date: %s\r\n", getDateAndTime());
        strcat(sndLine, tempStr);
        printf("Got Date\n");
        
        strcat(sndLine, "Connection: Closed\r\n");
        printf("Got Connection CLosed\n");
        
        bzero(tempStr, MAXLINE);
        (void)sprintf(tempStr, "Last-Modified: %s", getFileModifiedTime(&buffer[6])); //\r\n get appended from getFileModifiedTime()
        strcat(sndLine, tempStr);
        printf("Got Last Modified Time\n");
        
        bzero(tempStr, MAXLINE);
        (void)sprintf(tempStr, "Content-Length: %d\r\n", getFileSize(&buffer[6]));
        strcat(sndLine, tempStr);
        
        strcat(sndLine, "\r\n");
        (void)write(fd, sndLine, strlen(sndLine));
    }
    close(fd);
}

void *clientHandler(void *arg)
{
    long ret;
    // static char buffer[MAXLINE+1];
    char buffer[MAXLINE];
    int fd = *(int*)(arg);
    int i, hit = 1;
    
    ret = read(fd, buffer, MAXLINE-1);
    printf(".%d bytes received\n", ret);
    // printf("Request:%s.\n", buffer);
    if (!strncmp(buffer, "GET ", 4) || !strncmp(buffer,"get ", 4))
        handelGetRequest(buffer, fd);
    else if (!strncmp(buffer, "HEAD ",5) || !strncmp(buffer, "head ", 5))
        handelHeadRequest(buffer, fd);
    else if (!strncmp(buffer, "PUT ", 4) || !strncmp(buffer, "put ", 4))
        handlePutRequest(buffer, fd);
    else if (!strncmp(buffer, "DELETE ", 7) || !strncmp(buffer, "delete ", 7))
        handleDeleteRequest(buffer, fd);
    else
    {
        printf("Received UnKnown Request\n");
        notSupportedResponse(buffer, fd);
    }
    return 0;
}


int main(int argc, char *argv[])
{
    printf("Started Processing.\n");
    int    listenfd, connfd, clilen;
    pthread_t tid;
    struct  sockaddr_in cliaddr, servaddr;
    
    if (argc != 3) {
        printf("Usage: caseServer <address> <port> \n");
        return -1;
    }
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        fprintf(stderr, "Error unable to create socket, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;
    }
    printf("Received arguments\n");
    bzero(&servaddr, sizeof(servaddr));
    
    servaddr.sin_family        = AF_INET;
    servaddr.sin_addr.s_addr   = inet_addr(argv[1]);
    servaddr.sin_port          = htons(atoi(argv[2]));
    
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        fprintf(stderr, "Error binding to socket, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;
    }
    
    if (listen(listenfd, backlog) == -1) {
        fprintf(stderr, "Error listening for connection request, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;
    }
    
    while (1)
    {
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0 )
        {
            if (errno == EINTR)
                continue;
            else
            {
                fprintf(stderr, "Error connection request refused, errno = %d (%s) \n",
                        errno, strerror(errno));
            }
        }
        
        if (pthread_create(&tid, NULL, clientHandler, (void *)&connfd) != 0)
        {
            fprintf(stderr, "Error unable to create thread, errno = %d (%s) \n",
                    errno, strerror(errno));
        }
        
        pthread_join(tid,NULL);
    }
    
    //return 0;
}

