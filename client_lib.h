#ifndef client_lib_H
#define client_lib_H

struct fileStat{
    int fileSize;
    int creationTime;
    int accessTime;
    int modificationTime;
};

void setServer (char* serverIP, int port);

int openFile (char* name);

int readFile (int fd, void* buf);

int writeFile (int fd, void* buf);

int statFile (int fd, struct fileStat *buf);

int closeFile (int fd);

int connectToServer(void);

#endif
