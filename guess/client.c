#include "io.h"
#include "numbers.h"

#include <errno.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>


void PrintUsage(const char* prog) {
    fprintf(stderr, "=== number guessing client ===\n");
    fprintf(stderr, "Usage: %s UNIX_SOCKET_PATH \n\n", prog);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage(argv[0]);
        return 2;
    }

    const char* socketPath = argv[1];

    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    if (strlen(socketPath) >= sizeof(remote.sun_path)) {
        fprintf(stderr, "path '%s' is too long for UNIX domain socket\n", socketPath);
        return 1;
    }
    strcpy(remote.sun_path, socketPath);

    int fd;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    printf("Trying to connect...\n");


    socklen_t localLen = sizeof(remote);
    if (connect(fd, (struct sockaddr *)&remote, localLen) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected.\n");


    char number[100];
    while (true) {
        fgets(number, 100, stdin);
        uint32_t number_to_send = (unsigned int)ntohl(atoi(number));
        if (!SendAll(fd, (char*) &number_to_send, sizeof(number_to_send))) {
            break;
        }
        char sign[1];
        if (!RecvAll(fd, &sign, sizeof(sign))) {
            break;
        }

        if (sign[0] == '=') {
            close(fd);
            printf("%s \n", number);
            return 0;
        }
        fprintf(stderr, "%c \n", sign[0]);
    }

    close(fd);
    return 1;
}