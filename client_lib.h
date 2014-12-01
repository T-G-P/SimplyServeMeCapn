#ifndef client_lib_H
#define client_lib_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct fileStat {
    off_t fileSize;
    time_t creationTime;
    time_t accessTime;
    time_t modificationTime;
};

void setServer(char *serverIP, int port);

int openFile(char *name);

int readFile(int fd, void *buf);

int writeFile(int fd, void *buf);

int statFile(int fd, struct fileStat *buf);

int closeFile(int fd);

int connectToServer(void);

int resolve(char *address);

#endif
