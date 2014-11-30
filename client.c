#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "client.h"
#include "client_lib.h"

/*
• It should take the server IP, the server port and a valid file name as argument.
• Call setServer with the server information.
• Open a file named "file.in" using openFile() API.
• Copy the content of the file that is taken as argument, into the file "file.in" using writeFile().
• Using statFile() print information of "file.in"
• Open another file "reverse.in".
• Read the content of the file "file.in" using readFile() API.
• Copy the content of the file "file.in" in reverse order into the file "reverse.in".
• Using statFile() print information of "reverse.in".
• Close the files.
*/

int main(int argc, char **argv) {
    int portNum;
    char *hostName;
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
    openFile("file.in");
    //openFile("/etc/shadow");

}

