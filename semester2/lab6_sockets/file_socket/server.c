#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"

void close_socket(int s) {
    static int sock;
    if (s) {
        sock = s;
        return;
    }
    close(sock);
    unlink(SOCKET_NAME);
}

void signal_handler(int sig) {
    printf("closing");
    close_socket(0);
}

int main() {
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket error");
        return SOCKET_ERR;
    }
    // for initialising static sock variable
    close_socket(sock);

    struct sockaddr sock_addr = {};
    sock_addr.sa_family = AF_UNIX;
    strcpy(sock_addr.sa_data, SOCKET_NAME);

    if (bind(sock, &sock_addr, sizeof(sock_addr)) < 0) {
        close_socket(0);
        perror("bind error");
        return BIND_ERR;
    }

    signal(SIGINT, signal_handler);

    printf("waiting for any clients\n\n");

    char msg[MSG_LEN];
    while (1) {
        ssize_t rec_len = recv(sock, msg, sizeof(msg), 0);
        if (rec_len == -1) {
            close_socket(0);
            printf("recv error");
            return RECV_ERR;
        }

        msg[rec_len] = '\n';
        msg[rec_len + 1] = 0;
        printf("Client's message: %s", msg);
    }
}
