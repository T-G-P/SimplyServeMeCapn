#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "server.h"

char *baseDir;

int main(int argc, char *argv[]) {
    int portNum;
    if (argc != 5) {
        printf("Invalid # of arguments\n");
        return 1;
    }
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-port") == 0) {
            portNum = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-mount") == 0) {
            baseDir = argv[i + 1];
            i++;
        } else {
            printf("unknown flag %s\n", argv[i]);
            return 1;
        }
    }

    int listenSocket;
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in peerAddress, ownAddress;
    ownAddress.sin_family = AF_INET;
    ownAddress.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    ownAddress.sin_port = htons(portNum);

    if (bind(listenSocket, (struct sockaddr *)&ownAddress, sizeof(ownAddress)) < 0) {
        perror("Couldn't open listening socket");
        return 1;
    }

    listen(listenSocket, 256);

    pthread_t threadId;
    int incomingConnection;
    int sockaddr_in_size = sizeof(peerAddress);
    while (incomingConnection = accept(listenSocket, (struct sockaddr *)&peerAddress, &sockaddr_in_size)) {
        if (pthread_create(&threadId, NULL, connectionHandler, (void *) &incomingConnection) < 0) {
            perror("Couldn't create thread");
            return 1;
        }
    }

    if (incomingConnection < 0) {
        perror("Couldn't accept incoming connection");
        return 1;
    }

    return 0;
}

const READ_SIZE = 2048;

