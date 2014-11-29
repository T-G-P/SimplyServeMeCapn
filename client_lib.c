#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "client.h"
#include "client_lib.h"

char* givenServerIP;
int givenPort;
struct sockaddr_in serverAddress;

int connectToServer() {
    int socket;
    socket = socket(AF_INET, SOCK_STREAM, 0);
    bzero (&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = givenServerIP; // 0.0.0.0
    serverAddress.sin_port = htons(givenPort);
    return connect(socket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    //while (fgets(sendline, 10000, stdin) != NULL) {

    //fputs(recvline, stdout);
   //}

}

void setServer(char* serverIP, int port) {
    givenServerIP = serverIP;
    givenPort = port;
}

int openFile(char* name) {
    char recvline[1000];
    char message[1000];
    int num_bytes
    int socketDescriptor = connectToServer();

    sprintf(message, "OPEN %s\n", name);
    sendto (socketDescriptor, message, strlen(message), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    num_bytes = recvfrom(socketDescriptor, recvline, 10000, 0, NULL, NULL);
    recvline[num_bytes] = 0;
    close(socketDescriptor);

    if(strcmp(recvline, "OK") == 0) {
        return shm_open(name, O_RDWR | O_CREAT, 0600);
    } else if (strcmp(strtok(recvline," "),"Error") == 0){
        printf("%s\n",recvline);
        return -1;
    } else {
        puts("Malformed response\n");
        return -1;
    }

}

int readFile(int fd, void* buf) {

}

int writeFile(int fd, void* buf) {

}

int statFile(int fd, struct fileStat *buf) {

}

int closeFile(int fd) {

}

