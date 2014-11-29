#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "server.h"


int main(int argc, char *argv[]) {
    int portNum;
    char *baseDir;
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

    if (bind (listenSocket, (struct sockaddr *)&ownAddress, sizeof(ownAddress)) < 0) {
        perror("Couldn't open listening socket");
        return 1;
    }

    listen (listenSocket, 256);

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
    int readSize;
    char readBuffer[READ_SIZE];
    const char *text;
    int length;
    char *messagePart;
    char **messagePartsPtr;

    readSize = recv((long)incomingConnection, readBuffer, READ_SIZE, 0);
    // "OPEN filename\n", "READ", "WRITE", "CLOSE"
    if (readBuffer[readSize] != '\n') { //malformed request
        text = "Error, malformed request!";
        length = strlen (text) + 1;
        write (incomingConnection, &length, sizeof (length));
        /* Write the string. */
        write (incomingConnection, text, length);
        close (incomingConnection);
        printf("Malformed request: '%s'\n", readBuffer);
        return;
    }
    messagePart = strtok_r(readBuffer, " ", messagePartsPtr);
    //switch(messagePart){
    //    /* case "OPEN":
    //    //stuff
    //    break;
    //    case "READ":
    //    break;
    //    case "WRITE":
    //    break;
    //    case "CLOSE":
    //    break;
    //    */
    //    default:
    // if (strcmp(messagePart, "OPEN") == 0){
    //       // do something
    //     FILE
    //     return;
    // }
    // else{
        text = "Error, unknown command: ";
        length = strlen (text) + 1;
        write (incomingConnection, &length, sizeof (length));
        /* Write the string. */
        write (incomingConnection, text, length);
        close (incomingConnection);
        return;
    // }

}