void *connectionHandler(void *incomingConnection) {
    int socketFd = *((int *) incomingConnection);
    printf("socket fd: %d\n", socketFd);
    int readSize;
    char readBuffer[READ_SIZE];
    char errorBuffer[READ_SIZE];
    char text[READ_SIZE];
    int length;
    char *messagePart;
    char *messagePartsPtr;

    readSize = recv(socketFd, readBuffer, READ_SIZE, 0);
    printf("Number of bytes for message: %d\n", readSize);
    printf("readBuffer: %s\n", readBuffer);
    // "OPEN filename", "READ", "WRITE", "CLOSE"
    // printf("Read buffer at last index %c:\n", readBuffer[readSize - 1]);
    // if (readBuffer[readSize - 1] != '\n') { //malformed request
    //     text = "Error, malformed request!";
    //     /* Write the string. */
    //     write(socketFd, text, strlen(text));
    //     close(socketFd);
    //     printf("Malformed request: '%s'\n", readBuffer);
    //     return;
    // }

    messagePart = strtok_r(readBuffer, " ", &messagePartsPtr);
    printf("Message part: %s\n", messagePart);

    if (strcmp(messagePart, "OPEN") == 0) {
        FILE *fp;
        int fd;
        char *fileName = strtok_r(NULL, messagePart, &messagePartsPtr);
        printf("Opening file: %s\n", fileName);
        char *filePath = malloc(strlen(fileName) + strlen(baseDir) + 1 + 1);
        printf("The path: %s, The file: %s\n", baseDir, fileName);
        sprintf(filePath, "%s/%s", baseDir, fileName);
        printf("Opening file: %s\n", filePath);
        char successMessage[2048];
        fp = fopen(filePath, "ab+");
        if (fp == NULL) {
            perror("Error opening file");
            strerror_r(errno, errorBuffer, READ_SIZE);
            write(socketFd, errorBuffer, strlen(errorBuffer));
            close(socketFd);
        }
        else {
            // success
            fd = fileno(fp);
            sprintf(successMessage, "OK %d", fd);
            write(socketFd, successMessage, strlen(successMessage));
            close(socketFd);
            // fclose(fp);
        }
        return;
    } else if (strcmp(messagePart, "READ") == 0) {
        int fd;
        char *fdString = strtok_r(NULL, " ", &messagePartsPtr);
        fd = atoi(fdString);
        printf("wants to read fd %d\n", fd);

        char *buf[READ_SIZE];
        ssize_t readBytes = read(fd, buf, READ_SIZE);
        if (readBytes == -1) {
            perror("Couldn't read file");
            char errorBuffer[1000];
            strerror_r(errno, errorBuffer, sizeof(errorBuffer));
            sprintf(text, "Error, couldn't read file: %s", errorBuffer);
            write(socketFd, text, strlen(text));
            close(socketFd);
            return;
        }
        printf("read %d bytes from fd %d\n", (int) readBytes, fd);

        char message[READ_SIZE + strlen("OK ")];
        sprintf(message, "OK %s", buf);
        write(socketFd, message, strlen(message));
        close(socketFd);
        return;
    } else if (strcmp(messagePart, "WRITE") == 0) {
        int fd;
        char *fdString = strtok_r(NULL, " ", &messagePartsPtr);
        char *writeBuffer = readBuffer + strlen("WRITE ") + strlen(fdString) + strlen(" ");
        // char *writeBuffer = strtok_r(NULL, " ", &messagePartsPtr);
        printf("asked to write to fd %s the following data: %s\n", fdString, writeBuffer);
        char *buf[READ_SIZE];
        fd = atoi(fdString);
        ssize_t writeBytes = write(fd, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1) {
            perror("Couldn't write file");
            char errorBuffer[1000];
            strerror_r(errno, errorBuffer, sizeof(errorBuffer));
            sprintf(text, "Error, couldn't write file: %s", errorBuffer);
            write(socketFd, text, strlen(text));
            close(socketFd);
            return;
        }
        printf("wrote %d bytes to fd %d\n", writeBytes, fd);
        char message[READ_SIZE];
        sprintf(message, "OK %d", writeBytes);
        write(socketFd, message, strlen(message));
        close(socketFd);
        return;
    }else if (strcmp(messagePart, "STAT") == 0){
        int fd;
        char *fdString = strtok_r(NULL, " ", &messagePartsPtr);
        //char *statBuffer = readBuffer + strlen("STAT ") + strlen(fdString) + strlen(" ");
        fd = atoi(fdString);
        //need to call fstat with fd and stat struct
        struct stat *fstatBuffer = malloc(sizeof(struct stat));
        // memcpy(fstatBuffer,statBuffer,sizeof(fstatBuffer));

        int fstatResponse = fstat(fd, fstatBuffer);
        if (fstatResponse == -1) {
            perror("Couldn't STAT file");
            char errorBuffer[1000];
            strerror_r(errno, errorBuffer, sizeof(errorBuffer));
            sprintf(text, "Error, couldn't STAT file: %s", errorBuffer);
            write(socketFd, text, strlen(text));
            close(socketFd);
            return;
        }

        struct fileStat *fileStatBuffer = malloc(sizeof(struct fileStat));
        fileStatBuffer->fileSize = fstatBuffer->st_size;
        printf("The size of the file: %lld\n",fstatBuffer->st_size);
        long long fileSize = fstatBuffer->st_size;
        fileStatBuffer->creationTime = fstatBuffer->st_ctime;
        printf("The creation time of the file: %lld\n",fstatBuffer->st_ctime);
        fileStatBuffer->accessTime = fstatBuffer->st_atime;
        printf("The access time of the file: %lld\n",fstatBuffer->st_atime);
        fileStatBuffer->modificationTime = fstatBuffer->st_mtime;
        printf("The modification time of the file: %lld\n",fstatBuffer->st_mtime);
        char message[READ_SIZE];
        //sprintf(message, "OK %s", fstatBuffer);
        sprintf(message, "OK %lld %lld %lld %lld",fstatBuffer->st_size,fstatBuffer->st_ctime,fstatBuffer->st_atime,fstatBuffer->st_mtime);
        //assert(strlen("OK ") + sizeof(fstatBuffer) <= sizeof(message));
        //memcpy(message + strlen(message), fstatBuffer, sizeof(fstatBuffer));
        // memcpy(message + READ_SIZE, fstatBuffer, sizeof(fstatBuffer));
        puts(message);
        write(socketFd, message, strlen(message));
        close(socketFd);
        return;

    }else if (strcmp(messagePart, "CLOSE") == 0) {
        puts("want to close file");
        int fd;
        char *fdString = strtok_r(NULL, " ", &messagePartsPtr);
        //char *statBuffer = readBuffer + strlen("STAT ") + strlen(fdString) + strlen(" ");
        fd = atoi(fdString);
        int closedFile = close(fd);
        char successMessage[2048];
        if (closedFile == -1) {
            perror("Error closing file");
            strerror_r(errno, errorBuffer, READ_SIZE);
            write(socketFd, errorBuffer, strlen(errorBuffer));
            close(socketFd);
        }
        else {
            // success
            sprintf(successMessage, "OK %d", fd);
            write(socketFd, successMessage, strlen(successMessage));
            close(socketFd);
            // fclose(fp);
        }
        return;
    } else {
        sprintf(text, "Error, unknown command: %s", messagePart);
        /* Write the string. */
        write(socketFd, text, strlen(text));
        close(socketFd);
        return;
    }

}
