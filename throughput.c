#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "clientSNFS.h"
#include <pthread.h>

float getcpu_speed(){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    const char* delim = ":\t";
    char * token = NULL;
    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL){
        return 0;
    }
    //Tokenizing each line until the cpu speed is found. Once the cpu speed is found, it gets returned
    while ((read = getline(&line, &len, fp)) != -1) {
        token = strtok(line, delim);
        if(strcmp(line, "cpu MHz") == 0){
            token = strtok(NULL," \t:");
            return atof(token);
        }
        else{
            token = strtok(NULL,delim);
        }
    }
    if (line){
        free(line);
        return 0;
    }
}

inline unsigned long long rdtsc(){
    unsigned long long cycle;
    __asm__ __volatile__("cpuid");
    __asm__ __volatile__("rdtsc" : "=r" (cycle): : );
    return cycle;
}

char *reverseBuf(char *buf){
    int i,j=0;
    char *str = (char *)malloc(strlen(buf));
    char *result = (char *)malloc(strlen(buf));
    strcpy(str,buf);
    strcpy(result,buf);
    for(i=strlen(str); i>=0; i--){
        if(str[i] !='\0'){
            while(j<strlen(str)){
                result[j] = str[i];
                j++;
                break;
            }
        }
    }
    return result;
}

void *run(){
    char *file1 = "file1.txt";
    char *file2 = "file2.txt";
    //int fd2 = openFile("/etc/shadow");
    //int fd3 = openFile("filename.in");

    // printf("got file descriptor: %d\n", fd);
    char *buf[2000];
    char *writeBuf = "This is a buf";
    struct fileStat *fileBuffer = malloc(sizeof(struct fileStat));
    struct fileStat *fileBuffer2 = malloc(sizeof(struct fileStat));

    int fd = openFile(file1);
    writeFile(fd, writeBuf);
    statFile(fd, fileBuffer);

    printf("File Size: %lld\n", (long long)fileBuffer->fileSize);
    printf("Creation Time: %lld\n", (long long)fileBuffer->creationTime);
    printf("Modifiction TIme: %lld\n", (long long)fileBuffer->modificationTime);
    printf("Access Time: %lld\n", (long long)fileBuffer->accessTime);

    int fd2 = openFile(file2);
    readFile(fd, buf);
    writeFile(fd2, reverseBuf(writeBuf));
    statFile(fd2, fileBuffer2);

    printf("File Size: %lld\n", (long long)fileBuffer2->fileSize);
    printf("Creation Time: %lld\n", (long long)fileBuffer2->creationTime);
    printf("Modifiction TIme: %lld\n", (long long)fileBuffer2->modificationTime);
    printf("Access Time: %lld\n", (long long)fileBuffer2->accessTime);

    closeFile(fd);
    closeFile(fd2);

    return;
}

int main(int argc, char **argv) {
    int portNum;
    char *hostName;
    float cpu_speed;
    if (argc != 5) {
        printf("Invalid # of arguments\n");
        return 1;
    }
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-port") == 0) {
            portNum = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-host") == 0) {
            hostName = argv[i + 1];
            i++;
        } else {
            printf("unknown flag %s\n", argv[i]);
            return 1;
        }
    }
    setServer(hostName, portNum);
    pthread_t runThread;
    pthread_t thread_pool[20];

    int j,x,cycle_time;
    int start_time = rdtsc();
    for (j = 0; j < 20; j++){
        pthread_create(&thread_pool[j],NULL,run,NULL);
    }
    for(x = 0; x < 20; x++){
        pthread_join(thread_pool[x], NULL);
    }
    int end_time = rdtsc();
    cpu_speed = getcpu_speed()*1000000;
    cycle_time = end_time-start_time;
    printf("\n\nmicroseconds for multi-threaded SNFS: %f\n",(cycle_time*1000000)/(cpu_speed));

    return 0;
}
