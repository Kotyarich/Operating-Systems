#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

#define USE_ERR -4
#define SENDTO_ERR -5

extern int errno;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("specify message");
        return USE_ERR;
    }

    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket err");
        return SOCKET_ERR;
    }

    struct sockaddr sock_addr = {};
    sock_addr.sa_family = AF_UNIX;
    strcpy(sock_addr.sa_data, SOCKET_NAME);

    char msg[MSG_LEN];
    sprintf(msg, "pid[%d]: %s", getpid(), argv[1]);
    ssize_t err = sendto(sock, msg, strlen(msg), 0, &sock_addr, sizeof(sock_addr));
    if (err == -1) {
        perror("sendto error");
        return SENDTO_ERR;
    }

    close(sock);
    return 0;
}
