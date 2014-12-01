#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "client.h"
#include "client_lib.h"

/*
• It should take the server IP, the server port and a valid file name as argument.
• Call setServer with the server information.
• Open a file named "file.in" using openFile() API.
• Copy the content of the file that is taken as argument, into the file "file.in" using writeFile().
• Using statFile() print information of "file.in"
• Open another file "reverse.in".
• Read the content of the file "file.in" using readFile() API.
• Copy the content of the file "file.in" in reverse order into the file "reverse.in".
• Using statFile() print information of "reverse.in".
• Close the files.
*/

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
    int fd = openFile("filename.in");
    int fd2 = openFile("reverse.in");
    //int fd2 = openFile("/etc/shadow");
    //int fd3 = openFile("filename.in");

    // printf("got file descriptor: %d\n", fd);
    char *buf[2000];
    char *writeBuf = "This is a test";
    char *reversewriteBuf = "test a is This";
    struct fileStat *fileBuffer = malloc(sizeof(struct fileStat));
    struct fileStat *fileBuffer2 = malloc(sizeof(struct fileStat));

    readFile(fd, buf);
    writeFile(fd, writeBuf);
    statFile(fd, fileBuffer);

    readFile(fd2, buf);
    writeFile(fd2, reversewriteBuf);
    statFile(fd2, fileBuffer2);

    //readFile(fd2, buf);
    //writeFile(fd2, writeBuf);
    //statFile(fd2, fileBuffer);

    //readFile(fd3, buf);
    //writeFile(fd3, writeBuf);
    //statFile(fd3, fileBuffer);

    printf("File Size: %lld\n", (long long)fileBuffer->fileSize);
    printf("Creation Time: %lld\n", (long long)fileBuffer->creationTime);
    printf("Modifiction TIme: %lld\n", (long long)fileBuffer->modificationTime);
    printf("Access Time: %lld\n", (long long)fileBuffer->accessTime);

    printf("File Size: %lld\n", (long long)fileBuffer2->fileSize);
    printf("Creation Time: %lld\n", (long long)fileBuffer2->creationTime);
    printf("Modifiction TIme: %lld\n", (long long)fileBuffer2->modificationTime);
    printf("Access Time: %lld\n", (long long)fileBuffer2->accessTime);
    //openFile("/etc/shadow");
    puts("read 2");
    readFile(fd, buf);
    closeFile(fd);
    puts("read 3");
    readFile(fd, buf);
    puts("write");
    writeFile(fd,buf);
    closeFile(fd2);
    openFile("filename.in");
}

