#ifndef server_H
#define server_H

struct fileStat {
    off_t fileSize;
    time_t creationTime;
    time_t accessTime;
    time_t modificationTime;
};

void *connectionHandler(void *);

#endif
