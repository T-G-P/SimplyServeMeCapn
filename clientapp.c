#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "clientSNFS.h"

char *reverseBuf(char *buf){
    int i,j=0;
    char *str = (char *)malloc(strlen(buf));
    char *result = (char *)malloc(strlen(buf));
    strcpy(str,buf);
    strcpy(result,buf);
    for(i=strlen(str); i>=0; i--){
        if(str[i] !='\0'){
            while(j<strlen(str)){
                result[j] = str[i];
                j++;
                break;
            }
        }
    }
    return result;
}

int main(int argc, char **argv) {
    int portNum;
    char *hostName;
    if (argc != 5) {
        printf("Invalid # of arguments\n");
        return 1;
    }
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-port") == 0) {
            portNum = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-host") == 0) {
            hostName = argv[i + 1];
            i++;
        } else {
            printf("unknown flag %s\n", argv[i]);
            return 1;
        }
    }
    setServer(hostName, portNum);
   //int fd2 = openFile("/etc/shadow");
   //int fd3 = openFile("filename.in");

    // printf("got file descriptor: %d\n", fd);
    char *buf[2000];
    char *writeBuf = "This is a buf";
    struct fileStat *fileBuffer = malloc(sizeof(struct fileStat));
    struct fileStat *fileBuffer2 = malloc(sizeof(struct fileStat));

    int fd = openFile("filename.in");
    writeFile(fd, writeBuf);
    statFile(fd, fileBuffer);

    printf("File Size: %lld\n", (long long)fileBuffer->fileSize);
    printf("Creation Time: %lld\n", (long long)fileBuffer->creationTime);
    printf("Modifiction TIme: %lld\n", (long long)fileBuffer->modificationTime);
    printf("Access Time: %lld\n", (long long)fileBuffer->accessTime);

    int fd2 = openFile("reverse.in");
    readFile(fd, buf);
    writeFile(fd2, reverseBuf(writeBuf));
    statFile(fd2, fileBuffer2);

    printf("File Size: %lld\n", (long long)fileBuffer2->fileSize);
    printf("Creation Time: %lld\n", (long long)fileBuffer2->creationTime);
    printf("Modifiction TIme: %lld\n", (long long)fileBuffer2->modificationTime);
    printf("Access Time: %lld\n", (long long)fileBuffer2->accessTime);

    closeFile(fd);
    closeFile(fd2);

    readFile(fd2, buf);

    //readFile(fd2, buf);
    writeFile(fd2, writeBuf);
    //statFile(fd2, fileBuffer);

    //readFile(fd3, buf);
    //writeFile(fd3, writeBuf);
    //statFile(fd3, fileBuffer);


    //openFile("/etc/shadow");
    //puts("read 2");
    //readFile(fd, buf);
    //closeFile(fd);
    //puts("read 3");
    //readFile(fd, buf);
    //puts("write");
    //writeFile(fd,buf);
    //closeFile(fd2);
    //openFile("filename.in");
}

