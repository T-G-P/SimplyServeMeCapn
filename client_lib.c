#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "client.h"
#include "client_lib.h"

char *givenServerIP;
int givenPort;
struct sockaddr_in serverAddress;

int connectToServer() {
    int connectSocket;
    connectSocket = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = resolve(givenServerIP);
    printf("ip: %d\n", serverAddress.sin_addr.s_addr);
    serverAddress.sin_port = htons(givenPort);

    if (connect(connectSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == 0) {
        return connectSocket;
    }

    perror("Couldn't connect to server");
    return -1;
}

int ip2long(char *addressString) {
    in_addr_t addr = inet_addr(addressString);
    if (addr != INADDR_NONE) {
        printf("parse ip successful\n");
        return addr;
    } else {
        printf("parse ip failed\n");
        return INADDR_NONE;
    }
}

int resolve(char *addressString) {
    int longip = ip2long(addressString);
    if (longip != INADDR_NONE) {
        printf("ip2long success\n");
        return longip;
    }
    printf("resolving hostname\n");
    struct hostent *host = gethostbyname(addressString);
    // puts(host->h_name);
    // char* asdf = *host->h_addr_list[0];
    printf("host resolved to: '%d'\n", *(unsigned long *)(host->h_addr_list[0]));
    // printf("host resolved to: '%d.%d.%d.%d'\n", asdf[0], asdf[1], asdf[2], asdf[3]);
    return *(unsigned long *)(host->h_addr_list[0]);
}

void setServer(char *serverIP, int port) {
    givenServerIP = serverIP;
    givenPort = port;
}

int openFile(char *name) {
    char recvline[1000];
    char message[1000];
    int num_bytes;
    int socketDescriptor;

    socketDescriptor = connectToServer();
    if (socketDescriptor == -1) {
        return -1;
    }

    puts("connected");

    sprintf(message, "OPEN %s\n", name);
    sendto(socketDescriptor, message, strlen(message), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    printf("->%s", message);

    num_bytes = recvfrom(socketDescriptor, recvline, 10000, 0, NULL, NULL);
    recvline[num_bytes] = 0;

    printf("<-%s\n", recvline);

    close(socketDescriptor);
    puts("closed");

    if (strcmp(recvline, "OK") == 0) {
        return shm_open(name, O_RDWR | O_CREAT, 0600);
    } else if (strcmp(strtok(recvline, " "), "Error") == 0) {
        printf("%s\n", recvline);
        return -1;
    } else {
        puts("Malformed response\n");
        return -1;
    }

}

int readFile(int fd, void *buf) {

}

int writeFile(int fd, void *buf) {

}

int statFile(int fd, struct fileStat *buf) {

}

int closeFile(int fd) {

}

