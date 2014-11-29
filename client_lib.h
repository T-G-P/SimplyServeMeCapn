#ifndef client-lib_H
#define client-lib_H

void setServer (char* serverIP, int port);

int openFile (char* name);

int readFile (int fd, void* buf);

int writeFile (int fd, void* buf);

int statFile (int fd, struct fileStat *buf);

int closeFile (int fd);

void connectToServer(void);

#endif
