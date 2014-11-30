#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
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
    // "OPEN filename", "READ", "WRITE", "CLOSE"
    printf("Read buffer at last index %c:\n", readBuffer[readSize - 1]);
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
    } else if (strcmp(messagePart, "WRITE") == 0{
        int fd;
        char *fdString = strtok_r(NULL, " ", &messagePartsPtr);
        char *writeBuffer = strtok_r(NULL, " ", &messagePartsPtr);
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

        char message[READ_SIZE + strlen("OK ")];
        sprintf(message, "OK %s", buf);
        write(socketFd, message, strlen(message));
        close(socketFd);
        return;

    } else {
        sprintf(text, "Error, unknown command: %s", messagePart);
        /* Write the string. */
        write(socketFd, text, strlen(text));
        close(socketFd);
        return;
    }

}
