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
#include <sys/types.h>
#include <unistd.h>
#include "clientSNFS.h"

char *givenServerIP;
int givenPort;
struct sockaddr_in serverAddress;

const NETWORK_BUFFER_LENGTH = 2000;

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
        puts("ip2long success");
        return longip;
    }
    puts("resolving hostname");
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
    char recvline[NETWORK_BUFFER_LENGTH];
    char message[NETWORK_BUFFER_LENGTH];
    int num_bytes;
    int socketDescriptor;
    int fd;

    socketDescriptor = connectToServer();
    if (socketDescriptor == -1) {
        return -1;
    }

    puts("connected");

    sprintf(message, "OPEN %s", name);
    sendto(socketDescriptor, message, strlen(message), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    printf("->%s\n", message);

    num_bytes = recvfrom(socketDescriptor, recvline, NETWORK_BUFFER_LENGTH, 0, NULL, NULL);
    recvline[num_bytes] = 0;

    printf("<-%s\n", recvline);

    //close(socketDescriptor);
    puts("closed");

    if (strncmp(recvline, "OK", 2) == 0) {
        char *fdString = recvline + 3;
        fd = atof(fdString);
        return fd;
    } else if (strcmp(strtok(recvline, " "), "Error") == 0) {
        printf("%s\n", recvline);
        return -1;
    } else {
        puts("Malformed response");
        return -1;
    }
}

int readFile(int fd, void *buf) {
    // Attempt store adthewholefilefromfiledescriptor fd into the buffer starting at buf. On success, the number of bytes read
    // is returned and -1 otherwise. You can assume file size will be less than 1KB.
    char recvline[NETWORK_BUFFER_LENGTH];
    bzero(recvline, NETWORK_BUFFER_LENGTH);
    char message[NETWORK_BUFFER_LENGTH];
    // int num_bytes;
    int socketDescriptor;

    socketDescriptor = connectToServer();
    if (socketDescriptor == -1) {
        return -1;
    }

    puts("connected");

    sprintf(message, "READ %d", fd);

    sendto(socketDescriptor, message, strlen(message), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    printf("->%s\n", message);

    int bytesRead = recvfrom(socketDescriptor, recvline, NETWORK_BUFFER_LENGTH, 0, NULL, NULL);
    if (bytesRead < 0) {
        perror("Failed to read from socket");
        exit(1);
    }
    if (bytesRead == 0) {
        puts("got a zero length read, weird");
        return -1;
    }
    printf("bytes read: %d\n", bytesRead);
    // num_bytes = recvfrom(socketDescriptor, recvline, NETWORK_BUFFER_LENGTH, 0, NULL, NULL);
    // recvline[num_bytes] = 0;

    printf("<-'%s'\n", recvline);

    close(socketDescriptor);
    puts("closed");

    char *filedata;
    // protocol: "OK ...filedata..."
    if (strncmp(recvline, "OK", 2) == 0) {
        // ok
        filedata = recvline + 3;
        strcpy(buf, filedata);
        //replace with bytes read
        return bytesRead - 3;
    } else {
        // error
        return -1;
    }
}

int writeFile(int fd, void *buf) {
    char recvline[NETWORK_BUFFER_LENGTH];
    bzero(recvline, NETWORK_BUFFER_LENGTH);
    char message[NETWORK_BUFFER_LENGTH];
    int socketDescriptor;

    socketDescriptor = connectToServer();
    if (socketDescriptor == -1) {
        return -1;
    }
    puts("connected");

    sprintf(message, "WRITE %d %s", fd, buf);

    sendto(socketDescriptor, message, strlen(message), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    printf("->%s\n", message);

    int bytesRead = recvfrom(socketDescriptor, recvline, NETWORK_BUFFER_LENGTH, 0, NULL, NULL);
    if (bytesRead < 0) {
        perror("Failed to read from socket");
        exit(1);
    }
    if (bytesRead == 0) {
        puts("got a zero length read, weird");
        return -1;
    }
    printf("bytes read: %d\n", bytesRead);
    // num_bytes = recvfrom(socketDescriptor, recvline, NETWORK_BUFFER_LENGTH, 0, NULL, NULL);
    // recvline[num_bytes] = 0;

    printf("<-'%s'\n", recvline);

    close(socketDescriptor);
    puts("closed");
    char *filedata;
    // protocol: "OK <number of bytes written>"
    if (strncmp(recvline, "OK", 2) == 0) {
        // ok
        char *bytesWritten = recvline + strlen("OK ");
        //replace with bytes read
        return atoi(bytesWritten);
    } else {
        // error
        return -1;
    }
}

int statFile(int fd, struct fileStat *buf) {
    int socketDescriptor = connectToServer();
    if (socketDescriptor == -1) {
        return -1;
    }

    char request[NETWORK_BUFFER_LENGTH];
    sprintf(request, "STAT %d", fd);

    write(socketDescriptor, request, strlen(request));
    printf("->%s\n", request);

    char response[NETWORK_BUFFER_LENGTH];
    int bytesRead = recvfrom(socketDescriptor, response, NETWORK_BUFFER_LENGTH, 0, NULL, NULL);
    if (bytesRead == -1) {
        perror("Failed to read from socket");
        return -1;
    } else if (bytesRead == 0) {
        puts("weird, got a zero length read");
        return -1;
    }

    if (strncmp("OK", response, 2) != 0) {
        puts("STAT request failed");
        return -1;
    }

    char *statDataString = response + strlen("OK ");

    if (strncmp(response, "OK", 2) == 0) {

        //memcpy(buf, statDataString, sizeof(buf));
        buf->fileSize = atoll(strtok(statDataString," "));
        buf->creationTime = atoll(strtok(NULL," "));
        buf->accessTime = atoll(strtok(NULL," "));
        buf->modificationTime = atoll(strtok(NULL," "));
        //printf("FROM CLIENT, statsize: %lld\n",buf->fileSize);
        //printf("FROM CLIENT, creationTime: %lld\n",buf->creationTime);
        //printf("FROM CLIENT, accessTime: %lld\n",buf->accessTime);
        //printf("FROM CLIENT, modificationTime: %lld\n",buf->modificationTime);
        return 0;

    } else {
        // error
        return -1;
    }

    return 0;
}

int closeFile(int fd) {
    char recvline[NETWORK_BUFFER_LENGTH];
    bzero(recvline, NETWORK_BUFFER_LENGTH);
    char message[NETWORK_BUFFER_LENGTH];
    int socketDescriptor;

    socketDescriptor = connectToServer();
    if (socketDescriptor == -1) {
        return -1;
    }

    puts("connected");

    sprintf(message, "CLOSE %d", fd);
    sendto(socketDescriptor, message, strlen(message), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    printf("->%s\n", message);

    int bytesRead = recvfrom(socketDescriptor, recvline, NETWORK_BUFFER_LENGTH, 0, NULL, NULL);
    if (bytesRead < 0) {
        perror("Failed to read from socket");
        exit(1);
    }
    if (bytesRead == 0) {
        puts("got a zero length read, weird");
        return -1;
    }
    printf("bytes read: %d\n", bytesRead);
    printf("<-'%s'\n", recvline);
    close(socketDescriptor);
    puts("closed");

    if (strncmp(recvline, "OK", 2) == 0) {
        return 0;
    } else {
        puts("Malformed response");
        return -1;
    }
}
