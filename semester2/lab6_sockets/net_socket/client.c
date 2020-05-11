#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"

#define SENDTO_ERR -202
#define CONNECT_ERR -203
#define GETHOST_ERR -204

#define MSGS_NUM 6

int main() {
    srand(time(NULL));
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket err");
        return SOCKET_ERR;
    }

    struct hostent *server = gethostbyname(HOST);
    if (!server) {
        perror("gethostbyname error");
        return GETHOST_ERR;
    }

    struct sockaddr_in sock_addr = {};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(PORT);
    sock_addr.sin_addr = *((struct in_addr*) server->h_addr_list[0]);

    if (connect(sock, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0) {
        perror("connect error");
        return CONNECT_ERR;
    }

    char msg_buffer[MSG_LEN];
    for (size_t i = 0; i < MSGS_NUM; i++) {
        sprintf(msg_buffer, "[%d]: My message number %ld\n", getpid(), i);
        ssize_t err = sendto(sock, msg_buffer, strlen(msg_buffer), 0, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
        if (err == -1) {
            close(sock);
            perror("sendto error");
            return SENDTO_ERR;
        }
        printf("Sent message number %ld\n", i);
        sleep(1 + rand() % 2);
    }

    return 0;
}
